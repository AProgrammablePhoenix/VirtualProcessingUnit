#pragma once

#include "../utility.h"

#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

enum class msg_codes {
	NOP					= 0x00,
	treatReceivedBuffer = 0x01,
	sendBuffer			= 0x02,
	addEndpoint			= 0x03
};

struct running_hdr {
	running_hdr(unsigned char** trsfrBuffer, size_t _trsfrBufferLen) {
		this->transferBuffer = trsfrBuffer;
		this->trsfrBufferLen = _trsfrBufferLen;
	}

	std::mutex stop_mtx;
	std::mutex recv_mtx;
	std::mutex hrecv_mtx;
	std::mutex ecrecv_mtx;
	std::mutex sent_mtx;

	bool stopRequested = false;
	bool hasReceived = false;
	bool stopReceiving = false;
	bool bufferSent = false;
	msg_codes msg_code = msg_codes::NOP;

	unsigned char** transferBuffer = nullptr;
	size_t trsfrBufferLen = 256;
	size_t selectedEP = 0;
};

struct startup_hdr {
	startup_hdr(std::string _rAddr, int _rPort, int _sPort) {
		this->recvAddr = _rAddr;
		this->recvPort = _rPort;

		this->thisPort = _sPort;
	}

	const std::string thisAddr = "127.0.0.1";

	std::string recvAddr;
	int recvPort, thisPort;
};

struct ip_endpoint {
	std::string* ipAddr;
	int port;
	size_t id;
};

void netint_submain(startup_hdr*& startup_header, running_hdr*& net_run_hdr);