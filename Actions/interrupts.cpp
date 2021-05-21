#include <iostream>
#include <memory>
#include <string>

#include "action_decl.h"
#include "actions_symbols.h"
#include "interrupts.h"

void actions_engine::_intcall(std::shared_ptr<void> value_ptr, regs* unused_regs, memory* unused_mem) {
	unsigned long long intcode = *std::static_pointer_cast<unsigned long long>(value_ptr);
	this->self_ints.pass((unsigned char)intcode);
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
		(ints_db[(unsigned long long)interrupt])(nullptr, this->self_regs, this->self_mem);
	}
}
void interrupts::init() {
#pragma region int_extended
	ints_db[(unsigned long long)int_codes::rdstdin] = b_getInput;
	ints_db[(unsigned long long)int_codes::mergeString] = b_mergeString;
	ints_db[(unsigned long long)int_codes::substring] = b_substring;
	ints_db[(unsigned long long)int_codes::_strlen] = b_strlen;
	ints_db[(unsigned long long)int_codes::print] = b_print;
	ints_db[(unsigned long long)int_codes::println] = b_println;
	ints_db[(unsigned long long)int_codes::printEOL] = b_printEOL;
	ints_db[(unsigned long long)int_codes::recast] = b_recast;
	ints_db[(unsigned long long)int_codes::fromString] = b_fromString;
	ints_db[(unsigned long long)int_codes::CRToSR] = b_CRToSR;
	ints_db[(unsigned long long)int_codes::RevSR] = b_RevSR;
	ints_db[(unsigned long long)int_codes::DRToSR] = b_DRToSR;
	ints_db[(unsigned long long)int_codes::DRToULL] = b_DRToULL;
	ints_db[(unsigned long long)int_codes::DRToLL] = b_DRToLL;
#pragma endregion
#pragma region int_memory
	ints_db[(unsigned long long)int_codes::declArray] = m_declArray;
	ints_db[(unsigned long long)int_codes::setAt] = m_setAt;
	ints_db[(unsigned long long)int_codes::getAt] = m_getAt;
	ints_db[(unsigned long long)int_codes::getDynSize] = m_getDynSize;
	ints_db[(unsigned long long)int_codes::dyndecl] = m_dyndecl;
	ints_db[(unsigned long long)int_codes::dynset] = m_dynset;
	ints_db[(unsigned long long)int_codes::dynget] = m_dynget;
	ints_db[(unsigned long long)int_codes::_struct] = m_structdecl;
	ints_db[(unsigned long long)int_codes::_struct_declprop] = m_structdeclprop;
	ints_db[(unsigned long long)int_codes::_struct_select] = m_structselect;
	ints_db[(unsigned long long)int_codes::_struct_get] = m_structget;
	ints_db[(unsigned long long)int_codes::_struct_set] = m_structset;
#pragma endregion
}