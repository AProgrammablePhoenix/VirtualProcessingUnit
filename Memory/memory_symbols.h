#pragma once

#include <iostream>
#include <string>

#include "memory_decl.h"
#include "../Registers/regs_decl.h"
#include "../Registers/registers_symbols.h"

#pragma warning (push)
#pragma warning (disable : 26812)

/*
enum memory_actions {
	push = 0x003F00,
	pop  = 0x004000,

	pushSR = 0x003F01,
	popSR  = 0x004001
};
*/

#pragma warning (pop)

// Memory stack symbols
void pushMem(registries_def reg, regs* registers, memory* mem);
void popMem(registries_def reg, regs* registers, memory* mem);

void pushMemSR(void* unused_p, regs* registers, memory* mem);
void popMemSR(void* unused_p, regs* registers, memory* mem);

// Memory arrays symbols
void m_declArray(void* unused_p, regs* registers, memory* mem);
void m_setAt(void* unused_p, regs* registers, memory* mem);
void m_getAt(void* unused_p, regs* registers, memory* mem);

/*
struct memory_symbols {
public:
	memory_symbols(regs* _registers, memory* _mem) {
		this->registers = _registers;
		this->mem = _mem;
		this->init();
	}

#pragma warning (push)
#pragma warning (disable : 26812)
	void execute(memory_actions action, void* value_ptr) {
		((void*(*)(void*, regs*, memory*))this->m_db[action])(value_ptr, this->registers, this->mem);
	}
#pragma warning (pop)

private:
	regs* registers;
	memory* mem;
	void* m_db[0x004001 + 1];

	void init() {
		m_db[memory_actions::push] = pushMem;
		m_db[memory_actions::pop] = popMem;
		
		m_db[memory_actions::pushSR] = pushMemSR;
		m_db[memory_actions::popSR] = popMemSR;
	}
};
*/