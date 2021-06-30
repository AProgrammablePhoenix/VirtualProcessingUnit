#include "../utility.h"

#if defined(ISWIN)

#pragma comment(lib, "ws2_32.lib")

#include "net_hdr.h"

#include <functional>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <windows.h>
//#include <winSock2.h>

static std::vector<unsigned char*> receivedBytes;

void recvThread(SOCKET& hSocket, running_hdr*& rhdr) {
	unsigned char* buffer = nullptr;

	while (true) {
		rhdr->ecrecv_mtx.lock();

		if (rhdr->stopReceiving) {
			rhdr->ecrecv_mtx.unlock();
			return;
		}

		rhdr->ecrecv_mtx.unlock();

		buffer = new unsigned char[256];
		int bytesReceived = recv(hSocket, (char*)buffer, 256, 0);

		if (bytesReceived == 0) {
			delete[] buffer;
			break;
		}
		else if (bytesReceived == SOCKET_ERROR) {
			delete[] buffer;
		}
		else {
			receivedBytes.push_back(buffer);

			rhdr->hrecv_mtx.lock();
			rhdr->hasReceived = true;
			rhdr->hrecv_mtx.unlock();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}
void sendFct(SOCKET& hSocket, running_hdr*& rhdr, unsigned char* data, size_t count) {
	rhdr->sent_mtx.lock();
	rhdr->bufferSent = true;

	if (send(hSocket, (const char*)data, (int)count, 0)) {
		// Handling error code
		rhdr->sent_mtx.unlock();
		return;
	}

	rhdr->sent_mtx.unlock();
}

void treatRcvMsg(running_hdr*& nrh) {
	nrh->msg_code = msg_codes::NOP;
	
	if ((*nrh->transferBuffer) != nullptr)
		delete[] (*nrh->transferBuffer);

	(*nrh->transferBuffer) = new unsigned char[256];

	if (receivedBytes.size() < 1 || receivedBytes.empty()) {
		std::fill((*nrh->transferBuffer), (*nrh->transferBuffer) + 256, 0);
		return;
	}

	mp_memcpy<unsigned char, unsigned char>(receivedBytes[0], (*nrh->transferBuffer), 256);

	delete[] receivedBytes[0];
	receivedBytes.erase(receivedBytes.begin());
}

void winnet_poststartup(startup_hdr*& startup_header, running_hdr*& net_run_hdr) {
	SOCKET hSocket;
	hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (hSocket == INVALID_SOCKET) {
		std::cout << "Error while creating network socket" << std::endl;
		return;
	}
	else {
		sockaddr_in thisSockAddr, recvSockAddr;

		recvSockAddr.sin_family = AF_INET;
		recvSockAddr.sin_port = htons(startup_header->recvPort);
		recvSockAddr.sin_addr.S_un.S_addr = inet_addr(startup_header->recvAddr.c_str());

		thisSockAddr.sin_family = AF_INET;
		thisSockAddr.sin_port = htons(startup_header->thisPort);
		thisSockAddr.sin_addr.S_un.S_addr = inet_addr(startup_header->thisAddr.c_str());

		if (bind(hSocket, (sockaddr*)(&thisSockAddr), sizeof(sockaddr_in)) != 0) {
			std::cout << "Error while binding connection" << std::endl;
			closesocket(hSocket);
			return;
		}

		if (connect(hSocket, (sockaddr*)(&recvSockAddr), sizeof(recvSockAddr)) != 0) {
			std::cout << "Error while establishing connection" << std::endl;
			closesocket(hSocket);
			return;
		}
		else {

			std::thread Trecv(recvThread, std::ref(hSocket), std::ref(net_run_hdr));
			while (true) {
				net_run_hdr->recv_mtx.lock();

				if (net_run_hdr->msg_code == msg_codes::treatReceivedBuffer) {
					treatRcvMsg(net_run_hdr);
				}
				else if (net_run_hdr->msg_code == msg_codes::sendBuffer) {
					sendFct(hSocket, net_run_hdr, (*net_run_hdr->transferBuffer), net_run_hdr->trsfrBufferLen);
				}

				net_run_hdr->msg_code = msg_codes::NOP;

				net_run_hdr->recv_mtx.unlock();
				net_run_hdr->stop_mtx.lock();

				if (net_run_hdr->stopRequested) {
					net_run_hdr->stop_mtx.unlock();
					break;
				}

				net_run_hdr->stop_mtx.unlock();
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}

			net_run_hdr->ecrecv_mtx.lock();
			net_run_hdr->stopReceiving = true;
			net_run_hdr->ecrecv_mtx.unlock();

			Trecv.detach();
			closesocket(hSocket);
		}
	}
}

void netint_submain(startup_hdr*& startup_header, running_hdr*& net_run_hdr) {
	const int iMinWinsockVer = 2;
	WSADATA wsadata;

	if (!WSAStartup(MAKEWORD(iMinWinsockVer, 0), &wsadata)) {
		if (LOBYTE(wsadata.wVersion) >= iMinWinsockVer) {
			winnet_poststartup(startup_header, net_run_hdr);
		}
		else {
			std::cout << "Network API Version is not compatible" << std::endl;
			return;
		}

		if (WSACleanup() != 0) {
			std::cout << "Failed cleaning up networking interface" << std::endl;
			return;
		}

		while (receivedBytes.size() > 1) {
			delete[] receivedBytes[0];
			receivedBytes.erase(receivedBytes.begin());
		}
	}
	else {
		std::cout << "Failed launching networking interface startup" << std::endl;
		return;
	}
}

#endif