#include <iostream>
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
		unsigned char* temp = new unsigned char[newlen + this->stacksize];
		std::copy(this->_newmem, this->_newmem + 2048 + this->stacksize, temp);
		delete[] this->_newmem;
		this->_newmem = temp;
		this->_newmemlen = newlen;
		this->resized = true;
	}
}
void memory::_NMS(unsigned char* data, size_t count, size_t addr) {
	if (addr > this->_newmemlen)
		return;
	for (size_t i = addr, j = 0; i < addr + count && j < count; i++, j++) {
		this->_newmem[i + this->stacksize] = data[j];
	}
}
void memory::_NMG(unsigned char* data, size_t count, size_t addr) {
	if (addr > this->_newmemlen)
		return;
	for (size_t i = addr, j = 0; i < addr + count && j < count; i++, j++) {
		data[j] = this->_newmem[i + this->stacksize];
	}
}

void memory::destroy() {
	this->_arrays.destroy();
}
void memory::init() {
	this->_newmem = new unsigned char[this->_newmemlen + this->stacksize];

	m_container start_block;
	start_block.set((unsigned char*)"NULL_AREA[MEMORY_START]");
	_memory.push_back(start_block);
}
