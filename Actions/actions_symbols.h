#pragma once

#include <iostream>
#include <memory>
#include <string>

#include "../ExtendedSymbols/files.h"
#include "../Memory/memory_symbols.h"
#include "../Networking/net_symbols.h"
#include "../Registers/registers_symbols.h"
#include "interrupts.h"
#include "process_symbols.h"

enum class virtual_actions {
	// Interrupts
	_int  = 0x0001,

	// Registers
	getAX = 0x0002,
	getBX = 0x0003,
	getCX = 0x0004,
	getDX = 0x0005,

	setAX = 0x0006,
	setBX = 0x0007,
	setCX = 0x0008,
	setDX = 0x0009,

	getEAX = 0x000A,
	getEBX = 0x000B,
	getECX = 0x000C,
	getEDX = 0x000D,

	setEAX = 0x000E,
	setEBX = 0x000F,
	setECX = 0x0010,
	setEDX = 0x0011,

	getRAX = 0x0012,
	getRBX = 0x0013,
	getRCX = 0x0014,
	getRDX = 0x0015,

	setRAX = 0x0016,
	setRBX = 0x0017,
	setRCX = 0x0018,
	setRDX = 0x0019,

	setSR = 0x001A,
	getSR = 0x001B,

	setCR = 0x001C,
	getCR = 0x001D,

	setDR = 0x001E,
	getDR = 0x001F,

	movAX = 0x0020,
	movBX = 0x0021,
	movCX = 0x0022,
	movDX = 0x0023,

	movEAX = 0x0024,
	movEBX = 0x0025,
	movECX = 0x0026,
	movEDX = 0x0027,

	movRAX = 0x0028,
	movRBX = 0x0029,
	movRCX = 0x002A,
	movRDX = 0x002B,

	inc = 0x002C,
	dec = 0x002D,

	incDR = 0x002E,
	decDR = 0x002F,

	mulAX = 0x0030,
	mulBX = 0x0031,
	mulCX = 0x0032,
	mulDX = 0x0033,

	mulEAX = 0x0034,
	mulEBX = 0x0035,
	mulECX = 0x0036,
	mulEDX = 0x0037,

	mulRAX = 0x0038,
	mulRBX = 0x0039,
	mulRCX = 0x003A,
	mulRDX = 0x003B,

	divAX = 0x003C,
	divBX = 0x003D,
	divCX = 0x003E,
	divDX = 0x003F,

	divEAX = 0x0040,
	divEBX = 0x0041,
	divECX = 0x0042,
	divEDX = 0x0043,

	divRAX = 0x0044,
	divRBX = 0x0045,
	divRCX = 0x0046,
	divRDX = 0x0047,

	addAX = 0x0048,
	addBX = 0x0049,
	addCX = 0x004A,
	addDX = 0x004B,

	addEAX = 0x004C,
	addEBX = 0x004D,
	addECX = 0x004E,
	addEDX = 0x004F,

	addRAX = 0x0050,
	addRBX = 0x0051,
	addRCX = 0x0052,
	addRDX = 0x0053,

	subAX = 0x0054,
	subBX = 0x0055,
	subCX = 0x0056,
	subDX = 0x0057,

	subEAX = 0x0058,
	subEBX = 0x0059,
	subECX = 0x005A,
	subEDX = 0x005B,

	subRAX = 0x005C,
	subRBX = 0x005D,
	subRCX = 0x005E,
	subRDX = 0x005F,

	// Extended
	toString = 0x0060,
	castreg = 0x0061,

	// Memory
	push = 0x0062,
	pop = 0x0063,

	pushSR = 0x0064,
	popSR = 0x0065,

	pushCR = 0x0066,
	popCR = 0x0067,

	pushDR = 0x0068,
	popDR = 0x0069,

	nsms = 0x006A,

	movsm = 0x006B,
	movgm = 0x006C,

	movsmSR = 0x006D,
	movgmSR = 0x006E,

	movsmCR = 0x006F,
	movgmCR = 0x0070,

	movsmDR = 0x0071,
	movgmDR = 0x0072,

	// Native binary ops [belong to Registers]
	_not = 0x0073,
	_and = 0x0074,
	_or  = 0x0075,
	_xor = 0x0076,
	_shl = 0x0077,
	_shr = 0x0078,

	_log = 0x0079,
	_log2 = 0x007A,
	_log10 = 0x007B,
	_pow = 0x007C,

	_dlog = 0x007D,
	_dlog2 = 0x007E,
	_dlog10 = 0x007F,
	_dpow = 0x0080,

	// Process
	ijmp = 0x0081,
	jmp = 0x0082,
	cmp = 0x0083,
	je  = 0x0084,
	jne = 0x0085,
	jl  = 0x0086,
	jg  = 0x0087,
	jle = 0x0088,
	jge = 0x0089,
	cmpstr = 0x008A,

	gca = 0x008B,
	hlt = 0x008C,

	call = 0x008D,
	lcall = 0x008E,
	ret = 0x008F,
	svcall = 0x0090,
	rscall = 0x0091,

	nopen  = 0x0092,
	nclose = 0x0093,
	nget   = 0x0094,
	nsend  = 0x0095,
	nhrecv = 0x0096,
	ncrtep = 0x0097,
	nselep = 0x0098
};

extern void (*a_db[0x0098 + 1])(std::shared_ptr<void>, regs*, memory*);

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
#pragma region networking
		a_db[(size_t)virtual_actions::nopen] = net_open;
		a_db[(size_t)virtual_actions::nclose] = net_close;
		a_db[(size_t)virtual_actions::nget] = net_get;
		a_db[(size_t)virtual_actions::nsend] = net_send;
		a_db[(size_t)virtual_actions::nhrecv] = net_hrecv;
		a_db[(size_t)virtual_actions::ncrtep] = net_crtep;
		a_db[(size_t)virtual_actions::nselep] = net_selep;
#pragma endregion
	}
};