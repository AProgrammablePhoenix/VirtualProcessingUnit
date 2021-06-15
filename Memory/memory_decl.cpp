#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "dyn_vars.h"
#include "mem_arrays.h"
#include "mem_structs.h"
#include "memory_decl.h"

void m_container::set(unsigned char data[]) {
	delete[] this->obj;
	obj = new unsigned char[sizeof(size_t)];
	this->obj = data;
}
void m_container::set(unsigned char data[], size_t length) {
	delete[] this->obj;
	obj = new unsigned char[length];
	this->obj = data;
}
unsigned char* m_container::get() {
	return this->obj;
}

memory::memory(regs* _registers) {
	this->_arrays = mem_arrays(_registers);
	this->_dynvars = mem_dyn_vars(_registers);
	this->_structs = mem_structs(_registers, this);
	this->init();
}
void memory::push(unsigned char* data, size_t count = 8) {
	if (this->stacktop + count - 1 < this->stacksize) {
		for (size_t addr = this->stacktop; addr < this->stacktop + count; addr++) {
			this->_newmem[addr] = data[addr - this->stacktop];
		}
		this->stacktop += count;
	}
}
void memory::pop(unsigned char* data, size_t count = 8) {
	if (this->stacktop - count >= 0) {
		for (size_t addr = this->stacktop - 1, oaddr = count - 1;
			addr >= this->stacktop - count && addr < this->stacksize && oaddr >= 0;
			addr--, oaddr--) {
				data[oaddr] = this->_newmem[addr];
				this->_newmem[addr] = 0;
		}
		this->stacktop -= count;
	}
}

void memory::_SMS(size_t newlen) {
	if (!this->resized && newlen > _newmemlen) {
		unsigned char* temp = new unsigned char[newlen + this->stacksize + this->rozsize];
		std::copy(this->_newmem, this->_newmem + this->stacksize + this->rozsize + 2048, temp);
		delete[] this->_newmem;
		this->_newmem = temp;
		this->_newmemlen = newlen;
		this->resized = true;
	}
}
void memory::_NMS(unsigned char* data, size_t count, size_t addr) {
	if (addr + count - 1 > this->_newmemlen)
		return;
	for (size_t i = addr, j = 0; i < addr + count && j < count; i++, j++) {
		this->_newmem[i + this->stacksize + this->rozsize] = data[j];
	}
}
void memory::_NMG(unsigned char* data, size_t count, size_t addr) {
	if (addr + count - 1 > this->_newmemlen)
		return;
	for (size_t i = addr, j = 0; i < addr + count && j < count; i++, j++) {
		data[j] = this->_newmem[i + this->stacksize + this->rozsize];
	}
}

void memory::_RSROZ(size_t newlen) {
	if (newlen > this->rozsize) {
		unsigned char* temp = new unsigned char[this->stacksize + newlen + this->_newmemlen];
		std::copy(this->_newmem, this->_newmem + this->stacksize + this->rozsize + this->_newmemlen, temp);
		delete[] this->_newmem;
		this->_newmem = temp;
		this->rozsize = newlen;
	}
}
void memory::_ROZVS(unsigned char* data, size_t count) {
	if (this->rozstacktop + count - 1 < this->rozsize) {
		for (size_t addr = this->rozstacktop; addr < this->rozstacktop + count; addr++) {
			this->_newmem[this->rozoffset + addr] = data[addr - this->rozstacktop];
		}
		this->rozstacktop += count;
	}
	else {
		this->_RSROZ(this->rozsize + count);
		for (size_t addr = this->rozstacktop; addr < this->rozstacktop + count; addr++) {
			this->_newmem[this->rozoffset + addr] = data[addr - this->rozstacktop];
		}
		this->rozstacktop += count;
	}
}
void memory::_ROZVG(unsigned char* data, size_t count, size_t addr) {
	if (addr + count - 1 > this->rozsize)
		return;
	for (size_t i = addr, j = 0; i < addr + count && j < count; i++, j++) {
		data[j] = this->_newmem[i + this->stacksize];
	}
}
size_t memory::_ROZGST() {
	return this->rozstacktop;
}

void memory::destroy() {
	this->_arrays.destroy();
	delete[] this->_newmem;
}
void memory::init() {
	this->_newmem = new unsigned char[this->_newmemlen + this->rozsize + this->stacksize];

	m_container start_block;
	start_block.set((unsigned char*)"NULL_AREA[MEMORY_START]");
	_memory.push_back(start_block);
}