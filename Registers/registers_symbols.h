#pragma once

#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include "../utility.h"
#include "../Memory/memory_decl.h"
#include "regs_decl.h"

//RPH: Regs Preproc Header
#ifndef RPH
	#define COMN_NMATHS_ARGS CUSTOM_STD_ARGS(reg, registers, mem)
	#define RPH(...) (void)0
#endif

enum class comn_registers {
	AL = 0x01,
	AH,
	BL,
	BH,
	CL,
	CH,
	DL,
	DH,

	AX,
	BX,
	CX,
	DX,

	EAX,
	EBX,
	ECX,
	EDX,

	RAX,
	RBX,
	RCX,
	RDX,

	RBP,
	RSP,

	RDI,
	RSI,

	CR,

	FPR0,
	FPR1,
	FPR2,
	FPR3,

	EFPR0,
	EFPR1,
	EFPR2,
	EFPR3,

	RFPR0,
	RFPR1,
	RFPR2,
	RFPR3
};

inline comn_registers ATTOREGID(arg_tuple& at, memory* const& mem) {
	std::unique_ptr<uint8_t[]> temp = std::make_unique<uint8_t[]>(sizeof(size_t));
	mem->_MG(temp.get(), sizeof(size_t), std::get<0>(at));
	comn_registers reg_id = (comn_registers)(ATOULL(temp.get()));
	
	return reg_id;
}
inline comn_registers ATTOREGID(std::shared_ptr<void>& at_ptr, memory* const& mem) {
	return ATTOREGID(*std::static_pointer_cast<arg_tuple>(at_ptr), mem);
}

void b_setGP(std::shared_ptr<void> a, regs* registers, memory* mem);
void b_movGP(std::shared_ptr<void> reg, regs* registers, memory* mem);

// Native maths ops
void b_incGP(COMN_NMATHS_ARGS);
void b_decGP(COMN_NMATHS_ARGS);

void b_mulGP(COMN_NMATHS_ARGS);
void b_divGP(COMN_NMATHS_ARGS);
void b_addGP(COMN_NMATHS_ARGS);
void b_subGP(COMN_NMATHS_ARGS);

//extended ops
#pragma region extended_ops
void b_getInput(std::shared_ptr<void> unused_p, regs* registers, memory* mem);
void b_toString(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_strlen(std::shared_ptr<void> unused_p, regs* registers, memory* mem);

void b_print(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m);
void b_println(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m);
void b_printEOL(std::shared_ptr<void> unused_p, regs* unused_r, memory* unused_m);

void b_castreg(std::shared_ptr<void> receiver, regs* registers, memory* mem);
void b_recast(std::shared_ptr<void> unused_p, regs* registers, memory* mem);
void b_fromString(std::shared_ptr<void> unused_p, regs* registers, memory* mem);
void b_CRToSR(std::shared_ptr<void> unused_p, regs* registers, memory* mem);
void b_RevSR(std::shared_ptr<void> unused_p, regs* registers, memory* mem);
void b_FPToSR(std::shared_ptr<void> unused_p, regs* registers, memory* mem);
void b_FPToULL(std::shared_ptr<void> unused_p, regs* registers, memory* mem);
void b_FPToLL(std::shared_ptr<void> unused_p, regs* registers, memory* mem);
#pragma endregion

// Native binary ops
#pragma region native_bin_ops
void b_not(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_log2(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_log10(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_log(std::shared_ptr<void> reg, regs* registers, memory* mem);

void b_dlog2(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_dlog10(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_dlog(std::shared_ptr<void> reg, regs* registers, memory* mem);


void b_and(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_or(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_xor(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_shl(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_shr(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_pow(std::shared_ptr<void> reg, regs* registers, memory* mem);

void b_dpow(std::shared_ptr<void> reg, regs* registers, memory* mem);
#pragma endregion

struct comn_registers_table {
public:
	comn_registers_table(regs* const& _registers) {
		this->registers = _registers;
		this->init();
	}

	void* access(comn_registers reg_id) {
		return this->table[(size_t)reg_id];
	}

	static bool is_num_reg(comn_registers reg_id) {
		if ((size_t)reg_id < (size_t)comn_registers::CR)
			return true;
		else
			return false;
	}
	static bool is_fp_reg(comn_registers reg_id) {
		if ((size_t)reg_id >= (size_t)comn_registers::FPR0 && (size_t)reg_id <= (size_t)comn_registers::RFPR3)
			return true;
		else
			return false;
	}
private:
	regs* registers;
	void* table[(size_t)comn_registers::RFPR3 + 1];
	
	void init() {
		table[(size_t)comn_registers::AX] = registers->ax;
		table[(size_t)comn_registers::BX] = registers->bx;
		table[(size_t)comn_registers::CX] = registers->cx;
		table[(size_t)comn_registers::DX] = registers->dx;

		table[(size_t)comn_registers::EAX] = registers->eax;
		table[(size_t)comn_registers::EBX] = registers->ebx;
		table[(size_t)comn_registers::ECX] = registers->ecx;
		table[(size_t)comn_registers::EDX] = registers->edx;

		table[(size_t)comn_registers::RAX] = registers->rax;
		table[(size_t)comn_registers::RBX] = registers->rbx;
		table[(size_t)comn_registers::RCX] = registers->rcx;
		table[(size_t)comn_registers::RDX] = registers->rdx;

		table[(size_t)comn_registers::RBP] = registers->rbp;
		table[(size_t)comn_registers::RSP] = registers->rsp;
		table[(size_t)comn_registers::RDI] = registers->rdi;
		table[(size_t)comn_registers::RSI] = registers->rsi;

		table[(size_t)comn_registers::CR] = registers->cr;

		table[(size_t)comn_registers::FPR0] = registers->fpr0;
		table[(size_t)comn_registers::FPR1] = registers->fpr1;
		table[(size_t)comn_registers::FPR2] = registers->fpr2;
		table[(size_t)comn_registers::FPR3] = registers->fpr3;

		table[(size_t)comn_registers::EFPR0] = registers->efpr0;
		table[(size_t)comn_registers::EFPR1] = registers->efpr1;
		table[(size_t)comn_registers::EFPR2] = registers->efpr2;
		table[(size_t)comn_registers::EFPR3] = registers->efpr3;

		table[(size_t)comn_registers::RFPR0] = registers->rfpr0;
		table[(size_t)comn_registers::RFPR1] = registers->rfpr1;
		table[(size_t)comn_registers::RFPR2] = registers->rfpr2;
		table[(size_t)comn_registers::RFPR3] = registers->rfpr3;
	}
};

template<typename T>
inline void b_set_num(const std::shared_ptr<void>& args_ptr, regs*& registers, memory* const& mem, comn_registers reg_id) {
	if (!comn_registers_table::is_num_reg(reg_id))
		return;

	unsigned char* uc_n = nullptr;
	try {
		const auto [vaddr, vsize, vopt] = *std::static_pointer_cast<arg_tuple>(args_ptr);
		uc_n = new unsigned char[sizeof(size_t)];
		mem->_MG(uc_n, sizeof(size_t), vaddr);

		((reg_int<size_t>*)comn_registers_table(registers).access(reg_id))->set((T)ATOULL(uc_n));

		delete[] uc_n;
	}
	catch (...) {
		if (uc_n)
			delete[] uc_n;
	}
}
inline void b_set_chr(const std::shared_ptr<void>& args_ptr, regs*& registers, memory* const& mem) {
	unsigned char* uc_c = nullptr;
	try {
		const auto [vaddr, vsize, vopt] = *std::static_pointer_cast<arg_tuple>(args_ptr);
		uc_c = new unsigned char[1];
		mem->_MG(uc_c, 1, vaddr);

		registers->cr->set((char)uc_c[0]);
		delete[] uc_c;
	}
	catch (...) {
		if (uc_c)
			delete[] uc_c;
	}
}

inline long double b_fetch_dbl_args(const std::shared_ptr<void>& args_ptr, memory* const& mem, bool& exitcode) {
	unsigned char* uc_d = nullptr;
	try {
		const auto [vaddr, vsize, vopt] = *std::static_pointer_cast<arg_tuple>(args_ptr);

		uc_d = new unsigned char[sizeof(long double)];
		mem->_MG(uc_d, sizeof(long double), vaddr);

		long double res = ATOLD(uc_d);
		delete[] uc_d;

		exitcode ^= exitcode; // set exitcode to false (0)
		return res;
	}
	catch (...) {
		if (uc_d)
			delete[] uc_d;

		exitcode = true;
		return 0;
	}
}