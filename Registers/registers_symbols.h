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

enum class registries_def {
	AL = 0x01,
	AH = 0x02,
	BL = 0x03,
	BH = 0x04,
	CL = 0x05,
	CH = 0x06,
	DL = 0x07,
	DH = 0x08,

	AX = 0x09,
	BX = 0x0A,
	CX = 0x0B,
	DX = 0x0C,

	EAX = 0x0D,
	EBX = 0x0E,
	ECX = 0x0F,
	EDX = 0x10,

	RAX = 0x11,
	RBX = 0x12,
	RCX = 0x13,
	RDX = 0x14,

	RBP = 0x15,
	RSP = 0x16
};

enum class extra_registries {
	SR = 0x17,
	CR = 0x18,
	DR = 0x19,
};

// Arg_Tuple to reg id
inline registries_def ATTOREGID(arg_tuple& at, memory*& mem) {
	unsigned char* temp = new unsigned char[sizeof(size_t)];
	mem->_ROZVG(temp, sizeof(size_t), std::get<0>(at));
	registries_def reg_id = (registries_def)(ATOULL(temp));

	delete[] temp;
	return reg_id;
}

inline registries_def ATTOREGID(std::shared_ptr<void>& at_ptr, memory*& mem) {
	return ATTOREGID(*std::static_pointer_cast<arg_tuple>(at_ptr), mem);
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
void b_setDR(std::shared_ptr<void> a, regs* registers, memory* mem);


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
#pragma endregion

// Native maths ops
#pragma region native_maths_ops
void b_inc(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_dec(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_incDR(std::shared_ptr<void> unused_p, regs* registers, memory* mem);
void b_decDR(std::shared_ptr<void> unused_p, regs* registers, memory* mem);

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
void b_DRToSR(std::shared_ptr<void> unused_p, regs* registers, memory* mem);
void b_DRToULL(std::shared_ptr<void> unused_p, regs* registers, memory* mem);
void b_DRToLL(std::shared_ptr<void> unused_p, regs* registers, memory* mem);
#pragma endregion

// Native binary ops
#pragma region native_bin_ops
void b_not(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_log2(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_log10(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_log(std::shared_ptr<void> reg, regs* registers, memory* mem);

void b_dlog2(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m);
void b_dlog10(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m);
void b_dlog(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m);


void b_and(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_or(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_xor(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_shl(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_shr(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_pow(std::shared_ptr<void> reg, regs* registers, memory* mem);

void b_dpow(std::shared_ptr<void> unused_p, regs* registers, memory* mem);
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
	void* access(extra_registries reg_id) {
		return this->table[(size_t)reg_id];
	}
#pragma warning (pop)
private:
	regs* registers;
	void* table[0x19 + 1];

	void init() {
		table[(size_t)extra_registries::SR] = registers->sr;
		table[(size_t)extra_registries::CR] = registers->cr;
		table[(size_t)extra_registries::DR] = registers->dr;
	}
};

template<typename T>
inline void b_set_num(const std::shared_ptr<void>& args_ptr, regs*& registers, memory* const& mem, registries_def reg_id) {
	unsigned char* uc_n = nullptr;
	try {
		const auto [vaddr, vsize] = *std::static_pointer_cast<std::tuple<size_t, size_t>>(args_ptr);
		uc_n = new unsigned char[sizeof(size_t)];
		mem->_ROZVG(uc_n, sizeof(size_t), vaddr);

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
		mem->_ROZVG(uc_s, vsize, vaddr);

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
		mem->_ROZVG(uc_c, 1, vaddr);

		registers->cr->set((char)uc_c[0]);
		delete[] uc_c;
	}
	catch (...) {
		if (uc_c)
			delete[] uc_c;
	}
}
inline void b_set_dbl(const std::shared_ptr<void>& args_ptr, regs*& registers, memory* const& mem) {
	unsigned char* uc_d = nullptr;
	try {
		const auto [vaddr, vsize] = *std::static_pointer_cast<std::tuple<size_t, size_t>>(args_ptr);

		uc_d = new unsigned char[sizeof(double)];
		mem->_ROZVG(uc_d, sizeof(double), vaddr);

		registers->dr->set(ATOD(uc_d));
		delete[] uc_d;
	}
	catch (...) {
		if (uc_d)
			delete[] uc_d;
	}
}