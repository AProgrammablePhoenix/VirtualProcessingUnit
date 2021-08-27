#include <iostream>
#include <memory>
#include <string>

#include "../utility.h"
#include "action_decl.h"
#include "actions_symbols.h"
#include "interrupts.h"

void handle_display(std::shared_ptr<void> unused_p, regs* const registers, memory* const unused_m);
void handle_conversion(std::shared_ptr<void> unused_p, regs* const registers, memory* const mem);

void actions_engine::_intcall(std::shared_ptr<void> value_ptr, regs* registers, memory* unused_m) {
	if (registers->IF) {
		const auto [vaddr, vsize, vopt] = *std::static_pointer_cast<arg_tuple>(value_ptr);
		unsigned char* uc_n = new unsigned char[sizeof(size_t)];
		this->self_mem->_MG(uc_n, sizeof(size_t), vaddr);

		size_t intcode = ATOULL(uc_n);
		this->self_ints.pass((unsigned char)intcode);
		delete[] uc_n;
	}
}

void (*ints_db[0xFF + 1])(std::shared_ptr<void>, regs*, memory*);

interrupts::interrupts() {
	this->self_regs = NULL;
	this->self_mem = NULL;
}
interrupts::interrupts(memory*& mem, regs*& registers) {
	this->self_regs = registers;
	this->self_mem = mem;
	this->init();
}

void interrupts::pass(unsigned char interrupt) {
	if (this->self_regs->IF)
		(ints_db[(size_t)interrupt])(nullptr, this->self_regs, this->self_mem);
}
void interrupts::init() {
	ints_db[(size_t)int_codes::rdstdin] = b_getInput;
	ints_db[(size_t)int_codes::mergeString] = b_mergeString;
	ints_db[(size_t)int_codes::substring] = b_substring;
	ints_db[(size_t)int_codes::_strlen] = b_strlen;
	ints_db[(size_t)int_codes::display] = handle_display;
	ints_db[(size_t)int_codes::recast] = b_recast;
	ints_db[(size_t)int_codes::fromString] = b_fromString;
	ints_db[(size_t)int_codes::conversion] = handle_conversion;
}

void handle_display(std::shared_ptr<void> unused_p, regs* const registers, memory* const unused_m) {
	uint8_t int_call = registers->al->get();

	switch (int_call) {
		case 0:
			b_print(nullptr, registers, nullptr);
			break;
		case 1:
			b_println(nullptr, registers, nullptr);
			break;
		case 2:
			b_printEOL(nullptr, registers, nullptr);
			break;
	}
}
void handle_conversion(std::shared_ptr<void> unused_p, regs* const registers, memory* const mem) {
	uint8_t int_call = registers->al->get();

	switch (int_call) {
		case 0:
			b_CRToSR(nullptr, registers, nullptr);
			break;
		case 1:
			b_RevSR(nullptr, registers, nullptr);
			break;
		case 2:
			b_FPToSR(nullptr, registers, mem);
			break;
		case 3:
			b_FPToULL(nullptr, registers, mem);
			break;
		case 4:
			b_FPToLL(nullptr, registers, mem);
			break;
	}
}