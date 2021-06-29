#include <iostream>
#include <string>
#include <vector>

#include "dyn_vars.h"
#include "mem_arrays.h"
#include "mem_structs.h"
#include "memory_decl.h"

memory::memory(regs* _registers) {
	this->_arrays = mem_arrays(_registers);
	this->_dynvars = mem_dyn_vars(_registers);
	this->_structs = mem_structs(_registers, this);
	this->init();
}
void memory::push(unsigned char* data, size_t count = 8) {
	if (this->stacktop + count - 1 < this->stacksize) {
		for (size_t addr = this->stacktop; addr < this->stacktop + count; addr++) {
			this->_memory[addr] = data[addr - this->stacktop];
		}
		this->stacktop += count;
	}
}
void memory::pop(unsigned char* data, size_t count = 8) {
	if (this->stacktop - count >= 0) {
		for (size_t addr = this->stacktop - 1, oaddr = count - 1;
			addr >= this->stacktop - count && addr < this->stacksize && oaddr >= 0;
			addr--, oaddr--) {
				data[oaddr] = this->_memory[addr];
				this->_memory[addr] = 0;
		}
		this->stacktop -= count;
	}
}

void memory::_SMS(size_t newlen) {
	if (!this->resized && newlen > _memlen) {
		unsigned char* temp = new unsigned char[newlen + this->stacksize + this->rozsize];
		std::copy(this->_memory, this->_memory + this->stacksize + this->rozsize + 2048, temp);
		delete[] this->_memory;
		this->_memory = temp;

		memset(this->_memory + this->stacksize + this->rozsize + this->_memlen, 0, newlen - this->_memlen);

		this->_memlen = newlen;
		this->resized = true;
	}
}
void memory::_MS(unsigned char* data, size_t count, size_t addr) {
	if (addr + count - 1 > this->_memlen)
		return;
	for (size_t i = addr, j = 0; i < addr + count && j < count; i++, j++) {
		this->_memory[i + this->stacksize + this->rozsize] = data[j];
	}
}
void memory::_MG(unsigned char* data, size_t count, size_t addr) {
	if (addr + count - 1 > this->_memlen)
		return;
	for (size_t i = addr, j = 0; i < addr + count && j < count; i++, j++) {
		data[j] = this->_memory[i + this->stacksize + this->rozsize];
	}
}

void memory::_RSROZ(size_t newlen) {
	if (newlen > this->rozsize) {
		unsigned char* temp = new unsigned char[this->stacksize + newlen + this->_memlen];
		std::copy(this->_memory, this->_memory + this->stacksize + this->rozsize + this->_memlen, temp);
		delete[] this->_memory;

		memset(this->_memory + this->rozoffset + this->rozsize, 0, newlen - this->rozsize);

		this->_memory = temp;
		this->rozsize = newlen;
	}
}
void memory::_ROZVS(unsigned char* data, size_t count) {
	if (this->rozstacktop + count - 1 < this->rozsize) {
		for (size_t addr = this->rozstacktop; addr < this->rozstacktop + count; addr++) {
			this->_memory[this->rozoffset + addr] = data[addr - this->rozstacktop];
		}
		this->rozstacktop += count;
	}
	else {
		this->_RSROZ(this->rozsize + count);
		for (size_t addr = this->rozstacktop; addr < this->rozstacktop + count; addr++) {
			this->_memory[this->rozoffset + addr] = data[addr - this->rozstacktop];
		}
		this->rozstacktop += count;
	}
}
void memory::_ROZVG(unsigned char* data, size_t count, size_t addr) {
	if (addr + count - 1 > this->rozsize)
		return;
	for (size_t i = addr, j = 0; i < addr + count && j < count; i++, j++) {
		data[j] = this->_memory[i + this->stacksize];
	}
}
size_t memory::_ROZGST() {
	return this->rozstacktop;
}

void memory::destroy() {
	this->_arrays.destroy();
	this->_netman.destroy();
	delete[] this->_memory;
}
void memory::init() {
	this->_memory = new unsigned char[this->_memlen + this->rozsize + this->stacksize];
	memset(this->_memory, 0, this->_memlen + this->rozsize + this->stacksize);
}