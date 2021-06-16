#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "dyn_vars.h"
#include "mem_arrays.h"
#include "mem_structs.h"

struct memory {
public:
	memory(regs* _registers);

	void push(unsigned char* data, size_t count);
	void pop(unsigned char* data, size_t count);

	// Set Memory Size
	void _SMS(size_t newlen);
	// Memory Set
	void _MS(unsigned char* data, size_t count, size_t addr);
	// Memory Get
	void _MG(unsigned char* data, size_t count, size_t addr);

	// Resize ROZ
	void _RSROZ(size_t newlen);
	// Push readonly value on ROZ (equivalent to push, but value cannot be poped, only read at its address on stack)
	void _ROZVS(unsigned char* data, size_t count);
	// Get readonly value at address in ROZ (read value of data that has been pushed on ROZ stack)
	void _ROZVG(unsigned char* data, size_t count, size_t addr);
	// Get rozstacktop (should be used before pushing new value, to get its future address in rozstack)
	size_t _ROZGST();

	void destroy();

	mem_arrays _arrays;
	mem_dyn_vars _dynvars;
	mem_structs _structs;
private:
	regs* registers;

	// At the start of program, the memory has a size of 2048 bytes, but may/should be expanded by user by calling SMS (Set Memory Size)
	// with unsigned __int64 as argument, describing new size of the memory
	unsigned char* _memory = nullptr;
	size_t _memlen = 2048;

	static constexpr size_t stacksize = 32768; // 32 Ko of stack
	size_t stacktop = 0;

	// readonly zone (roz) flags
	static constexpr size_t rozoffset = stacksize;
	size_t rozsize = 4096;					   // 4 Ko of roz (expandable)
	size_t rozstacktop = 0;

	bool resized = false;

	void init();
};