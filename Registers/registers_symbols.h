#pragma once

#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "../Memory/memory_decl.h"
#include "regs_decl.h"

enum registries_def {
	AL = 0x000001,
	AH = 0x000010,

	BL = 0x000002,
	BH = 0x000020,

	CL = 0x000003,
	CH = 0x000030,

	DL = 0x000004,
	DH = 0x000040,


	AX = 0x000005,
	BX = 0x000006,
	CX = 0x000007,
	DX = 0x000008,

	EAX = 0x000050,
	EBX = 0x000060,
	ECX = 0x000070,
	EDX = 0x000080,

	RAX = 0x000055,
	RBX = 0x000066,
	RCX = 0x000077,
	RDX = 0x000088
};

enum extra_registries {
	SR = 0x000009,
	CR = 0x00000A,
	DR = 0x00000B,
};

// Native registers ops
#pragma region native_regs_ops
void b_set16AX(std::shared_ptr<void> a, regs* regsiters, memory* unused_m);
void b_set16BX(std::shared_ptr<void> a, regs* registers, memory* unused_m);
void b_set16CX(std::shared_ptr<void> a, regs* registers, memory* unused_m);
void b_set16DX(std::shared_ptr<void> a, regs* registers, memory* unused_m);

void b_set32EAX(std::shared_ptr<void> a, regs* registers, memory* unused_m);
void b_set32EBX(std::shared_ptr<void> a, regs* registers, memory* unused_m);
void b_set32ECX(std::shared_ptr<void> a, regs* registers, memory* unused_m);
void b_set32EDX(std::shared_ptr<void> a, regs* registers, memory* unused_m);

void b_set64RAX(std::shared_ptr<void> a, regs* registers, memory* unused_m);
void b_set64RBX(std::shared_ptr<void> a, regs* registers, memory* unused_m);
void b_set64RCX(std::shared_ptr<void> a, regs* registers, memory* unused_m);
void b_set64RDX(std::shared_ptr<void> a, regs* registers, memory* unused_m);

void b_setSR(std::shared_ptr<void> a, regs* registers, memory* unused_m);
void b_setCR(std::shared_ptr<void> a, regs* registers, memory* unused_m);
void b_setDR(std::shared_ptr<void> a, regs* registers, memory* unused_m);


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
void b_mov16AX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_mov16BX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_mov16CX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_mov16DX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);

void b_mov32EAX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_mov32EBX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_mov32ECX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_mov32EDX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);

void b_mov64RAX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_mov64RBX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_mov64RCX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_mov64RDX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
#pragma endregion

// Native maths ops
#pragma region native_maths_ops
void b_inc(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_dec(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_incDR(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m);
void b_decDR(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m);

void b_mul16AX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_mul16BX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_mul16CX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_mul16DX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);

void b_mul32EAX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_mul32EBX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_mul32ECX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_mul32EDX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);

void b_mul64RAX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_mul64RBX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_mul64RCX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_mul64RDX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);

void b_div16AX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_div16BX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_div16CX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_div16DX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);

void b_div32EAX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_div32EBX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_div32ECX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_div32EDX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);

void b_div64RAX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_div64RBX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_div64RCX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_div64RDX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);

void b_add16AX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_add16BX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_add16CX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_add16DX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);

void b_add32EAX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_add32EBX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_add32ECX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_add32EDX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);

void b_add64RAX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_add64RBX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_add64RCX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_add64RDX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);

void b_sub16AX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_sub16BX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_sub16CX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_sub16DX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);

void b_sub32EAX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_sub32EBX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_sub32ECX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_sub32EDX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);

void b_sub64RAX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_sub64RBX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_sub64RCX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_sub64RDX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
#pragma endregion

//extended ops
#pragma region extended_ops
void b_getInput(std::shared_ptr<void> unused_p, regs* registers, memory* mem);
void b_toString(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
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
void b_not(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_log2(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_log10(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_log(std::shared_ptr<void> reg, regs* registers, memory* unused_m);

void b_dlog2(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_dlog10(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_dlog(std::shared_ptr<void> reg, regs* registers, memory* unused_m);


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
	registries_ptr_table(regs* _registers) {
		this->registers = _registers;
		this->init();
	}

#pragma warning (push)
#pragma warning (disable:26812)
	void* access(registries_def reg_id) {
		return this->table[reg_id];
	}
#pragma warning (pop)

private:
	regs* registers;
	void* table[0x000088 + 1];

	void init() {
		table[registries_def::AX] = registers->ax;
		table[registries_def::BX] = registers->bx;
		table[registries_def::CX] = registers->cx;
		table[registries_def::DX] = registers->dx;

		table[registries_def::EAX] = registers->eax;
		table[registries_def::EBX] = registers->ebx;
		table[registries_def::ECX] = registers->ecx;
		table[registries_def::EDX] = registers->edx;

		table[registries_def::RAX] = registers->rax;
		table[registries_def::RBX] = registers->rbx;
		table[registries_def::RCX] = registers->rcx;
		table[registries_def::RDX] = registers->rdx;
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
		return this->table[reg_id];
	}
#pragma warning (pop)
private:
	regs* registers;
	void* table[0x00000B + 1];

	void init() {
		table[extra_registries::SR] = registers->sr;
		table[extra_registries::CR] = registers->cr;
		table[extra_registries::DR] = registers->dr;
	}
};