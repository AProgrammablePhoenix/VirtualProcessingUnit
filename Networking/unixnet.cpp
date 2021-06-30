#include "../utility.h"

#if defined(ISUNIX)

#include "net_hdr.h"

#include <cstdio>
#include <cstdlib>

#include <functional>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

typedef int SOCKET;

int inet_pton(int af, const char* src, void* dst);
void bzero(void* s, size_t n);
void bcopy(const void* src, void* dest, size_t n);
inline bool isIPAddr(char* testStr) {
	sockaddr_in temp;
	return (inet_pton(AF_INET, testStr, &(temp.sin_addr))) > 0;
}

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

void unixnet_poststartup(SOCKET& hSocket, startup_hdr*& startup_header, running_hdr*& net_run_hdr) {
	sockaddr_in thisSockAddr, recvSockAddr;

	bzero((char*)&recvSockAddr, sizeof(recvSockAddr));
	recvSockAddr.sin_family = AF_INET;
	recvSockAddr.sin_port = htons(startup_header->recvPort);

	bzero((char*)&thisSockAddr, sizeof(thisSockAddr));
	thisSockAddr.sin_family = AF_INET;
	thisSockAddr.sin_port = htons(startup_header->thisPort);
	thisSockAddr.sin_addr.s_addr = inet_addr(startup_header->thisAddr.c_str());

	if (isIPAddr((char*)startup_header->recvAddr.c_str())) {
		recvSockAddr.sin_addr.s_addr = inet_addr(startup_header->recvAddr.c_str());
	}
	else {
		hostent* serv_recp = gethostbyname(startup_header->recvAddr.c_str());
		if (serv_recp == NULL) {
			std::cout << "No such external host: " << startup_header->recvAddr << std::endl;
			close(hSocket);
			return;
		}
		else {
			bcopy((char*)(serv_recp->h_addr), (char*)(&recvSockAddr.sin_addr.s_addr), serv_recp->h_length);
		}
	}

	if (bind(hSocket, (sockaddr*)(&thisSockAddr), sizeof(sockaddr_in)) != 0) {
		std::cout << "Error while binding connection" << std::endl;
		close(hSocket);
		return;
	}

	if (connect(hSocket, (sockaddr*)(&recvSockAddr), sizeof(recvSockAddr)) != 0) {
		std::cout << "Error while establishing connection" << std::endl;
		close(hSocket);
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
		close(hSocket);
	}
}

void netint_submain(startup_hdr*& startup_header, running_hdr*& net_run_hdr) {
	SOCKET hSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET) {
		std::cout << "Error while creating network socket" << std::endl;
		return;
	}
	else {
		unixnet_poststartup(hSocket, startup_header, net_run_hdr);
		while (receivedBytes.size() > 1) {
			delete[] receivedBytes[0];
			receivedBytes.erase(receivedBytes.begin());
		}
	}
}

#endif