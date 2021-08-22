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
	setAX,
	setBX,
	setCX,
	setDX,

	setEAX,
	setEBX,
	setECX,
	setEDX,

	setRAX,
	setRBX,
	setRCX,
	setRDX,

	gset,

	setSR,

	setCR,

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

	gmov,

	movFPR0,
	movFPR1,
	movFPR2,
	movFPR3,

	movEFPR0,
	movEFPR1,
	movEFPR2,
	movEFPR3,

	movRFPR0,
	movRFPR1,
	movRFPR2,
	movRFPR3,

	inc,
	dec,

	incFP,
	decFP,

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

	mulFPR0,
	mulFPR1,
	mulFPR2,
	mulFPR3,

	mulEFPR0,
	mulEFPR1,
	mulEFPR2,
	mulEFPR3,

	mulRFPR0,
	mulRFPR1,
	mulRFPR2,
	mulRFPR3,

	divFPR0,
	divFPR1,
	divFPR2,
	divFPR3,

	divEFPR0,
	divEFPR1,
	divEFPR2,
	divEFPR3,

	divRFPR0,
	divRFPR1,
	divRFPR2,
	divRFPR3,

	addFPR0,
	addFPR1,
	addFPR2,
	addFPR3,

	addEFPR0,
	addEFPR1,
	addEFPR2,
	addEFPR3,

	addRFPR0,
	addRFPR1,
	addRFPR2,
	addRFPR3,

	subFPR0,
	subFPR1,
	subFPR2,
	subFPR3,

	subEFPR0,
	subEFPR1,
	subEFPR2,
	subEFPR3,

	subRFPR0,
	subRFPR1,
	subRFPR2,
	subRFPR3,

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

	pushFP,
	popFP,

	movsm,
	movgm,

	movsmSR,
	movgmSR,

	movsmCR,
	movgmCR,

	movsmFP,
	movgmFP,

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
	cmpdbl,

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

	rfread,
	rfwrite,
	rflen,

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

	inline static void has_been_destoryed() {
		actions_engine::pre_destroyed = true;
	}
	void destroy() {
		if (!actions_engine::pre_destroyed) {
			if (this->self_mem)
				delete this->self_mem;
			if (this->self_regs)
				delete this->self_regs;
		}
	}
private:
	memory* self_mem;
	regs* self_regs;
	interrupts self_ints;
	inline static bool pre_destroyed = false;

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

		a_db[(size_t)virtual_actions::gset] = b_setGP;

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

		a_db[(size_t)virtual_actions::gmov] = b_movGP;

		a_db[(size_t)virtual_actions::movFPR0] = b_movFPR0;
		a_db[(size_t)virtual_actions::movFPR1] = b_movFPR1;
		a_db[(size_t)virtual_actions::movFPR2] = b_movFPR2;
		a_db[(size_t)virtual_actions::movFPR3] = b_movFPR3;

		a_db[(size_t)virtual_actions::movEFPR0] = b_movEFPR0;
		a_db[(size_t)virtual_actions::movEFPR1] = b_movEFPR1;
		a_db[(size_t)virtual_actions::movEFPR2] = b_movEFPR2;
		a_db[(size_t)virtual_actions::movEFPR3] = b_movEFPR3;

		a_db[(size_t)virtual_actions::movRFPR0] = b_movRFPR0;
		a_db[(size_t)virtual_actions::movRFPR1] = b_movRFPR1;
		a_db[(size_t)virtual_actions::movRFPR2] = b_movRFPR2;
		a_db[(size_t)virtual_actions::movRFPR3] = b_movRFPR3;
#pragma endregion
		a_db[(size_t)virtual_actions::inc] = b_inc;
		a_db[(size_t)virtual_actions::dec] = b_dec;
		a_db[(size_t)virtual_actions::incFP] = b_incFP;
		a_db[(size_t)virtual_actions::decFP] = b_decFP;
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

		a_db[(size_t)virtual_actions::mulFPR0] = b_mulFPR0;
		a_db[(size_t)virtual_actions::mulFPR1] = b_mulFPR1;
		a_db[(size_t)virtual_actions::mulFPR2] = b_mulFPR2;
		a_db[(size_t)virtual_actions::mulFPR3] = b_mulFPR3;

		a_db[(size_t)virtual_actions::mulEFPR0] = b_mulEFPR0;
		a_db[(size_t)virtual_actions::mulEFPR1] = b_mulEFPR1;
		a_db[(size_t)virtual_actions::mulEFPR2] = b_mulEFPR2;
		a_db[(size_t)virtual_actions::mulEFPR3] = b_mulEFPR3;

		a_db[(size_t)virtual_actions::mulRFPR0] = b_mulRFPR0;
		a_db[(size_t)virtual_actions::mulRFPR1] = b_mulRFPR1;
		a_db[(size_t)virtual_actions::mulRFPR2] = b_mulRFPR2;
		a_db[(size_t)virtual_actions::mulRFPR3] = b_mulRFPR3;
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

		a_db[(size_t)virtual_actions::divFPR0] = b_divFPR0;
		a_db[(size_t)virtual_actions::divFPR1] = b_divFPR1;
		a_db[(size_t)virtual_actions::divFPR2] = b_divFPR2;
		a_db[(size_t)virtual_actions::divFPR3] = b_divFPR3;

		a_db[(size_t)virtual_actions::divEFPR0] = b_divEFPR0;
		a_db[(size_t)virtual_actions::divEFPR1] = b_divEFPR1;
		a_db[(size_t)virtual_actions::divEFPR2] = b_divEFPR2;
		a_db[(size_t)virtual_actions::divEFPR3] = b_divEFPR3;

		a_db[(size_t)virtual_actions::divRFPR0] = b_divRFPR0;
		a_db[(size_t)virtual_actions::divRFPR1] = b_divRFPR1;
		a_db[(size_t)virtual_actions::divRFPR2] = b_divRFPR2;
		a_db[(size_t)virtual_actions::divRFPR3] = b_divRFPR3;
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

		a_db[(size_t)virtual_actions::addFPR0] = b_addFPR0;
		a_db[(size_t)virtual_actions::addFPR1] = b_addFPR1;
		a_db[(size_t)virtual_actions::addFPR2] = b_addFPR2;
		a_db[(size_t)virtual_actions::addFPR3] = b_addFPR3;

		a_db[(size_t)virtual_actions::addEFPR0] = b_addEFPR0;
		a_db[(size_t)virtual_actions::addEFPR1] = b_addEFPR1;
		a_db[(size_t)virtual_actions::addEFPR2] = b_addEFPR2;
		a_db[(size_t)virtual_actions::addEFPR3] = b_addEFPR3;

		a_db[(size_t)virtual_actions::addRFPR0] = b_addRFPR0;
		a_db[(size_t)virtual_actions::addRFPR1] = b_addRFPR1;
		a_db[(size_t)virtual_actions::addRFPR2] = b_addRFPR2;
		a_db[(size_t)virtual_actions::addRFPR3] = b_addRFPR3;
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

		a_db[(size_t)virtual_actions::subFPR0] = b_subFPR0;
		a_db[(size_t)virtual_actions::subFPR1] = b_subFPR1;
		a_db[(size_t)virtual_actions::subFPR2] = b_subFPR2;
		a_db[(size_t)virtual_actions::subFPR3] = b_subFPR3;

		a_db[(size_t)virtual_actions::subEFPR0] = b_subEFPR0;
		a_db[(size_t)virtual_actions::subEFPR1] = b_subEFPR1;
		a_db[(size_t)virtual_actions::subEFPR2] = b_subEFPR2;
		a_db[(size_t)virtual_actions::subEFPR3] = b_subEFPR3;

		a_db[(size_t)virtual_actions::subRFPR0] = b_subRFPR0;
		a_db[(size_t)virtual_actions::subRFPR1] = b_subRFPR1;
		a_db[(size_t)virtual_actions::subRFPR2] = b_subRFPR2;
		a_db[(size_t)virtual_actions::subRFPR3] = b_subRFPR3;
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

		a_db[(size_t)virtual_actions::pushFP] = pushMemFPR;
		a_db[(size_t)virtual_actions::popFP] = popMemFPR;

		a_db[(size_t)virtual_actions::movsm] = movsm;
		a_db[(size_t)virtual_actions::movgm] = movgm;

		a_db[(size_t)virtual_actions::movsmSR] = movsmSR;
		a_db[(size_t)virtual_actions::movgmSR] = movgmSR;

		a_db[(size_t)virtual_actions::movsmCR] = movsmCR;
		a_db[(size_t)virtual_actions::movgmCR] = movgmCR;

		a_db[(size_t)virtual_actions::movsmFP] = movsmFPR;
		a_db[(size_t)virtual_actions::movgmFP] = movgmFPR;

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
		a_db[(size_t)virtual_actions::cmpdbl] = p_cmpdbl;

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
#pragma region ext_symbols
		a_db[(size_t)virtual_actions::rfread] = ex_rfread;
		a_db[(size_t)virtual_actions::rfwrite] = ex_rfwrite;
		a_db[(size_t)virtual_actions::rflen] = ex_rflen;
#pragma endregion
#pragma region threading
		a_db[(size_t)virtual_actions::pcrtthread] = p_crtthread;
		a_db[(size_t)virtual_actions::prstthread] = p_rstthread;
		a_db[(size_t)virtual_actions::pendthread] = p_endthread;
#pragma endregion
	}
};