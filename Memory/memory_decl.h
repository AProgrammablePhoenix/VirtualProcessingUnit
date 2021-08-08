#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "dyn_vars.h"
#include "mem_arrays.h"
#include "mem_structs.h"
#include "../Networking/netman.h"

struct memory {
public:
	memory(regs* _registers);

	void update(regs* _registers);
	size_t getMemLen()const ;

	void push(unsigned char* data, size_t count);
	void pop(unsigned char* data, size_t count);

	// Resize memory
	void _MRSZ(size_t newlen);
	// Memory Set
	void _MS(unsigned char* data, size_t count, size_t addr);
	// Memory Get
	void _MG(unsigned char* data, size_t count, size_t addr);

	// Expand SDZ size by 4 KB
	void _SDZRSZ();
	// Push readonly value on SDZ (equivalent to push, but value cannot be poped, only read at its address on stack)
	void _SDZS(unsigned char* data, size_t count);
	// Get SDZ stack top (should be used before pushing new value, to get its future address in SDZ stack)
	size_t _SDZTOP() const;
	// Get SDZ total size (used in SDZS instruction that wrapps a call to this function)
	size_t _SDZS() const;

	void destroy();

	mem_arrays _arrays = mem_arrays();
	mem_dyn_vars _dynvars = mem_dyn_vars();
	mem_structs _structs = mem_structs();
	netman _netman;

	static constexpr size_t stacksize = 0x8000; // 32 KB of stack
private:
	regs* registers;
	
	// At the start of program, the memory has a size of 1048576 bytes (1MB), but may be expanded by user (only once) 
	// or kernel (multiple times) if needed; with unsigned __int64 as argument, describing new size of the memory
	unsigned char* _memory = nullptr;
	size_t _memlen = 0x100000;

	// static data zone (sdz) flags
	size_t sdzsize = 4096;					   // Default size of SDZ: 4 KB (expandable)
	size_t sdztop = 0;

	void init();
};