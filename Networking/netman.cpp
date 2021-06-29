#include <iostream>
#include <string>
#include <memory>

#include "../utility.h"
#include "net_hdr.h"
#include "netman.h"

net_stream::net_stream(running_hdr**& _rhdr, startup_hdr**& _shdr) {
	this->rhdr = _rhdr;
	this->shdr = _shdr;

	this->stream_buffer = new unsigned char[256];
}
running_hdr* net_stream::get_rhdr() {
	if (this->rhdr)
		return *(this->rhdr);
	else
		return nullptr;
}
startup_hdr* net_stream::get_shdr() {
	if (this->shdr)
		return *(this->shdr);
	else
		return nullptr;
}
void net_stream::destroy() {
	(*this->rhdr)->stop_mtx.lock();
	(*this->rhdr)->stopRequested = true;
	(*this->rhdr)->stop_mtx.unlock();

	if (this->rhdr)
		delete (*this->rhdr); delete this->rhdr;
	if (this->shdr)
		delete (*this->shdr); delete this->shdr;
	if (this->stream_buffer)
		delete[] this->stream_buffer;
}

void netman::openStream(net_stream*& stream, size_t id) {
	if (!stream)
		return;

	if (!this->streams.count(id)) {
		this->streams[id] = stream;
	}
	else {
		this->closeStream(id);
		this->streams[id] = stream;
	}
}
void netman::closeStream(size_t id) {
	if (this->streams.count(id)) {
		this->streams[id]->destroy();
		delete this->streams[id];
	}
}
net_stream* netman::getStream(size_t id) {
	if (!this->streams.count(id))
		return nullptr;

	return this->streams[id];
}
void netman::destroy() {
	for (auto const& pair : this->streams) {
		pair.second->destroy();
		delete this->streams[pair.first];
	}
}