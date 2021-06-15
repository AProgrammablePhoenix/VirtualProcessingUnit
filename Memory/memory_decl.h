#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "dyn_vars.h"
#include "mem_arrays.h"
#include "mem_structs.h"

struct m_container {
public:
	void set(unsigned char data[]);
	void set(unsigned char data[], size_t length);

	unsigned char* get();
private:
	unsigned char *obj = new unsigned char[sizeof(size_t)];
};

struct memory {
public:
	memory(regs* _registers);

	void push(unsigned char* data, size_t count);
	void pop(unsigned char* data, size_t count);

	// Set Memory Size
	void _SMS(size_t newlen);
	// New Memory Set
	void _NMS(unsigned char* data, size_t count, size_t addr);
	// New Memory Get
	void _NMG(unsigned char* data, size_t count, size_t addr);

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
	std::vector<m_container> _memory;

	// At the start of program, the memory has a size of 2048 bytes, but may/should be expanded by user by calling SMS (Set Memory Size)
	// with unsigned __int64 as argument, describing new size of the memory
	unsigned char* _newmem = nullptr;

	size_t _newmemlen = 2048;

	static constexpr size_t stacksize = 32768; // 32 Ko of stack
	size_t stacktop = 0;

	// readonly zone (roz) flags
	static constexpr size_t rozoffset = stacksize;
	size_t rozsize = 4096;					   // 4 Ko of roz (expandable)
	size_t rozstacktop = 0;

	bool resized = false;

	void init();
};