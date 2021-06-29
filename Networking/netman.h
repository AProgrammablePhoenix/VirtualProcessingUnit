#pragma once

#include <iostream>
#include <string>
#include <map>
#include <memory>

#include "../utility.h"
#include "net_hdr.h"

struct net_stream {
public:
	net_stream(running_hdr**& _rhdr, startup_hdr**& _shdr);

	running_hdr* get_rhdr();
	startup_hdr* get_shdr();

	void destroy();
private:
	unsigned char* stream_buffer = nullptr;

	running_hdr** rhdr = nullptr;
	startup_hdr** shdr = nullptr;
};

struct netman {
public:
	void openStream(net_stream*& stream, size_t id);
	void closeStream(size_t id);

	net_stream* getStream(size_t id);

	void destroy();
private:
	std::map<size_t, net_stream*> streams;
};