#include <iostream>
#include <memory>
#include <string>

#include "../utility.h"
#include "action_decl.h"
#include "actions_symbols.h"
#include "interrupts.h"

void actions_engine::_intcall(std::shared_ptr<void> value_ptr, regs* unused_regs, memory* unused_m) {
	std::tuple<size_t, size_t> varinfos = *std::static_pointer_cast<std::tuple<size_t, size_t>>(value_ptr);
	unsigned char* uc_n = new unsigned char[sizeof(size_t)];
	this->self_mem->_ROZVG(uc_n, sizeof(size_t), std::get<0>(varinfos));

	size_t intcode = ATOULL(uc_n);
	this->self_ints.pass((unsigned char)intcode);
	delete[] uc_n;
}

void (*ints_db[0xFF + 1])(std::shared_ptr<void>, regs*, memory*);

interrupts::interrupts() {
	this->self_regs = NULL;
	this->self_mem = NULL;
	this->allowedToRun = false;
}
interrupts::interrupts(memory*& mem, regs*& registers) {
	this->self_regs = registers;
	this->self_mem = mem;
	this->init();
	this->allowedToRun = true;
}

void interrupts::pass(unsigned char interrupt) {
	if (this->allowedToRun) {
		(ints_db[(size_t)interrupt])(nullptr, this->self_regs, this->self_mem);
	}
}
void interrupts::init() {
#pragma region int_extended
	ints_db[(size_t)int_codes::rdstdin] = b_getInput;
	ints_db[(size_t)int_codes::mergeString] = b_mergeString;
	ints_db[(size_t)int_codes::substring] = b_substring;
	ints_db[(size_t)int_codes::_strlen] = b_strlen;
	ints_db[(size_t)int_codes::print] = b_print;
	ints_db[(size_t)int_codes::println] = b_println;
	ints_db[(size_t)int_codes::printEOL] = b_printEOL;
	ints_db[(size_t)int_codes::recast] = b_recast;
	ints_db[(size_t)int_codes::fromString] = b_fromString;
	ints_db[(size_t)int_codes::CRToSR] = b_CRToSR;
	ints_db[(size_t)int_codes::RevSR] = b_RevSR;
	ints_db[(size_t)int_codes::DRToSR] = b_DRToSR;
	ints_db[(size_t)int_codes::DRToULL] = b_DRToULL;
	ints_db[(size_t)int_codes::DRToLL] = b_DRToLL;
#pragma endregion
#pragma region int_memory
	ints_db[(size_t)int_codes::declArray] = m_declArray;
	ints_db[(size_t)int_codes::setAt] = m_setAt;
	ints_db[(size_t)int_codes::getAt] = m_getAt;
	ints_db[(size_t)int_codes::getDynSize] = m_getDynSize;
	ints_db[(size_t)int_codes::dyndecl] = m_dyndecl;
	ints_db[(size_t)int_codes::dynset] = m_dynset;
	ints_db[(size_t)int_codes::dynget] = m_dynget;
	ints_db[(size_t)int_codes::_struct] = m_structdecl;
	ints_db[(size_t)int_codes::_struct_declprop] = m_structdeclprop;
	ints_db[(size_t)int_codes::_struct_select] = m_structselect;
	ints_db[(size_t)int_codes::_struct_get] = m_structget;
	ints_db[(size_t)int_codes::_struct_set] = m_structset;
#pragma endregion
}