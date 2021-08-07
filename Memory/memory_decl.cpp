#include <iostream>
#include <string>
#include <vector>

#include "dyn_vars.h"
#include "mem_arrays.h"
#include "mem_structs.h"
#include "memory_decl.h"

namespace {
	inline bool isvalidpush(const size_t& stackbase, const size_t& stacktop, const size_t& stackend, const size_t& stackbgn, const size_t count) {
		if (stacktop > stackbase) // Corrupted stack frame
			return false;
		else if (stacktop < stackend) // Stack overflow
			return false;
		else if (stackbase >= stackbgn) // stack underflow
			return false;
		else if (stacktop - count >= stackbase || stacktop - count < stackend) // future stack overflow
			return false;
		return true;
	}
	inline bool isvalidpop(const size_t& stackbase, const size_t& stacktop, const size_t& stackend, const size_t& stackbgn, const size_t count) {
		if (stacktop > stackbase) // Corrupted stack frame
			return false;
		else if (stacktop < stackend) // Stack overflow
			return false;
		else if (stackbase >= stackbgn) // stack underflow
			return false;
		else if (stacktop + count > stackbase || stacktop + count < stackend || stacktop + count > stackbgn) // future stack underflow
			return false;
		return true;
	}
}

memory::memory(regs* _registers) {
	if (!_registers)
		throw std::invalid_argument("Memory(constructor): Pointer to registers is invalid");

	this->_arrays = mem_arrays(_registers);
	this->_dynvars = mem_dyn_vars(_registers);
	this->_structs = mem_structs(_registers, this);
	this->registers = _registers;
	this->init();
}
void memory::update(regs* _registers) {
	if (!_registers)
		throw std::invalid_argument("Memory(constructor): Pointer to registers is invalid");

	this->registers = _registers;
}
size_t memory::getMemLen() {
	return this->_memlen;
}

void memory::push(unsigned char* data, size_t count = 8) {
	if (isvalidpush(*this->registers->rbp, *this->registers->rsp, *this->registers->rsend, *this->registers->rsbgn, count)) {
		for (size_t addr = *this->registers->rsp, iaddr = 0; addr >= *this->registers->rsp - count && iaddr < count; addr--, iaddr++) {
			this->_memory[this->rozsize + addr] = data[iaddr];
		}
		*this->registers->rsp = *this->registers->rsp - count;
	}
}
void memory::pop(unsigned char* data, size_t count = 8) {
	if (isvalidpop(*this->registers->rbp, *this->registers->rsp, *this->registers->rsend, *this->registers->rsbgn, count)) {
		for (size_t addr = *this->registers->rsp + 1, oaddr = count; addr <= *this->registers->rsp + count && oaddr > 0;
				addr++, oaddr--) {
			data[oaddr - 1] = this->_memory[this->rozsize + addr];
			this->_memory[this->rozsize + addr] = 0;
		}
		*this->registers->rsp = *this->registers->rsp + count;
	}
}

void memory::_MRSZ(size_t newlen) {
	if (newlen > this->_memlen) {
		unsigned char* temp = new unsigned char[newlen + this->rozsize];
		std::copy(this->_memory, this->_memory + this->rozsize + this->_memlen, temp);
		delete[] this->_memory;
		this->_memory = temp;

		memset(this->_memory + this->rozsize + this->_memlen, 0, newlen - this->_memlen);

		this->_memlen = newlen;
	}
}
void memory::_MS(unsigned char* data, size_t count, size_t addr) {
	if (addr + count - 1 > this->_memlen)
		return;
	for (size_t i = addr, j = 0; i < addr + count && j < count; i++, j++) {
		this->_memory[i + this->rozsize] = data[j];
	}
}
void memory::_MG(unsigned char* data, size_t count, size_t addr) {
	if (addr + count - 1 > this->_memlen)
		return;
	for (size_t i = addr, j = 0; i < addr + count && j < count; i++, j++) {
		data[j] = this->_memory[i + this->rozsize];
	}
}

void memory::_RSROZ(size_t newlen) {
	if (newlen > this->rozsize) {
		unsigned char* temp = new unsigned char[newlen + this->_memlen];
		std::copy(this->_memory, this->_memory + this->rozsize + this->_memlen, temp);
		delete[] this->_memory;

		memset(this->_memory + this->rozsize, 0, newlen - this->rozsize);

		this->_memory = temp;
		this->rozsize = newlen;
	}
}
void memory::_ROZVS(unsigned char* data, size_t count) {
	if (this->rozstacktop + count - 1 < this->rozsize) {
		for (size_t addr = this->rozstacktop; addr < this->rozstacktop + count; addr++) {
			this->_memory[addr] = data[addr - this->rozstacktop];
		}
		this->rozstacktop += count;
	}
	else {
		this->_RSROZ(this->rozsize + count);
		for (size_t addr = this->rozstacktop; addr < this->rozstacktop + count; addr++) {
			this->_memory[addr] = data[addr - this->rozstacktop];
		}
		this->rozstacktop += count;
	}
}
void memory::_ROZVG(unsigned char* data, size_t count, size_t addr) {
	if (addr + count - 1 > this->rozsize)
		return;
	for (size_t i = addr, j = 0; i < addr + count && j < count; i++, j++) {
		data[j] = this->_memory[i];
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
	this->_memory = new unsigned char[this->_memlen + this->rozsize];
	memset(this->_memory, 0, this->_memlen + this->rozsize);
}