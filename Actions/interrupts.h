#pragma once

#include <iostream>
#include <memory>
#include <string>

enum class int_codes {
	rdstdin     = 0x01, // Get input from command line
	mergeString = 0x02,
	substring   = 0x03,
	_strlen     = 0x04,
	print		= 0x05,
	println		= 0x06,
	printEOL	= 0x07,
	recast		= 0x08,
	fromString  = 0x09,
	CRToSR		= 0x0A,
	RevSR		= 0x0B,
	FPToSR		= 0x0C,
	FPToULL		= 0x0D,
	FPToLL		= 0x0E
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
	bool allowedToRun = false;

	void init();
};