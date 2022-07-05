#pragma once

#include <iostream>
#include <memory>
#include <string>

enum class int_codes {
	rdstdin     = 0x01, // Get input from command line
	_strlen     = 0x04,
	display		= 0x05,
	recast		= 0x06,
	fromString  = 0x07,
	conversion	= 0x08
};

extern void (*ints_db[0xFF + 1])(std::shared_ptr<void>, regs*, memory*);

struct interrupts {
public:
	interrupts();
	interrupts(memory*& mem, regs*& registers);
	void pass(unsigned char intcode);
private:
	regs* self_regs;
	memory* self_mem;

	void init();
};