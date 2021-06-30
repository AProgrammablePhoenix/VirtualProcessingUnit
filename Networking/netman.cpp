#include <iostream>
#include <string>
#include <memory>
#include <thread>

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
		return (*this->rhdr);
	else
		return nullptr;
}
startup_hdr* net_stream::get_shdr() {
	if (this->shdr)
		return (*this->shdr);
	else
		return nullptr;
}
running_hdr** net_stream::get_full_rhdr() {
	if (this->rhdr)
		return this->rhdr;
	else
		return nullptr;
}
startup_hdr** net_stream::get_full_shdr() {
	if (this->shdr)
		return this->shdr;
	else
		return nullptr;
}
void net_stream::destroy(std::thread* net_thread) {
	(*this->rhdr)->stop_mtx.lock();
	(*this->rhdr)->stopRequested = true;
	(*this->rhdr)->stop_mtx.unlock();

	if (net_thread->joinable())
		net_thread->join();

	std::this_thread::sleep_for(std::chrono::milliseconds(200));

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

	running_hdr** rhdr = stream->get_full_rhdr();
	startup_hdr** shdr = stream->get_full_shdr();

	std::thread stream_thread(netint_submain, std::ref(*shdr), std::ref(*rhdr));
	this->threads[id] = std::move(stream_thread);
}
void netman::closeStream(size_t id) {
	if (this->streams.count(id)) {
		this->streams[id]->destroy(&this->threads[id]);
		this->threads.erase(id);
		delete this->streams[id];
		this->streams.erase(id);
	}
}
net_stream* netman::getStream(size_t id) {
	if (!this->streams.count(id))
		return nullptr;

	return this->streams[id];
}
void netman::destroy() {
	// Needed because keys are reased from map later : avoid some memory violations errors
	std::vector<size_t> keys;
	for (auto const& pair : this->streams)
		keys.push_back(pair.first);

	for (size_t key : keys)
		this->closeStream(key);
}