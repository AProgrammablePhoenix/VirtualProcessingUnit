#include "../utility.h"

#if defined(ISWIN)

#pragma comment(lib, "ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "net_hdr.h"

#include <functional>
#include <iostream>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <winSock2.h>
#include <ws2tcpip.h>
#include <windows.h>

struct inet_data {
	std::vector<unsigned char*> receivedBytes;
	std::map<size_t, sockaddr_in*> endpoints;
};

inline bool isIPAddr(char* testStr) {
	sockaddr_in temp;
	return (inet_pton(AF_INET, testStr, &(temp.sin_addr))) > 0;
}

void recvThread(SOCKET& hSocket, running_hdr*& rhdr, inet_data*& idata) {
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
			idata->receivedBytes.push_back(buffer);

			rhdr->hrecv_mtx.lock();
			rhdr->hasReceived = true;
			rhdr->hrecv_mtx.unlock();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}
void sendFct(SOCKET& hSocket, running_hdr*& rhdr, unsigned char* data, size_t count, inet_data*& idata) {
	rhdr->sent_mtx.lock();
	sockaddr_in* ep_sockaddr = idata->endpoints[rhdr->selectedEP];

	if (sendto(hSocket, (const char*)data, (int)count, 0, (sockaddr*)ep_sockaddr, sizeof(*ep_sockaddr)) != (int)count) {
		rhdr->bufferSent = true;
		rhdr->sent_mtx.unlock();
		return;
	}

	rhdr->bufferSent = true;
	rhdr->sent_mtx.unlock();
}

void treatRcvMsg(running_hdr*& nrh, inet_data*& idata) {
	nrh->msg_code = msg_codes::NOP;
	
	if ((*nrh->transferBuffer) != nullptr)
		delete[] (*nrh->transferBuffer);

	(*nrh->transferBuffer) = new unsigned char[256];

	if (idata->receivedBytes.size() < 1 || idata->receivedBytes.empty()) {
		std::fill((*nrh->transferBuffer), (*nrh->transferBuffer) + 256, 0);
		return;
	}

	mp_memcpy<unsigned char, unsigned char>(idata->receivedBytes[0], (*nrh->transferBuffer), 256);

	delete[] idata->receivedBytes[0];
	idata->receivedBytes.erase(idata->receivedBytes.begin());
}
void addEndpoint(running_hdr*& rhdr, ip_endpoint& ep_data, inet_data*& idata) {
	sockaddr_in ep_sockaddr;

	memset((char*)&ep_sockaddr, 0, sizeof(ep_sockaddr));
	ep_sockaddr.sin_family = AF_INET;
	ep_sockaddr.sin_port = htons(ep_data.port);

	if (isIPAddr((char*)ep_data.ipAddr->c_str())) {
		ep_sockaddr.sin_addr.s_addr = inet_addr(ep_data.ipAddr->c_str());
	} else {
		hostent* ep_recp = gethostbyname(ep_data.ipAddr->c_str());
		if (ep_recp == NULL) {
			std::cout << "No such external host: " << ep_data.ipAddr << std::endl;
			return;
		} else {
			memmove((char*)(&ep_sockaddr.sin_addr.s_addr), (char*)(ep_recp->h_addr), ep_recp->h_length);
		}
	}

	idata->endpoints[ep_data.id] = new sockaddr_in(ep_sockaddr);
	delete ep_data.ipAddr;
}
void winnet_poststartup(startup_hdr*& startup_header, running_hdr*& net_run_hdr, inet_data*& idata) {
	SOCKET hSocket;
	hSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (hSocket == INVALID_SOCKET) {
		std::cout << "Error while creating network socket" << std::endl;
		return;
	}
	else {
		sockaddr_in thisSockAddr, recvSockAddr;

		recvSockAddr.sin_family = AF_INET;
		recvSockAddr.sin_port = htons(startup_header->recvPort);

		thisSockAddr.sin_family = AF_INET;
		thisSockAddr.sin_port = htons(startup_header->thisPort);
		thisSockAddr.sin_addr.S_un.S_addr = inet_addr(startup_header->thisAddr.c_str());

		if (isIPAddr((char*)startup_header->recvAddr.c_str())) {
			recvSockAddr.sin_addr.s_addr = inet_addr(startup_header->recvAddr.c_str());
		}
		else {
			hostent* serv_recp = gethostbyname(startup_header->recvAddr.c_str());
			if (serv_recp == NULL) {
				std::cout << "No such external host: " << startup_header->recvAddr << std::endl;
				return;
			}
			else {
				memmove((char*)(&recvSockAddr.sin_addr.s_addr), (char*)(serv_recp->h_addr), serv_recp->h_length);
			}
		}

		if (bind(hSocket, (sockaddr*)(&thisSockAddr), sizeof(sockaddr_in)) != 0) {
			std::cout << "Error while binding connection" << std::endl;
			closesocket(hSocket);
			return;
		}

		else {
			idata->endpoints[0] = new sockaddr_in(recvSockAddr);
			std::thread Trecv(recvThread, std::ref(hSocket), std::ref(net_run_hdr), std::ref(idata));
			while (true) {
				net_run_hdr->recv_mtx.lock();

				if (net_run_hdr->msg_code == msg_codes::treatReceivedBuffer) {
					treatRcvMsg(net_run_hdr, idata);
				}
				else if (net_run_hdr->msg_code == msg_codes::sendBuffer) {
					sendFct(hSocket, net_run_hdr, (*net_run_hdr->transferBuffer), net_run_hdr->trsfrBufferLen, idata);
				} else if (net_run_hdr->msg_code == msg_codes::addEndpoint) {
					ip_endpoint ipe;
					unsigned char* container = (*net_run_hdr->transferBuffer);

					memcpy_s(&ipe, sizeof(ip_endpoint), container, sizeof(ip_endpoint));

					addEndpoint(net_run_hdr, ipe, idata);
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
		inet_data* idata = new inet_data;

		if (LOBYTE(wsadata.wVersion) >= iMinWinsockVer) {
			winnet_poststartup(startup_header, net_run_hdr, idata);
		}
		else {
			std::cout << "Network API Version is not compatible" << std::endl;
			delete idata;
			return;
		}

		if (WSACleanup() != 0) {
			std::cout << "Failed cleaning up networking interface" << std::endl;
			delete idata;
			return;
		}

		while (idata->receivedBytes.size() > 1) {
			delete[] idata->receivedBytes[0];
			idata->receivedBytes.erase(idata->receivedBytes.begin());
		}

		std::vector<size_t> ep_keys;
		for (auto const& pair : idata->endpoints)
			ep_keys.push_back(pair.first);

		for (size_t key : ep_keys)
			delete idata->endpoints[key];

		delete idata;
	}
	else {
		std::cout << "Failed launching networking interface startup" << std::endl;
		return;
	}
}

#endif