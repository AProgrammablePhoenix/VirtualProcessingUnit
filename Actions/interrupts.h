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
	DRToSR		= 0x0C,
	DRToULL		= 0x0D,
	DRToLL		= 0x0E,
	declArray   = 0x0F,
	setAt       = 0x10,
	getAt       = 0x11,
	getDynSize  = 0x12,
	dyndecl     = 0x13,
	dynset      = 0x14,
	dynget		= 0x15,
	_struct		= 0x16,
	_struct_declprop = 0x17,
	_struct_select	 = 0x18,
	_struct_get		 = 0x19,
	_struct_set		 = 0x1A
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