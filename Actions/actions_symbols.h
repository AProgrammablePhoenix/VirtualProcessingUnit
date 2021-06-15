#pragma once

#include <iostream>
#include <memory>
#include <string>

#include "../Memory/memory_symbols.h"
#include "../Registers/registers_symbols.h"
#include "interrupts.h"
#include "process_symbols.h"

enum class virtual_actions {
	// Interrupts
	_int  = 0x000001,

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

	setCR = 0x001803,
	getCR = 0x001804,

	setDR = 0x001805,
	getDR = 0x001806,

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

	incDR = 0x002501,
	decDR = 0x002502,

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
	castreg = 0x003A16,

	// Memory
	push = 0x003F00,
	pop = 0x004000,

	pushSR = 0x003F01,
	popSR = 0x004001,

	pushCR = 0x003F02,
	popCR = 0x004002,

	pushDR = 0x003F03,
	popDR = 0x004003,

	nsms = 0x003F10,

	movsm = 0x003F04,
	movgm = 0x004004,

	movsmSR = 0x003F05,
	movgmSR = 0x004005,

	movsmCR = 0x003F06,
	movgmCR = 0x004006,

	movsmDR = 0x003F07,
	movgmDR = 0x004007,

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

	_dlog = 0x004261,
	_dlog2 = 0x004271,
	_dlog10 = 0x004281,
	_dpow = 0x004291,

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

extern void (*a_db[0x004291 + 1])(std::shared_ptr<void>, regs*, memory*);

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

		this->self_ints = interrupts(this->self_mem, this->self_regs);
		this->init();
	}

	actions_engine(memory*& memory, regs*& registers) {
		this->self_mem = memory;
		this->self_regs = registers;
		this->self_ints = interrupts(memory, registers);
		this->init();
	}

	void execute(virtual_actions action, std::shared_ptr<void> value_ptr) {
		if (action == virtual_actions::_int)
			this->_intcall(value_ptr, nullptr, nullptr);
		else
			(a_db[(size_t)action])(value_ptr, this->self_regs, this->self_mem);
	}

	size_t* getStepCounterPtr() {
		return this->self_regs->process_step;
	}
	memory* getMemoryPtr() {
		return this->self_mem;
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
	interrupts self_ints;

	void _intcall(std::shared_ptr<void> value_ptr, regs* unused_regs, memory* unused_m);

	void init() {
#pragma region b_set
		a_db[(size_t)virtual_actions::setAX] = b_set16AX;
		a_db[(size_t)virtual_actions::setBX] = b_set16BX;
		a_db[(size_t)virtual_actions::setCX] = b_set16CX;
		a_db[(size_t)virtual_actions::setDX] = b_set16DX;

		a_db[(size_t)virtual_actions::setEAX] = b_set32EAX;
		a_db[(size_t)virtual_actions::setEBX] = b_set32EBX;
		a_db[(size_t)virtual_actions::setECX] = b_set32ECX;
		a_db[(size_t)virtual_actions::setEDX] = b_set32EDX;

		a_db[(size_t)virtual_actions::setRAX] = b_set64RAX;
		a_db[(size_t)virtual_actions::setRBX] = b_set64RBX;
		a_db[(size_t)virtual_actions::setRCX] = b_set64RCX;
		a_db[(size_t)virtual_actions::setRDX] = b_set64RDX;

		a_db[(size_t)virtual_actions::setSR] = b_setSR;
		a_db[(size_t)virtual_actions::setCR] = b_setCR;
		a_db[(size_t)virtual_actions::setDR] = b_setDR;
#pragma endregion
#pragma region b_get
		a_db[(size_t)virtual_actions::getAX] = b_get16AX;
		a_db[(size_t)virtual_actions::getBX] = b_get16BX;
		a_db[(size_t)virtual_actions::getCX] = b_get16CX;
		a_db[(size_t)virtual_actions::getDX] = b_get16DX;

		a_db[(size_t)virtual_actions::getEAX] = b_get32EAX;
		a_db[(size_t)virtual_actions::getEBX] = b_get32EBX;
		a_db[(size_t)virtual_actions::getECX] = b_get32ECX;
		a_db[(size_t)virtual_actions::getEDX] = b_get32EDX;

		a_db[(size_t)virtual_actions::getRAX] = b_get64RAX;
		a_db[(size_t)virtual_actions::getRBX] = b_get64RBX;
		a_db[(size_t)virtual_actions::getRCX] = b_get64RCX;
		a_db[(size_t)virtual_actions::getRDX] = b_get64RDX;

		a_db[(size_t)virtual_actions::getSR] = b_getSR;
		a_db[(size_t)virtual_actions::getCR] = b_getCR;
		a_db[(size_t)virtual_actions::getDR] = b_getDR;
#pragma endregion
#pragma region b_mov
		a_db[(size_t)virtual_actions::movAX] = b_mov16AX;
		a_db[(size_t)virtual_actions::movBX] = b_mov16BX;
		a_db[(size_t)virtual_actions::movCX] = b_mov16CX;
		a_db[(size_t)virtual_actions::movDX] = b_mov16DX;

		a_db[(size_t)virtual_actions::movEAX] = b_mov32EAX;
		a_db[(size_t)virtual_actions::movEBX] = b_mov32EBX;
		a_db[(size_t)virtual_actions::movECX] = b_mov32ECX;
		a_db[(size_t)virtual_actions::movEDX] = b_mov32EDX;

		a_db[(size_t)virtual_actions::movRAX] = b_mov64RAX;
		a_db[(size_t)virtual_actions::movRBX] = b_mov64RBX;
		a_db[(size_t)virtual_actions::movRCX] = b_mov64RCX;
		a_db[(size_t)virtual_actions::movRDX] = b_mov64RDX;
#pragma endregion
		a_db[(size_t)virtual_actions::inc] = b_inc;
		a_db[(size_t)virtual_actions::dec] = b_dec;
		a_db[(size_t)virtual_actions::incDR] = b_incDR;
		a_db[(size_t)virtual_actions::decDR] = b_decDR;
#pragma region b_mul
		a_db[(size_t)virtual_actions::mulAX] = b_mul16AX;
		a_db[(size_t)virtual_actions::mulBX] = b_mul16BX;
		a_db[(size_t)virtual_actions::mulCX] = b_mul16CX;
		a_db[(size_t)virtual_actions::mulDX] = b_mul16DX;

		a_db[(size_t)virtual_actions::mulEAX] = b_mul32EAX;
		a_db[(size_t)virtual_actions::mulEBX] = b_mul32EBX;
		a_db[(size_t)virtual_actions::mulECX] = b_mul32ECX;
		a_db[(size_t)virtual_actions::mulEDX] = b_mul32EDX;

		a_db[(size_t)virtual_actions::mulRAX] = b_mul64RAX;
		a_db[(size_t)virtual_actions::mulRBX] = b_mul64RBX;
		a_db[(size_t)virtual_actions::mulRCX] = b_mul64RCX;
		a_db[(size_t)virtual_actions::mulRDX] = b_mul64RDX;
#pragma endregion
#pragma region b_div
		a_db[(size_t)virtual_actions::divAX] = b_div16AX;
		a_db[(size_t)virtual_actions::divBX] = b_div16BX;
		a_db[(size_t)virtual_actions::divCX] = b_div16CX;
		a_db[(size_t)virtual_actions::divDX] = b_div16DX;

		a_db[(size_t)virtual_actions::divEAX] = b_div32EAX;
		a_db[(size_t)virtual_actions::divEBX] = b_div32EBX;
		a_db[(size_t)virtual_actions::divECX] = b_div32ECX;
		a_db[(size_t)virtual_actions::divEDX] = b_div32EDX;

		a_db[(size_t)virtual_actions::divRAX] = b_div64RAX;
		a_db[(size_t)virtual_actions::divRBX] = b_div64RBX;
		a_db[(size_t)virtual_actions::divRCX] = b_div64RCX;
		a_db[(size_t)virtual_actions::divRDX] = b_div64RDX;
#pragma endregion
#pragma region b_add
		a_db[(size_t)virtual_actions::addAX] = b_add16AX;
		a_db[(size_t)virtual_actions::addBX] = b_add16BX;
		a_db[(size_t)virtual_actions::addCX] = b_add16CX;
		a_db[(size_t)virtual_actions::addDX] = b_add16DX;

		a_db[(size_t)virtual_actions::addEAX] = b_add32EAX;
		a_db[(size_t)virtual_actions::addEBX] = b_add32EBX;
		a_db[(size_t)virtual_actions::addECX] = b_add32ECX;
		a_db[(size_t)virtual_actions::addEDX] = b_add32EDX;

		a_db[(size_t)virtual_actions::addRAX] = b_add64RAX;
		a_db[(size_t)virtual_actions::addRBX] = b_add64RBX;
		a_db[(size_t)virtual_actions::addRCX] = b_add64RCX;
		a_db[(size_t)virtual_actions::addRDX] = b_add64RDX;
#pragma endregion
#pragma region b_sub
		a_db[(size_t)virtual_actions::subAX] = b_sub16AX;
		a_db[(size_t)virtual_actions::subBX] = b_sub16BX;
		a_db[(size_t)virtual_actions::subCX] = b_sub16CX;
		a_db[(size_t)virtual_actions::subDX] = b_sub16DX;

		a_db[(size_t)virtual_actions::subEAX] = b_sub32EAX;
		a_db[(size_t)virtual_actions::subEBX] = b_sub32EBX;
		a_db[(size_t)virtual_actions::subECX] = b_sub32ECX;
		a_db[(size_t)virtual_actions::subEDX] = b_sub32EDX;

		a_db[(size_t)virtual_actions::subRAX] = b_sub64RAX;
		a_db[(size_t)virtual_actions::subRBX] = b_sub64RBX;
		a_db[(size_t)virtual_actions::subRCX] = b_sub64RCX;
		a_db[(size_t)virtual_actions::subRDX] = b_sub64RDX;
#pragma endregion
#pragma region b_extended
		a_db[(size_t)virtual_actions::toString] = b_toString;
		a_db[(size_t)virtual_actions::castreg] = b_castreg;
#pragma endregion
#pragma region b_binary
		a_db[(size_t)virtual_actions::_not] = b_not;
		a_db[(size_t)virtual_actions::_and] = b_and;
		a_db[(size_t)virtual_actions::_or]  = b_or;
		a_db[(size_t)virtual_actions::_xor] = b_xor;
		a_db[(size_t)virtual_actions::_shl] = b_shl;
		a_db[(size_t)virtual_actions::_shr] = b_shr;

		a_db[(size_t)virtual_actions::_log] = b_log;
		a_db[(size_t)virtual_actions::_log2] = b_log2;
		a_db[(size_t)virtual_actions::_log10] = b_log10;
		a_db[(size_t)virtual_actions::_pow] = b_pow;
		a_db[(size_t)virtual_actions::_dlog] = b_dlog;
		a_db[(size_t)virtual_actions::_dlog2] = b_dlog2;
		a_db[(size_t)virtual_actions::_dlog10] = b_dlog10;
		a_db[(size_t)virtual_actions::_dpow] = b_dpow;
#pragma endregion

#pragma region mem
		a_db[(size_t)virtual_actions::push] = pushMem;
		a_db[(size_t)virtual_actions::pop] = popMem;

		a_db[(size_t)virtual_actions::pushSR] = pushMemSR;
		a_db[(size_t)virtual_actions::popSR] = popMemSR;

		a_db[(size_t)virtual_actions::pushCR] = pushMemCR;
		a_db[(size_t)virtual_actions::popCR] = popMemCR;

		a_db[(size_t)virtual_actions::pushDR] = pushMemDR;
		a_db[(size_t)virtual_actions::popDR] = popMemDR;

		a_db[(size_t)virtual_actions::nsms] = nsms;

		a_db[(size_t)virtual_actions::movsm] = movsm;
		a_db[(size_t)virtual_actions::movgm] = movgm;

		a_db[(size_t)virtual_actions::movsmSR] = movsmSR;
		a_db[(size_t)virtual_actions::movgmSR] = movgmSR;

		a_db[(size_t)virtual_actions::movsmCR] = movsmCR;
		a_db[(size_t)virtual_actions::movgmCR] = movgmCR;

		a_db[(size_t)virtual_actions::movsmDR] = movsmDR;
		a_db[(size_t)virtual_actions::movgmDR] = movgmDR;
#pragma endregion
#pragma region process
		a_db[(size_t)virtual_actions::ijmp] = p_inverseJmpSign;
		a_db[(size_t)virtual_actions::jmp] = p_jmp;
		a_db[(size_t)virtual_actions::cmp] = p_cmp;
		a_db[(size_t)virtual_actions::je] = p_je;
		a_db[(size_t)virtual_actions::jne] = p_jne;
		a_db[(size_t)virtual_actions::jl] = p_jl;
		a_db[(size_t)virtual_actions::jg] = p_jg;
		a_db[(size_t)virtual_actions::jle] = p_jle;
		a_db[(size_t)virtual_actions::jge] = p_jge;
		a_db[(size_t)virtual_actions::cmpstr] = p_cmpstr;

		a_db[(size_t)virtual_actions::gca] = p_gca;
		a_db[(size_t)virtual_actions::hlt] = p_hlt;

		a_db[(size_t)virtual_actions::call] = p_call;
		a_db[(size_t)virtual_actions::lcall] = p_lcall;
		a_db[(size_t)virtual_actions::ret] = p_ret;
		a_db[(size_t)virtual_actions::svcall] = p_svcall;
		a_db[(size_t)virtual_actions::rscall] = p_rscall;
#pragma endregion
	}
};