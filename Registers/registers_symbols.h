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
	#define RPH_BIO_ARGS CUSTOM_STD_ARGS(a, registers, mem)
	#define RPH_MOV_ARGS CUSTOM_STD_ARGS(reg, registers, mem)
	#define RPH_NMATHS_ARGS RPH_MOV_ARGS

	#define RPH_FPR_PROTO(io_type, regname)	void b_##io_type##regname(RPH_BIO_ARGS)

	#define RPH_FPR_MOV_PROTO(regname) void b_mov##regname(RPH_MOV_ARGS)

	#define RPH_FPR_NMATHS_PROTO(optype, regname) void b_##optype##regname(RPH_NMATHS_ARGS)
	#define RPH_FPR_NMATHS_MUL_PROTO(regname) RPH_FPR_NMATHS_PROTO(mul, regname)
	#define RPH_FPR_NMATHS_DIV_PROTO(regname) RPH_FPR_NMATHS_PROTO(div, regname)
	#define RPH_FPR_NMATHS_ADD_PROTO(regname) RPH_FPR_NMATHS_PROTO(add, regname)
	#define RPH_FPR_NMATHS_SUB_PROTO(regname) RPH_FPR_NMATHS_PROTO(sub, regname)
	#define RPH_FPR_NMATHS_ALL_PROTO(regname)	\
		RPH_FPR_NMATHS_MUL_PROTO(regname);		\
		RPH_FPR_NMATHS_DIV_PROTO(regname);		\
		RPH_FPR_NMATHS_ADD_PROTO(regname);		\
		RPH_FPR_NMATHS_SUB_PROTO(regname)

	#define RPH(...) (void)0
#endif

enum class registries_def {
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
	RSP
};

enum class extra_registries {
	SR = 0x17,
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

// Arg_Tuple to reg id
inline registries_def ATTOREGID(arg_tuple& at, memory* const& mem) {
	unsigned char* temp = new unsigned char[sizeof(size_t)];
	mem->_MG(temp, sizeof(size_t), std::get<0>(at));
	registries_def reg_id = (registries_def)(ATOULL(temp));

	delete[] temp;
	return reg_id;
}
inline registries_def ATTOREGID(std::shared_ptr<void>& at_ptr, memory* const& mem) {
	return ATTOREGID(*std::static_pointer_cast<arg_tuple>(at_ptr), mem);
}
inline extra_registries ATTOXREGID(arg_tuple& at, memory* const& mem) {
	unsigned char* temp = new unsigned char[sizeof(size_t)]; 
	mem->_MG(temp, sizeof(size_t), std::get<0>(at));
	extra_registries xreg_id = (extra_registries)(ATOULL(temp));

	delete[] temp;
	return xreg_id;
}
inline extra_registries ATTOXREGID(std::shared_ptr<void>& at_ptr, memory* const& mem) {
	return ATTOXREGID(*std::static_pointer_cast<arg_tuple>(at_ptr), mem);
}

inline bool is_reg_fpreg(const extra_registries& regid) {
	if (regid >= extra_registries::FPR0 && regid <= extra_registries::RFPR3)
		return true;
	else
		return false;
}

// Native registers ops
#pragma region native_regs_ops
void b_set16AX(std::shared_ptr<void> a, regs* regsiters, memory* mem);
void b_set16BX(std::shared_ptr<void> a, regs* registers, memory* mem);
void b_set16CX(std::shared_ptr<void> a, regs* registers, memory* mem);
void b_set16DX(std::shared_ptr<void> a, regs* registers, memory* mem);

void b_set32EAX(std::shared_ptr<void> a, regs* registers, memory* mem);
void b_set32EBX(std::shared_ptr<void> a, regs* registers, memory* mem);
void b_set32ECX(std::shared_ptr<void> a, regs* registers, memory* mem);
void b_set32EDX(std::shared_ptr<void> a, regs* registers, memory* mem);

void b_set64RAX(std::shared_ptr<void> a, regs* registers, memory* mem);
void b_set64RBX(std::shared_ptr<void> a, regs* registers, memory* mem);
void b_set64RCX(std::shared_ptr<void> a, regs* registers, memory* mem);
void b_set64RDX(std::shared_ptr<void> a, regs* registers, memory* mem);

void b_setSR(std::shared_ptr<void> a, regs* registers, memory* mem);
void b_setCR(std::shared_ptr<void> a, regs* registers, memory* mem);

RPH_FPR_PROTO(set, FPR0);
RPH_FPR_PROTO(set, FPR1);
RPH_FPR_PROTO(set, FPR2);
RPH_FPR_PROTO(set, FPR3);
RPH_FPR_PROTO(set, EFPR0);
RPH_FPR_PROTO(set, EFPR1);
RPH_FPR_PROTO(set, EFPR2);
RPH_FPR_PROTO(set, EFPR3);
RPH_FPR_PROTO(set, RFPR0);
RPH_FPR_PROTO(set, RFPR1);
RPH_FPR_PROTO(set, RFPR2);
RPH_FPR_PROTO(set, RFPR3);

void b_get16AX(std::shared_ptr<void> receiver, regs* registers, memory* unused_m);
void b_get16BX(std::shared_ptr<void> receiver, regs* registers, memory* unused_m);
void b_get16CX(std::shared_ptr<void> receiver, regs* registers, memory* unused_m);
void b_get16DX(std::shared_ptr<void> receiver, regs* registers, memory* unused_m);

void b_get32EAX(std::shared_ptr<void> receiver, regs* registers, memory* unused_m);
void b_get32EBX(std::shared_ptr<void> receiver, regs* registers, memory* unused_m);
void b_get32ECX(std::shared_ptr<void> receiver, regs* registers, memory* unused_m);
void b_get32EDX(std::shared_ptr<void> receiver, regs* registers, memory* unused_m);

void b_get64RAX(std::shared_ptr<void> receiver, regs* registers, memory* unused_m);
void b_get64RBX(std::shared_ptr<void> receiver, regs* registers, memory* unused_m);
void b_get64RCX(std::shared_ptr<void> receiver, regs* registers, memory* unused_m);
void b_get64RDX(std::shared_ptr<void> receiver, regs* registers, memory* unused_m);

void b_getSR(std::shared_ptr<void> receiver, regs* registers, memory* unused_m);
void b_getCR(std::shared_ptr<void> receiver, regs* registers, memory* unused_m);
void b_getDR(std::shared_ptr<void> receiver, regs* registers, memory* unused_m);

#pragma endregion

// Mov ops
#pragma region mov_ops
void b_mov16AX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_mov16BX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_mov16CX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_mov16DX(std::shared_ptr<void> reg, regs* registers, memory* mem);

void b_mov32EAX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_mov32EBX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_mov32ECX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_mov32EDX(std::shared_ptr<void> reg, regs* registers, memory* mem);

void b_mov64RAX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_mov64RBX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_mov64RCX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_mov64RDX(std::shared_ptr<void> reg, regs* registers, memory* mem);

void b_mov64RBP(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_mov64RSP(std::shared_ptr<void> reg, regs* registers, memory* mem);

RPH_FPR_MOV_PROTO(FPR0);
RPH_FPR_MOV_PROTO(FPR1);
RPH_FPR_MOV_PROTO(FPR2);
RPH_FPR_MOV_PROTO(FPR3);

RPH_FPR_MOV_PROTO(EFPR0);
RPH_FPR_MOV_PROTO(EFPR1);
RPH_FPR_MOV_PROTO(EFPR2);
RPH_FPR_MOV_PROTO(EFPR3);

RPH_FPR_MOV_PROTO(RFPR0);
RPH_FPR_MOV_PROTO(RFPR1);
RPH_FPR_MOV_PROTO(RFPR2);
RPH_FPR_MOV_PROTO(RFPR3);
#pragma endregion

// Native maths ops
#pragma region native_maths_ops
void b_inc(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_dec(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_incDR(std::shared_ptr<void> unused_p, regs* registers, memory* mem);
void b_decDR(std::shared_ptr<void> unused_p, regs* registers, memory* mem);
void b_incFP(RPH_NMATHS_ARGS);
void b_decFP(RPH_NMATHS_ARGS);

void b_mul16AX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_mul16BX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_mul16CX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_mul16DX(std::shared_ptr<void> reg, regs* registers, memory* mem);

void b_mul32EAX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_mul32EBX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_mul32ECX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_mul32EDX(std::shared_ptr<void> reg, regs* registers, memory* mem);

void b_mul64RAX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_mul64RBX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_mul64RCX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_mul64RDX(std::shared_ptr<void> reg, regs* registers, memory* mem);

void b_div16AX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_div16BX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_div16CX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_div16DX(std::shared_ptr<void> reg, regs* registers, memory* mem);

void b_div32EAX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_div32EBX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_div32ECX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_div32EDX(std::shared_ptr<void> reg, regs* registers, memory* mem);

void b_div64RAX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_div64RBX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_div64RCX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_div64RDX(std::shared_ptr<void> reg, regs* registers, memory* mem);

void b_add16AX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_add16BX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_add16CX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_add16DX(std::shared_ptr<void> reg, regs* registers, memory* mem);

void b_add32EAX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_add32EBX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_add32ECX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_add32EDX(std::shared_ptr<void> reg, regs* registers, memory* mem);

void b_add64RAX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_add64RBX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_add64RCX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_add64RDX(std::shared_ptr<void> reg, regs* registers, memory* mem);

void b_add64RBP(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_add64RSP(std::shared_ptr<void> reg, regs* registers, memory* mem);

void b_sub16AX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_sub16BX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_sub16CX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_sub16DX(std::shared_ptr<void> reg, regs* registers, memory* mem);

void b_sub32EAX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_sub32EBX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_sub32ECX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_sub32EDX(std::shared_ptr<void> reg, regs* registers, memory* mem);

void b_sub64RAX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_sub64RBX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_sub64RCX(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_sub64RDX(std::shared_ptr<void> reg, regs* registers, memory* mem);

void b_sub64RBP(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_sub64RSP(std::shared_ptr<void> reg, regs* registers, memory* mem);

RPH_FPR_NMATHS_ALL_PROTO(FPR0);
RPH_FPR_NMATHS_ALL_PROTO(FPR1);
RPH_FPR_NMATHS_ALL_PROTO(FPR2);
RPH_FPR_NMATHS_ALL_PROTO(FPR3);

RPH_FPR_NMATHS_ALL_PROTO(EFPR0);
RPH_FPR_NMATHS_ALL_PROTO(EFPR1);
RPH_FPR_NMATHS_ALL_PROTO(EFPR2);
RPH_FPR_NMATHS_ALL_PROTO(EFPR3);

RPH_FPR_NMATHS_ALL_PROTO(RFPR0);
RPH_FPR_NMATHS_ALL_PROTO(RFPR1);
RPH_FPR_NMATHS_ALL_PROTO(RFPR2);
RPH_FPR_NMATHS_ALL_PROTO(RFPR3);
#pragma endregion

//extended ops
#pragma region extended_ops
void b_getInput(std::shared_ptr<void> unused_p, regs* registers, memory* mem);
void b_toString(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_mergeString(std::shared_ptr<void> unused_p, regs* registers, memory* mem);
void b_substring(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m);
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

struct registries_ptr_table {
public:
	registries_ptr_table(regs*& _registers) {
		this->registers = _registers;
		this->init();
	}

#pragma warning (push)
#pragma warning (disable:26812)
	void* access(registries_def reg_id) {
		return this->table[(size_t)reg_id];
	}
#pragma warning (pop)

private:
	regs* registers;
	void* table[0x16 + 1];

	void init() {
		table[(size_t)registries_def::AX] = registers->ax;
		table[(size_t)registries_def::BX] = registers->bx;
		table[(size_t)registries_def::CX] = registers->cx;
		table[(size_t)registries_def::DX] = registers->dx;

		table[(size_t)registries_def::EAX] = registers->eax;
		table[(size_t)registries_def::EBX] = registers->ebx;
		table[(size_t)registries_def::ECX] = registers->ecx;
		table[(size_t)registries_def::EDX] = registers->edx;

		table[(size_t)registries_def::RAX] = registers->rax;
		table[(size_t)registries_def::RBX] = registers->rbx;
		table[(size_t)registries_def::RCX] = registers->rcx;
		table[(size_t)registries_def::RDX] = registers->rdx;

		table[(size_t)registries_def::RBP] = registers->rbp;
		table[(size_t)registries_def::RSP] = registers->rsp;
	}
};

struct extra_registries_ptr_table {
public:
	extra_registries_ptr_table(regs* _registers) {
		this->registers = _registers;
		this->init();
	}
#pragma warning (push)
#pragma warning (disable : 26812)
	void* access(const extra_registries& reg_id) {
		return this->table[(size_t)reg_id];
	}
#pragma warning (pop)
private:
	regs* registers;
	void* table[(size_t)extra_registries::RFPR3 + 1];

	void init() {
		table[(size_t)extra_registries::SR] = registers->sr;
		table[(size_t)extra_registries::CR] = registers->cr;

		table[(size_t)extra_registries::FPR0] = registers->fpr0;
		table[(size_t)extra_registries::FPR1] = registers->fpr1;
		table[(size_t)extra_registries::FPR2] = registers->fpr2;
		table[(size_t)extra_registries::FPR3] = registers->fpr3;

		table[(size_t)extra_registries::EFPR0] = registers->efpr0;
		table[(size_t)extra_registries::EFPR1] = registers->efpr1;
		table[(size_t)extra_registries::EFPR2] = registers->efpr2;
		table[(size_t)extra_registries::EFPR3] = registers->efpr3;

		table[(size_t)extra_registries::RFPR0] = registers->rfpr0;
		table[(size_t)extra_registries::RFPR1] = registers->rfpr1;
		table[(size_t)extra_registries::RFPR2] = registers->rfpr2;
		table[(size_t)extra_registries::RFPR3] = registers->rfpr3;
	}
};

template<typename T>
inline void b_set_num(const std::shared_ptr<void>& args_ptr, regs*& registers, memory* const& mem, registries_def reg_id) {
	unsigned char* uc_n = nullptr;
	try {
		const auto [vaddr, vsize] = *std::static_pointer_cast<arg_tuple>(args_ptr); 
		uc_n = new unsigned char[sizeof(size_t)];
		mem->_MG(uc_n, sizeof(size_t), vaddr);

		registries_ptr_table ptr_table = registries_ptr_table(registers);
		((reg_int<size_t>*)ptr_table.access(reg_id))->set((T)ATOULL(uc_n));

		delete[] uc_n;
	}
	catch (...) {
		if (uc_n)
			delete[] uc_n;
	}
}
inline void b_set_str(const std::shared_ptr<void>& args_ptr, regs*& registers, memory* const& mem) {
	unsigned char* uc_s = nullptr;
	try {
		const auto [vaddr, vsize] = *std::static_pointer_cast<std::tuple<size_t, size_t>>(args_ptr);

		uc_s = new unsigned char[vsize];
		mem->_MG(uc_s, vsize, vaddr);

		registers->sr->set(std::string((const char*)uc_s));
		delete[] uc_s;
	}
	catch (...) {
		if (uc_s)
			delete[] uc_s;
	}
}
inline void b_set_chr(const std::shared_ptr<void>& args_ptr, regs*& registers, memory* const& mem) {
	unsigned char* uc_c = nullptr;
	try {
		const auto [vaddr, vsize] = *std::static_pointer_cast<std::tuple<size_t, size_t>>(args_ptr);
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

inline long double b_fetch_dbl_args(const std::shared_ptr<void>& args_ptr, regs*& registers, memory* const& mem, bool& exitcode) {
	unsigned char* uc_d = nullptr;
	try {
		const auto [vaddr, vsize] = *std::static_pointer_cast<std::tuple<size_t, size_t>>(args_ptr);

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