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
void memory::push(unsigned char data[]) {
	m_container buffer;
	buffer.set(data);
	this->_memory.push_back(buffer);
}
void memory::push(unsigned char data[], size_t length) {
	m_container buffer;
	buffer.set(data, length);
	this->_memory.push_back(buffer);
}
unsigned char* memory::pop() {
	if (this->_memory.size() < 2) {
		return NULL;
	}
	else {
		m_container buffer = this->_memory[this->_memory.size() - 1];
		this->_memory.pop_back();
		return buffer.get();
	}
}
unsigned char* memory::access(size_t addr) {
	if (this->_memory.size() < 2 || this->_memory.size() <= addr) {
		return NULL;
	}
	else {
		return this->_memory[addr].get();
	}
}
void memory::set(unsigned char *data, size_t addr) {
	if (addr >= this->_memory.size() || addr == 0) {
		return;
	}
	else {
		this->_memory[addr].set(data);
		return;
	}
}
void memory::set(unsigned char* data, size_t length, size_t addr) {
	if (addr >= this->_memory.size() || addr == 0) {
		return;
	}
	else {
		this->_memory[addr].set(data, length);
	}
}

void memory::_SMS(size_t newlen) {
	if (!this->resized && newlen > _newmemlen) {
		unsigned char* temp = new unsigned char[newlen + this->stacksize];
		std::copy(this->_newmem, this->_newmem + 2048, temp);
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
