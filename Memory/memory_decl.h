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
	bool resized = false;

	void init();
};