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
	_int = 1,

	// Registers
	getAX,
	getBX,
	getCX,
	getDX,

	setAX,
	setBX,
	setCX,
	setDX,

	getEAX,
	getEBX,
	getECX,
	getEDX,

	setEAX,
	setEBX,
	setECX,
	setEDX,

	getRAX,
	getRBX,
	getRCX,
	getRDX,

	setRAX,
	setRBX,
	setRCX,
	setRDX,

	setSR,
	getSR,

	setCR,
	getCR,

	setDR,
	getDR,

	setFPR0,
	setFPR1,
	setFPR2,
	setFPR3,

	setEFPR0,
	setEFPR1,
	setEFPR2,
	setEFPR3,

	setRFPR0,
	setRFPR1,
	setRFPR2,
	setRFPR3,

	movAX,
	movBX,
	movCX,
	movDX,

	movEAX,
	movEBX,
	movECX,
	movEDX,

	movRAX,
	movRBX,
	movRCX,
	movRDX,
	movRBP,
	movRSP,

	inc,
	dec,

	incDR,
	decDR,

	mulAX,
	mulBX,
	mulCX,
	mulDX,

	mulEAX,
	mulEBX,
	mulECX,
	mulEDX,

	mulRAX,
	mulRBX,
	mulRCX,
	mulRDX,

	divAX,
	divBX,
	divCX,
	divDX,

	divEAX,
	divEBX,
	divECX,
	divEDX,

	divRAX,
	divRBX,
	divRCX,
	divRDX,

	addAX,
	addBX,
	addCX,
	addDX,

	addEAX,
	addEBX,
	addECX,
	addEDX,

	addRAX,
	addRBX,
	addRCX,
	addRDX,
	addRBP,
	addRSP,

	subAX,
	subBX,
	subCX,
	subDX,

	subEAX,
	subEBX,
	subECX,
	subEDX,

	subRAX,
	subRBX,
	subRCX,
	subRDX,
	subRBP,
	subRSP,

	// Extended
	toString,
	castreg,

	sdzs,

	// Memory
	push,
	pop,

	pushSR,
	popSR,

	pushCR,
	popCR,

	pushDR,
	popDR,

	movsm,
	movgm,

	movsmSR,
	movgmSR,

	movsmCR,
	movgmCR,

	movsmDR,
	movgmDR,

	// Native binary ops [belong to Registers]
	_not,
	_and,
	_or,
	_xor,
	_shl,
	_shr,

	_log,
	_log2,
	_log10,
	_pow,

	_dlog,
	_dlog2,
	_dlog10,
	_dpow,

	// Process
	jmp,
	cmp,
	je,
	jne,
	jl,
	jg,
	jle,
	jge,
	cmpstr,

	gca,
	hlt,

	call,
	lcall,
	ret,
	svcall,
	rscall,

	nopen,
	nclose,
	nget,
	nsend,
	nhrecv,
	ncrtep,
	nselep,

	pcrtthread,
	prstthread,
	pendthread	
};

extern void (*a_db[(size_t)virtual_actions::pendthread + 1])(std::shared_ptr<void>, regs*, memory*);

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

	void setThreadsMap(std::map<size_t, int>*& _threadsStatuses) {
		this->self_regs->threadsStatuses = _threadsStatuses;
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
	regs* getRegsPtr() {
		return this->self_regs;
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

		a_db[(size_t)virtual_actions::setFPR0] = b_setFPR0;
		a_db[(size_t)virtual_actions::setFPR1] = b_setFPR1;
		a_db[(size_t)virtual_actions::setFPR2] = b_setFPR2;
		a_db[(size_t)virtual_actions::setFPR3] = b_setFPR3;

		a_db[(size_t)virtual_actions::setEFPR0] = b_setEFPR0;
		a_db[(size_t)virtual_actions::setEFPR1] = b_setEFPR1;
		a_db[(size_t)virtual_actions::setEFPR2] = b_setEFPR2;
		a_db[(size_t)virtual_actions::setEFPR3] = b_setEFPR3;

		a_db[(size_t)virtual_actions::setRFPR0] = b_setRFPR0;
		a_db[(size_t)virtual_actions::setRFPR1] = b_setRFPR1;
		a_db[(size_t)virtual_actions::setRFPR2] = b_setRFPR2;
		a_db[(size_t)virtual_actions::setRFPR3] = b_setRFPR3;
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

		a_db[(size_t)virtual_actions::movRBP] = b_mov64RBP;
		a_db[(size_t)virtual_actions::movRSP] = b_mov64RSP;
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

		a_db[(size_t)virtual_actions::addRBP] = b_add64RBP;
		a_db[(size_t)virtual_actions::addRSP] = b_add64RSP;
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

		a_db[(size_t)virtual_actions::subRBP] = b_sub64RBP;
		a_db[(size_t)virtual_actions::subRSP] = b_sub64RSP;
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

		a_db[(size_t)virtual_actions::movsm] = movsm;
		a_db[(size_t)virtual_actions::movgm] = movgm;

		a_db[(size_t)virtual_actions::movsmSR] = movsmSR;
		a_db[(size_t)virtual_actions::movgmSR] = movgmSR;

		a_db[(size_t)virtual_actions::movsmCR] = movsmCR;
		a_db[(size_t)virtual_actions::movgmCR] = movgmCR;

		a_db[(size_t)virtual_actions::movsmDR] = movsmDR;
		a_db[(size_t)virtual_actions::movgmDR] = movgmDR;

		a_db[(size_t)virtual_actions::sdzs] = sdzsMem;
#pragma endregion
#pragma region process
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
#pragma region threading
		a_db[(size_t)virtual_actions::pcrtthread] = p_crtthread;
		a_db[(size_t)virtual_actions::prstthread] = p_rstthread;
		a_db[(size_t)virtual_actions::pendthread] = p_endthread;
#pragma endregion
	}
};