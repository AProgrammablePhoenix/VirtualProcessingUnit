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
			this->_memory[this->sdzsize + addr] = data[iaddr];
		}
		*this->registers->rsp = *this->registers->rsp - count;
	}
}
void memory::pop(unsigned char* data, size_t count = 8) {
	if (isvalidpop(*this->registers->rbp, *this->registers->rsp, *this->registers->rsend, *this->registers->rsbgn, count)) {
		for (size_t addr = *this->registers->rsp + 1, oaddr = count; addr <= *this->registers->rsp + count && oaddr > 0;
				addr++, oaddr--) {
			data[oaddr - 1] = this->_memory[this->sdzsize + addr];
			this->_memory[this->sdzsize + addr] = 0;
		}
		*this->registers->rsp = *this->registers->rsp + count;
	}
}

void memory::_MRSZ(size_t newlen) {
	if (newlen > this->_memlen) {
		unsigned char* temp = new unsigned char[newlen + this->sdzsize];
		std::copy(this->_memory, this->_memory + this->sdzsize + this->_memlen, temp);
		delete[] this->_memory;
		this->_memory = temp;

		memset(this->_memory + this->sdzsize + this->_memlen, 0, newlen - this->_memlen);

		this->_memlen = newlen;
	}
}
void memory::_MS(unsigned char* data, size_t count, size_t addr) {
	if (addr + count - 1 > this->_memlen)
		return;
	for (size_t i = addr, j = 0; i < addr + count && j < count; i++, j++) {
		this->_memory[i] = data[j];
	}
}
void memory::_MG(unsigned char* data, size_t count, size_t addr) {
	if (addr + count - 1 > this->_memlen)
		return;
	for (size_t i = addr, j = 0; i < addr + count && j < count; i++, j++) {
		data[j] = this->_memory[i];
	}
}

void memory::_SDZRSZ() {
	if (this->sdzsize + 0x1000 > this->sdzsize) { // In case there isn't enough memory
		unsigned char* temp = new unsigned char[this->sdzsize + 0x1000 + this->_memlen];
		std::copy(this->_memory, this->_memory + this->sdzsize + this->_memlen, temp);
		delete[] this->_memory;

		memset(this->_memory + this->sdzsize, 0, 0x1000);

		this->_memory = temp;
		this->sdzsize += 0x1000;
	}
}
void memory::_SDZS(unsigned char* data, size_t count) {
	if (this->sdztop + count - 1 < this->sdzsize) {
		for (size_t addr = this->sdztop; addr < this->sdztop + count; addr++) {
			this->_memory[addr] = data[addr - this->sdztop];
		}
		this->sdztop += count;
	}
	else {
		this->_SDZRSZ();
		for (size_t addr = this->sdztop; addr < this->sdztop + count; addr++) {
			this->_memory[addr] = data[addr - this->sdztop];
		}
		this->sdztop += count;
	}
}
size_t memory::_SDZTOP() {
	return this->sdztop;
}

void memory::destroy() {
	this->_arrays.destroy();
	this->_netman.destroy();
	delete[] this->_memory;
}
void memory::init() {
	this->_memory = new unsigned char[this->_memlen + this->sdzsize];
	memset(this->_memory, 0, this->_memlen + this->sdzsize);
}