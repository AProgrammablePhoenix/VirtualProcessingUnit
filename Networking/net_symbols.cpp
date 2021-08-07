#include <iostream>
#include <string>
#include <memory>

#include "../utility.h"
#include "../Registers/registers_symbols.h"
#include "../Memory/memory_symbols.h"
#include "net_hdr.h"
#include "net_symbols.h"
#include "netman.h"

size_t popMemNum(memory*& mem) {
	unsigned char* temp = new unsigned char[sizeof(size_t)];

	mem->pop(temp, sizeof(size_t));
	size_t value = 0;

	mp_memcpy(temp, &value);
	delete[] temp;

	return value;
}

// Open a network stream
// Argument: ID of stream to open
// Stack: ... recvAddr recvPort thisPort
void net_open(std::shared_ptr<void> stream_id_ptr, regs* registers, memory* mem) {
	arg_tuple varinfos = *std::static_pointer_cast<arg_tuple>(stream_id_ptr);
	unsigned char* temp = new unsigned char[sizeof(size_t)];
	mem->_MG(temp, sizeof(size_t), std::get<0>(varinfos));

	size_t stream_id = ATOULL(temp);
	delete[] temp;

	int thisPort = (int)popMemNum(mem);
	int recvPort = (int)popMemNum(mem);

	std::string saved_sr = registers->sr->get();

	popMemSR(nullptr, registers, mem);
	std::string recvAddr = registers->sr->get();

	registers->sr->set(saved_sr);

	unsigned char* streamBuffer = new unsigned char[256];
	running_hdr** rhdr = new running_hdr*(new running_hdr(&streamBuffer, 256));
	startup_hdr** shdr = new startup_hdr*(new startup_hdr(recvAddr, thisPort, recvPort));

	net_stream* stream = new net_stream(rhdr, shdr);

	mem->_netman.openStream(stream, stream_id);
}
// Close a network stream
// Argument: ID of stream to close
void net_close(std::shared_ptr<void> stream_id_ptr, regs* registers, memory* mem) {
	arg_tuple varinfos = *std::static_pointer_cast<arg_tuple>(stream_id_ptr);
	unsigned char* temp = new unsigned char[sizeof(size_t)];
	mem->_MG(temp, sizeof(size_t), std::get<0>(varinfos));

	size_t stream_id = ATOULL(temp);
	delete[] temp;

	mem->_netman.closeStream(stream_id);
}
// Fetch received data from specified stream id
// Argument: stream id
// Stack: ... Output/Input addr (in memory ; length: 256)
void net_get(std::shared_ptr<void> stream_id_ptr, regs* registers, memory* mem) {
	arg_tuple varinfos = *std::static_pointer_cast<arg_tuple>(stream_id_ptr);
	unsigned char* temp = new unsigned char[sizeof(size_t)];
	mem->_MG(temp, sizeof(size_t), std::get<0>(varinfos));

	size_t stream_id = ATOULL(temp);
	delete[] temp;

	size_t outAddr = popMemNum(mem);

	net_stream* nstream = mem->_netman.getStream(stream_id);
	if (!nstream)
		return;

	running_hdr* rhdr = nstream->get_rhdr();
	if (!rhdr)
		return;

	temp = new unsigned char[256];

	rhdr->transferBuffer = &temp;
	rhdr->recv_mtx.lock();
	rhdr->msg_code = msg_codes::treatReceivedBuffer;
	rhdr->recv_mtx.unlock();

	std::this_thread::sleep_for(std::chrono::milliseconds(200));

	mp_memcpy<unsigned char, unsigned char>((*rhdr->transferBuffer), temp, 256);

	mem->_MS(temp, 256, outAddr);
	delete[] temp;
}
void net_send(std::shared_ptr<void> stream_id_ptr, regs* registers, memory* mem) {
	arg_tuple varinfos = *std::static_pointer_cast<arg_tuple>(stream_id_ptr);
	unsigned char* temp = new unsigned char[sizeof(size_t)];
	mem->_MG(temp, sizeof(size_t), std::get<0>(varinfos));

	size_t stream_id = ATOULL(temp);
	delete[] temp;

	size_t inAddr = popMemNum(mem);
	temp = new unsigned char[256];
	mem->_MG(temp, 256, inAddr);

	net_stream* nstream = mem->_netman.getStream(stream_id);
	if (!nstream) {
		delete[] temp;
		return;
	}

	running_hdr* rhdr = nstream->get_rhdr();
	if (!rhdr) {
		delete[] temp;
		return;
	}

	rhdr->transferBuffer = &temp;
	rhdr->recv_mtx.lock();
	rhdr->msg_code = msg_codes::sendBuffer;
	rhdr->recv_mtx.unlock();

	// Need this loop to keep temp variable alive and to delete it once data is sent
	while (true) {
		rhdr->sent_mtx.lock();
		if (rhdr->bufferSent)
			break;
		rhdr->sent_mtx.unlock();

		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

	rhdr->bufferSent = false;
	rhdr->sent_mtx.unlock();
	delete[] temp;
}
// Check if data has been recerived
// Argument: stream id
// Output in stack: 1 (has received) | 0 (hasn't received anything)
void net_hrecv(std::shared_ptr<void> stream_id_ptr, regs* registers, memory* mem) {
	arg_tuple varinfos = *std::static_pointer_cast<arg_tuple>(stream_id_ptr);
	unsigned char* temp = new unsigned char[sizeof(size_t)];
	mem->_MG(temp, sizeof(size_t), std::get<0>(varinfos));

	size_t stream_id = ATOULL(temp);
	delete[] temp;

	net_stream* nstream = mem->_netman.getStream(stream_id);
	if (!nstream)
		return;

	running_hdr* rhdr = nstream->get_rhdr();
	if (!rhdr)
		return;

	rhdr->hrecv_mtx.lock();

	size_t hrecvd = (size_t)rhdr->hasReceived;

	if (hrecvd)
		rhdr->hasReceived = false;
	
	rhdr->hrecv_mtx.unlock();

	temp = new unsigned char[sizeof(size_t)];
	mp_memcpy<size_t, unsigned char>((size_t*)(&hrecvd), temp, sizeof(size_t));

	mem->push(temp, sizeof(size_t));
	delete[] temp;
}

// Create a network stream endpoint with a specified id (id used to select it)
// Argument: stream id
// Stack ... recvAddr(string) recvPort(unsigned number) endpoint_id
void net_crtep(std::shared_ptr<void> stream_id_ptr, regs* registers, memory* mem) {
	arg_tuple varinfos = *std::static_pointer_cast<arg_tuple>(stream_id_ptr);
	unsigned char* temp = new unsigned char[sizeof(size_t)];
	mem->_MG(temp, sizeof(size_t), std::get<0>(varinfos));

	size_t stream_id = ATOULL(temp);
	delete[] temp;

	size_t ep_id = popMemNum(mem);
	size_t recvPort = popMemNum(mem);

	std::string saved_sr = registers->sr->get();

	popMemSR(nullptr, registers, mem);
	std::string recvAddr = registers->sr->get();

	registers->sr->set(saved_sr);

	net_stream* nstream = mem->_netman.getStream(stream_id);
	if (!nstream)
		return;

	running_hdr** rhdr = nstream->get_full_rhdr();
	if (!rhdr)
		return;

	ip_endpoint ipe;
	ipe.ipAddr = new std::string(recvAddr);
	ipe.port = (int)recvPort;
	ipe.id = ep_id;

	temp = new unsigned char[sizeof(ip_endpoint)];
#if defined(ISWIN)
	memcpy_s(temp, sizeof(ip_endpoint), &ipe, sizeof(ip_endpoint));
#else
	memcpy(temp, &ipe, sizeof(ip_endpoint));
#endif

	unsigned char** saved = (*rhdr)->transferBuffer;
	(*rhdr)->transferBuffer = &temp;

	(*rhdr)->recv_mtx.lock();
	(*rhdr)->msg_code = msg_codes::addEndpoint;
	(*rhdr)->recv_mtx.unlock();

	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	delete[] temp;
	(*rhdr)->transferBuffer = saved;
}
// Select stream endpoint (used to send data)
// Argument: stream id
// Stack: ... endpoint_id
void net_selep(std::shared_ptr<void> stream_id_ptr, regs* registers, memory* mem) {
	arg_tuple varinfos = *std::static_pointer_cast<arg_tuple>(stream_id_ptr);
	unsigned char* temp = new unsigned char[sizeof(size_t)];
	mem->_MG(temp, sizeof(size_t), std::get<0>(varinfos));

	size_t stream_id = ATOULL(temp);
	
	mem->pop(temp, sizeof(size_t));	
	size_t ep_id = ATOULL(temp);
	delete[] temp;

	net_stream* nstream = mem->_netman.getStream(stream_id);
	if (!nstream)
		return;

	running_hdr** rhdr = nstream->get_full_rhdr();
	if (!rhdr)
		return;

	(*rhdr)->selectedEP = ep_id;
}