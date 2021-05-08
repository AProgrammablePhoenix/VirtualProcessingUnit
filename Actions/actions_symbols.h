#pragma once

#include <iostream>
#include <string>
#include <memory>

#include "../Registers/registers_symbols.h"
#include "../Memory/memory_symbols.h"
#include "process_symbols.h"

enum class virtual_actions {
	// Registers
	getAX = 0x000100,
	getBX = 0x000200,
	getCX = 0x000300,
	getDX = 0x000400,

	setAX = 0x000500,
	setBX = 0x000600,
	setCX = 0x000700,
	setDX = 0x000800,

	getEAX = 0x000900,
	getEBX = 0x000A00,
	getECX = 0x000B00,
	getEDX = 0x000C00,

	setEAX = 0x000D00,
	setEBX = 0x000E00,
	setECX = 0x000F00,
	setEDX = 0x001000,

	getRAX = 0x001100,
	getRBX = 0x001200,
	getRCX = 0x001300,
	getRDX = 0x001400,

	setRAX = 0x001500,
	setRBX = 0x001600,
	setRCX = 0x001700,
	setRDX = 0x001800,

	setSR = 0x001801,
	getSR = 0x001802,

	movAX = 0x001900,
	movBX = 0x001A00,
	movCX = 0x001B00,
	movDX = 0x001C00,

	movEAX = 0x001D00,
	movEBX = 0x001E00,
	movECX = 0x001F00,
	movEDX = 0x002000,

	movRAX = 0x002100,
	movRBX = 0x002200,
	movRCX = 0x002300,
	movRDX = 0x002400,

	inc = 0x002500,
	dec = 0x002600,

	mulAX = 0x002700,
	mulBX = 0x002800,
	mulCX = 0x002900,
	mulDX = 0x002A00,

	mulEAX = 0x002B00,
	mulEBX = 0x002C00,
	mulECX = 0x002D00,
	mulEDX = 0x002E00,

	mulRAX = 0x002F00,
	mulRBX = 0x003000,
	mulRCX = 0x003100,
	mulRDX = 0x003200,

	divAX = 0x003300,
	divBX = 0x003400,
	divCX = 0x003500,
	divDX = 0x003600,

	divEAX = 0x003700,
	divEBX = 0x003800,
	divECX = 0x003900,
	divEDX = 0x003A00,

	divRAX = 0x003B00,
	divRBX = 0x003C00,
	divRCX = 0x003D00,
	divRDX = 0x003E00,

	addAX = 0x002701,
	addBX = 0x002702,
	addCX = 0x002703,
	addDX = 0x002704,

	addEAX = 0x002705,
	addEBX = 0x002706,
	addECX = 0x002707,
	addEDX = 0x002708,

	addRAX = 0x002709,
	addRBX = 0x00270A,
	addRCX = 0x00270B,
	addRDX = 0x00270C,

	subAX = 0x00270D,
	subBX = 0x00270E,
	subCX = 0x00270F,
	subDX = 0x0027A0,

	subEAX = 0x0027A1,
	subEBX = 0x0027A2,
	subECX = 0x0027A3,
	subEDX = 0x0027A4,

	subRAX = 0x0027A5,
	subRBX = 0x0027A6,
	subRCX = 0x0027A7,
	subRDX = 0x0027A8,

	// Extended
	toString = 0x003011,
	mergeString = 0x003012,
	substring = 0x003013,
	print = 0x003014,
	println = 0x003015,
	printEOL = 0x003016,
	_strlen = 0x003170,
	rdstdin = 0x003171, // Get input from command line

	castreg = 0x003A16,
	recast = 0x003A17,
	fromString = 0x003A18,

	// Memory
	push = 0x003F00,
	pop = 0x004000,

	pushSR = 0x003F01,
	popSR = 0x004001,

	declArray = 0x004100,
	setAt = 0x004110,
	getAt = 0x004120,
	getDynSize = 0x004130,
	dyndecl = 0x004140,
	dynset = 0x004150,
	dynget = 0x004160,
	_struct = 0x004170,
	_struct_declprop = 0x004171,
	_struct_select = 0x004172,
	_struct_get = 0x004173,
	_struct_set = 0x004174,

	// Native binary ops [belong to Registers]
	_not = 0x004200,
	_and = 0x004210,
	_or  = 0x004220,
	_xor = 0x004230,
	_shl = 0x004240,
	_shr = 0x004250,

	_log = 0x004260,
	_log2 = 0x004270,
	_log10 = 0x004280,
	_pow = 0x004290,

	// Process
	ijmp = 0x003017,
	jmp = 0x003018,
	cmp = 0x003019,
	je  = 0x0030A1,
	jne = 0x0030A2,
	jl  = 0x0030A3,
	jg  = 0x0030A4,
	jle = 0x0030A5,
	jge = 0x0030A6,
	cmpstr = 0x0030A7,

	gca = 0x0030A8,
	hlt = 0x0030A9,

	call = 0x0030AA,
	lcall = 0x0030AB,
	ret = 0x0030AC,
	svcall = 0x0030AD,
	rscall = 0x0030AE
};

typedef void* (*core_function)(std::shared_ptr<void>, regs*, memory*);

extern void (*a_db[0x004290 + 1])(std::shared_ptr<void>, regs*, memory*);

struct actions_engine {
public:
	actions_engine() {
		this->self_regs = new regs;
		this->self_mem = new memory(this->self_regs);

		if (!this->self_regs || !this->self_mem) {
			std::cout << "Error while allocating new process memory" << std::endl;
			delete this->self_regs;
			delete this->self_mem;
		}

		this->init();
	}

	actions_engine(memory*& memory, regs*& registers) {
		this->self_mem = memory;
		this->self_regs = registers;
		this->init();
	}

	void execute(virtual_actions action, std::shared_ptr<void> value_ptr) {
		((void* (*)(std::shared_ptr<void>, regs*, memory*))a_db[(unsigned long long)action])(value_ptr, this->self_regs, this->self_mem);
	}

	unsigned long long* getStepCounterPtr() {
		return this->self_regs->process_step;
	}
	bool toStop() {
		return *this->self_regs->stopRequested;
	}

	void destroy() {
		if (this->self_mem) {
			delete this->self_mem;
		}
		if (this->self_regs) {
			delete this->self_regs;
		}
	}
private:
	memory* self_mem;
	regs* self_regs;

	void init() {
#pragma region b_set
		a_db[(unsigned long long)virtual_actions::setAX] = b_set16AX;
		a_db[(unsigned long long)virtual_actions::setBX] = b_set16BX;
		a_db[(unsigned long long)virtual_actions::setCX] = b_set16CX;
		a_db[(unsigned long long)virtual_actions::setDX] = b_set16DX;

		a_db[(unsigned long long)virtual_actions::setEAX] = b_set32EAX;
		a_db[(unsigned long long)virtual_actions::setEBX] = b_set32EBX;
		a_db[(unsigned long long)virtual_actions::setECX] = b_set32ECX;
		a_db[(unsigned long long)virtual_actions::setEDX] = b_set32EDX;

		a_db[(unsigned long long)virtual_actions::setRAX] = b_set64RAX;
		a_db[(unsigned long long)virtual_actions::setRBX] = b_set64RBX;
		a_db[(unsigned long long)virtual_actions::setRCX] = b_set64RCX;
		a_db[(unsigned long long)virtual_actions::setRDX] = b_set64RDX;

		a_db[(unsigned long long)virtual_actions::setSR] = b_setSR;
#pragma endregion
#pragma region b_get
		a_db[(unsigned long long)virtual_actions::getAX] = b_get16AX;
		a_db[(unsigned long long)virtual_actions::getBX] = b_get16BX;
		a_db[(unsigned long long)virtual_actions::getCX] = b_get16CX;
		a_db[(unsigned long long)virtual_actions::getDX] = b_get16DX;

		a_db[(unsigned long long)virtual_actions::getEAX] = b_get32EAX;
		a_db[(unsigned long long)virtual_actions::getEBX] = b_get32EBX;
		a_db[(unsigned long long)virtual_actions::getECX] = b_get32ECX;
		a_db[(unsigned long long)virtual_actions::getEDX] = b_get32EDX;

		a_db[(unsigned long long)virtual_actions::getRAX] = b_get64RAX;
		a_db[(unsigned long long)virtual_actions::getRBX] = b_get64RBX;
		a_db[(unsigned long long)virtual_actions::getRCX] = b_get64RCX;
		a_db[(unsigned long long)virtual_actions::getRDX] = b_get64RDX;

		a_db[(unsigned long long)virtual_actions::getSR] = b_getSR;
#pragma endregion
#pragma region b_mov
		a_db[(unsigned long long)virtual_actions::movAX] = b_mov16AX;
		a_db[(unsigned long long)virtual_actions::movBX] = b_mov16BX;
		a_db[(unsigned long long)virtual_actions::movCX] = b_mov16CX;
		a_db[(unsigned long long)virtual_actions::movDX] = b_mov16DX;

		a_db[(unsigned long long)virtual_actions::movEAX] = b_mov32EAX;
		a_db[(unsigned long long)virtual_actions::movEBX] = b_mov32EBX;
		a_db[(unsigned long long)virtual_actions::movECX] = b_mov32ECX;
		a_db[(unsigned long long)virtual_actions::movEDX] = b_mov32EDX;

		a_db[(unsigned long long)virtual_actions::movRAX] = b_mov64RAX;
		a_db[(unsigned long long)virtual_actions::movRBX] = b_mov64RBX;
		a_db[(unsigned long long)virtual_actions::movRCX] = b_mov64RCX;
		a_db[(unsigned long long)virtual_actions::movRDX] = b_mov64RDX;
#pragma endregion
		a_db[(unsigned long long)virtual_actions::inc] = b_inc;
		a_db[(unsigned long long)virtual_actions::dec] = b_dec;
#pragma region b_mul
		a_db[(unsigned long long)virtual_actions::mulAX] = b_mul16AX;
		a_db[(unsigned long long)virtual_actions::mulBX] = b_mul16BX;
		a_db[(unsigned long long)virtual_actions::mulCX] = b_mul16CX;
		a_db[(unsigned long long)virtual_actions::mulDX] = b_mul16DX;

		a_db[(unsigned long long)virtual_actions::mulEAX] = b_mul32EAX;
		a_db[(unsigned long long)virtual_actions::mulEBX] = b_mul32EBX;
		a_db[(unsigned long long)virtual_actions::mulECX] = b_mul32ECX;
		a_db[(unsigned long long)virtual_actions::mulEDX] = b_mul32EDX;

		a_db[(unsigned long long)virtual_actions::mulRAX] = b_mul64RAX;
		a_db[(unsigned long long)virtual_actions::mulRBX] = b_mul64RBX;
		a_db[(unsigned long long)virtual_actions::mulRCX] = b_mul64RCX;
		a_db[(unsigned long long)virtual_actions::mulRDX] = b_mul64RDX;
#pragma endregion
#pragma region b_div
		a_db[(unsigned long long)virtual_actions::divAX] = b_div16AX;
		a_db[(unsigned long long)virtual_actions::divBX] = b_div16BX;
		a_db[(unsigned long long)virtual_actions::divCX] = b_div16CX;
		a_db[(unsigned long long)virtual_actions::divDX] = b_div16DX;

		a_db[(unsigned long long)virtual_actions::divEAX] = b_div32EAX;
		a_db[(unsigned long long)virtual_actions::divEBX] = b_div32EBX;
		a_db[(unsigned long long)virtual_actions::divECX] = b_div32ECX;
		a_db[(unsigned long long)virtual_actions::divEDX] = b_div32EDX;

		a_db[(unsigned long long)virtual_actions::divRAX] = b_div64RAX;
		a_db[(unsigned long long)virtual_actions::divRBX] = b_div64RBX;
		a_db[(unsigned long long)virtual_actions::divRCX] = b_div64RCX;
		a_db[(unsigned long long)virtual_actions::divRDX] = b_div64RDX;
#pragma endregion
#pragma region b_add
		a_db[(unsigned long long)virtual_actions::addAX] = b_add16AX;
		a_db[(unsigned long long)virtual_actions::addBX] = b_add16BX;
		a_db[(unsigned long long)virtual_actions::addCX] = b_add16CX;
		a_db[(unsigned long long)virtual_actions::addDX] = b_add16DX;

		a_db[(unsigned long long)virtual_actions::addEAX] = b_add32EAX;
		a_db[(unsigned long long)virtual_actions::addEBX] = b_add32EBX;
		a_db[(unsigned long long)virtual_actions::addECX] = b_add32ECX;
		a_db[(unsigned long long)virtual_actions::addEDX] = b_add32EDX;

		a_db[(unsigned long long)virtual_actions::addRAX] = b_add64RAX;
		a_db[(unsigned long long)virtual_actions::addRBX] = b_add64RBX;
		a_db[(unsigned long long)virtual_actions::addRCX] = b_add64RCX;
		a_db[(unsigned long long)virtual_actions::addRDX] = b_add64RDX;
#pragma endregion
#pragma region b_sub
		a_db[(unsigned long long)virtual_actions::subAX] = b_sub16AX;
		a_db[(unsigned long long)virtual_actions::subBX] = b_sub16BX;
		a_db[(unsigned long long)virtual_actions::subCX] = b_sub16CX;
		a_db[(unsigned long long)virtual_actions::subDX] = b_sub16DX;

		a_db[(unsigned long long)virtual_actions::subEAX] = b_sub32EAX;
		a_db[(unsigned long long)virtual_actions::subEBX] = b_sub32EBX;
		a_db[(unsigned long long)virtual_actions::subECX] = b_sub32ECX;
		a_db[(unsigned long long)virtual_actions::subEDX] = b_sub32EDX;

		a_db[(unsigned long long)virtual_actions::subRAX] = b_sub64RAX;
		a_db[(unsigned long long)virtual_actions::subRBX] = b_sub64RBX;
		a_db[(unsigned long long)virtual_actions::subRCX] = b_sub64RCX;
		a_db[(unsigned long long)virtual_actions::subRDX] = b_sub64RDX;
#pragma endregion
#pragma region b_extended
		a_db[(unsigned long long)virtual_actions::rdstdin] = b_getInput;
		a_db[(unsigned long long)virtual_actions::toString] = b_toString;
		a_db[(unsigned long long)virtual_actions::mergeString] = b_mergeString;
		a_db[(unsigned long long)virtual_actions::substring] = b_substring;
		a_db[(unsigned long long)virtual_actions::_strlen] = b_strlen;

		a_db[(unsigned long long)virtual_actions::print] = b_print;
		a_db[(unsigned long long)virtual_actions::println] = b_println;
		a_db[(unsigned long long)virtual_actions::printEOL] = b_printEOL;

		a_db[(unsigned long long)virtual_actions::castreg] = b_castreg;
		a_db[(unsigned long long)virtual_actions::recast] = b_recast;
		a_db[(unsigned long long)virtual_actions::fromString] = b_fromString;
#pragma endregion
#pragma region b_binary
		a_db[(unsigned long long)virtual_actions::_not] = b_not;
		a_db[(unsigned long long)virtual_actions::_and] = b_and;
		a_db[(unsigned long long)virtual_actions::_or]  = b_or;
		a_db[(unsigned long long)virtual_actions::_xor] = b_xor;
		a_db[(unsigned long long)virtual_actions::_shl] = b_shl;
		a_db[(unsigned long long)virtual_actions::_shr] = b_shr;

		a_db[(unsigned long long)virtual_actions::_log] = b_log;
		a_db[(unsigned long long)virtual_actions::_log2] = b_log2;
		a_db[(unsigned long long)virtual_actions::_log10] = b_log10;
		a_db[(unsigned long long)virtual_actions::_pow] = b_pow;
#pragma endregion

#pragma region mem
		a_db[(unsigned long long)virtual_actions::push] = pushMem;
		a_db[(unsigned long long)virtual_actions::pop] = popMem;

		a_db[(unsigned long long)virtual_actions::pushSR] = pushMemSR;
		a_db[(unsigned long long)virtual_actions::popSR] = popMemSR;

		a_db[(unsigned long long)virtual_actions::declArray] = m_declArray;
		a_db[(unsigned long long)virtual_actions::setAt] = m_setAt;
		a_db[(unsigned long long)virtual_actions::getAt] = m_getAt;
		a_db[(unsigned long long)virtual_actions::getDynSize] = m_getDynSize;
		
		a_db[(unsigned long long)virtual_actions::dyndecl] = m_dyndecl;
		a_db[(unsigned long long)virtual_actions::dynset] = m_dynset;
		a_db[(unsigned long long)virtual_actions::dynget] = m_dynget;

		a_db[(unsigned long long)virtual_actions::_struct] = m_structdecl;
		a_db[(unsigned long long)virtual_actions::_struct_declprop] = m_structdeclprop;
		a_db[(unsigned long long)virtual_actions::_struct_select] = m_structselect;
		a_db[(unsigned long long)virtual_actions::_struct_get] = m_structget;
		a_db[(unsigned long long)virtual_actions::_struct_set] = m_structset;
#pragma endregion
#pragma region process
		a_db[(unsigned long long)virtual_actions::ijmp] = p_inverseJmpSign;
		a_db[(unsigned long long)virtual_actions::jmp] = p_jmp;
		a_db[(unsigned long long)virtual_actions::cmp] = p_cmp;
		a_db[(unsigned long long)virtual_actions::je] = p_je;
		a_db[(unsigned long long)virtual_actions::jne] = p_jne;
		a_db[(unsigned long long)virtual_actions::jl] = p_jl;
		a_db[(unsigned long long)virtual_actions::jg] = p_jg;
		a_db[(unsigned long long)virtual_actions::jle] = p_jle;
		a_db[(unsigned long long)virtual_actions::jge] = p_jge;
		a_db[(unsigned long long)virtual_actions::cmpstr] = p_cmpstr;

		a_db[(unsigned long long)virtual_actions::gca] = p_gca;
		a_db[(unsigned long long)virtual_actions::hlt] = p_hlt;

		a_db[(unsigned long long)virtual_actions::call] = p_call;
		a_db[(unsigned long long)virtual_actions::lcall] = p_lcall;
		a_db[(unsigned long long)virtual_actions::ret] = p_ret;
		a_db[(unsigned long long)virtual_actions::svcall] = p_svcall;
		a_db[(unsigned long long)virtual_actions::rscall] = p_rscall;
#pragma endregion
	}
};