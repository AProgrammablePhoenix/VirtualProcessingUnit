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
	set,
	mov,

	inc,
	dec,

	mul,
	div,
	add,
	sub,

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

	inline static void has_been_destroyed() {
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
		a_db[(size_t)virtual_actions::set] = b_setGP;
		a_db[(size_t)virtual_actions::mov] = b_movGP;

		a_db[(size_t)virtual_actions::inc] = b_incGP;
		a_db[(size_t)virtual_actions::dec] = b_decGP;

		a_db[(size_t)virtual_actions::mul] = b_mulGP;
		a_db[(size_t)virtual_actions::div] = b_divGP;
		a_db[(size_t)virtual_actions::add] = b_addGP;
		a_db[(size_t)virtual_actions::sub] = b_subGP;
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