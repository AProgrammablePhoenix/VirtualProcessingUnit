#include <iostream>
#include <memory>
#include <string>

#include "../utility.h"
#include "regs_decl.h"
#include "registers_symbols.h"

#ifndef NMATHS_PREPROC
	#define GLOBL_ARGS CUSTOM_STD_ARGS(reg, registers, mem)
	#define GLOBL_ARGS_D1 CUSTOM_STD_ARGS(unused_p, registers, mem)
	
	#define NATIVE_INCDEC(opsign) \
		registries_def reg_id = ATTOREGID(reg, mem); \
		registries_ptr_table ptr_table = registries_ptr_table(registers); \
		((reg_int<size_t>*)ptr_table.access(reg_id))->set(((reg_int<size_t>*)ptr_table.access(reg_id))->get() ##opsign 1)

	#define FP_INCDEC(reg, opsign) \
		registers->reg->set(registers->reg->get() ##opsign 1)

	#define NATIVE_OP(opreg, opsign, opsize) \
		registries_def reg_id = ATTOREGID(reg, mem); \
		registries_ptr_table ptr_table = registries_ptr_table(registers); \
		opsize value = ((reg_int<opsize>*)ptr_table.access(reg_id))->get(); \
		registers->opreg->set(registers->opreg->get() opsign value);

	#define NMATH_PREPROC(...) (void)0
#endif

void b_inc(GLOBL_ARGS) {
	NATIVE_INCDEC(+);
}
void b_dec(GLOBL_ARGS) {
	NATIVE_INCDEC(-);
}

void b_incDR(GLOBL_ARGS_D1) {
	FP_INCDEC(dr, +);
}
void b_decDR(GLOBL_ARGS_D1) {
	FP_INCDEC(dr, -);
}

void b_mul16AX(GLOBL_ARGS) {
	NATIVE_OP(ax, *, unsigned short);
}
void b_mul16BX(GLOBL_ARGS) {
	NATIVE_OP(bx, *, unsigned short);
}
void b_mul16CX(GLOBL_ARGS) {
	NATIVE_OP(cx, *, unsigned short);
}
void b_mul16DX(GLOBL_ARGS) {
	NATIVE_OP(dx, *, unsigned short);
}

void b_mul32EAX(GLOBL_ARGS) {
	NATIVE_OP(eax, *, unsigned int);
}
void b_mul32EBX(GLOBL_ARGS) {
	NATIVE_OP(ebx, *, unsigned int);
}
void b_mul32ECX(GLOBL_ARGS) {
	NATIVE_OP(ecx, *, unsigned int);
}
void b_mul32EDX(GLOBL_ARGS) {
	NATIVE_OP(edx, *, unsigned int);
}

void b_mul64RAX(GLOBL_ARGS) {
	NATIVE_OP(rax, *, size_t);
}
void b_mul64RBX(GLOBL_ARGS) {
	NATIVE_OP(rbx, *, size_t);
}
void b_mul64RCX(GLOBL_ARGS) {
	NATIVE_OP(rcx, *, size_t);
}
void b_mul64RDX(GLOBL_ARGS) {
	NATIVE_OP(rdx, *, size_t);
}


void b_div16AX(GLOBL_ARGS) {
	NATIVE_OP(ax, /, unsigned short);
}
void b_div16BX(GLOBL_ARGS) {
	NATIVE_OP(bx, / , unsigned short);
}
void b_div16CX(GLOBL_ARGS) {
	NATIVE_OP(cx, / , unsigned short);
}
void b_div16DX(GLOBL_ARGS) {
	NATIVE_OP(dx, / , unsigned short);
}

void b_div32EAX(GLOBL_ARGS) {
	NATIVE_OP(eax, / , unsigned int);
}
void b_div32EBX(GLOBL_ARGS) {
	NATIVE_OP(ebx, / , unsigned int);
}
void b_div32ECX(GLOBL_ARGS) {
	NATIVE_OP(ecx, / , unsigned int);
}
void b_div32EDX(GLOBL_ARGS) {
	NATIVE_OP(edx, / , unsigned int);
}

void b_div64RAX(GLOBL_ARGS) {
	NATIVE_OP(rax, / , size_t);
}
void b_div64RBX(GLOBL_ARGS) {
	NATIVE_OP(rbx, / , size_t);
}
void b_div64RCX(GLOBL_ARGS) {
	NATIVE_OP(rcx, / , size_t);
}
void b_div64RDX(GLOBL_ARGS) {
	NATIVE_OP(rdx, / , size_t);
}


void b_add16AX(GLOBL_ARGS) {
	NATIVE_OP(ax, +, unsigned short);
}
void b_add16BX(GLOBL_ARGS) {
	NATIVE_OP(bx, +, unsigned short);
}
void b_add16CX(GLOBL_ARGS) {
	NATIVE_OP(cx, +, unsigned short);
}
void b_add16DX(GLOBL_ARGS) {
	NATIVE_OP(dx, +, unsigned short);
}

void b_add32EAX(GLOBL_ARGS) {
	NATIVE_OP(eax, +, unsigned int);
}
void b_add32EBX(GLOBL_ARGS) {
	NATIVE_OP(ebx, +, unsigned int);
}
void b_add32ECX(GLOBL_ARGS) {
	NATIVE_OP(ecx, +, unsigned int);
}
void b_add32EDX(GLOBL_ARGS) {
	NATIVE_OP(edx, +, unsigned int);
}

void b_add64RAX(GLOBL_ARGS) {
	NATIVE_OP(rax, +, size_t);
}
void b_add64RBX(GLOBL_ARGS) {
	NATIVE_OP(rbx, +, size_t);
}
void b_add64RCX(GLOBL_ARGS) {
	NATIVE_OP(rcx, +, size_t);
}
void b_add64RDX(GLOBL_ARGS) {
	NATIVE_OP(rdx, +, size_t);
}

void b_add64RBP(GLOBL_ARGS) {
	NATIVE_OP(rbp, +, size_t);
}
void b_add64RSP(GLOBL_ARGS) {
	NATIVE_OP(rsp, +, size_t);
}


void b_sub16AX(GLOBL_ARGS) {
	NATIVE_OP(ax, -, unsigned short);
}
void b_sub16BX(GLOBL_ARGS) {
	NATIVE_OP(bx, -, unsigned short);
}
void b_sub16CX(GLOBL_ARGS) {
	NATIVE_OP(cx, -, unsigned short);
}
void b_sub16DX(GLOBL_ARGS) {
	NATIVE_OP(dx, -, unsigned short);
}

void b_sub32EAX(GLOBL_ARGS) {
	NATIVE_OP(eax, -, unsigned int);
}
void b_sub32EBX(GLOBL_ARGS) {
	NATIVE_OP(ebx, -, unsigned int);
}
void b_sub32ECX(GLOBL_ARGS) {
	NATIVE_OP(ecx, -, unsigned int);
}
void b_sub32EDX(GLOBL_ARGS) {
	NATIVE_OP(edx, -, unsigned int);
}

void b_sub64RAX(GLOBL_ARGS) {
	NATIVE_OP(rax, -, size_t);
}
void b_sub64RBX(GLOBL_ARGS) {
	NATIVE_OP(rbx, -, size_t);
}
void b_sub64RCX(GLOBL_ARGS) {
	NATIVE_OP(rcx, -, size_t);
}
void b_sub64RDX(GLOBL_ARGS) {
	NATIVE_OP(rdx, -, size_t);
}

void b_sub64RBP(GLOBL_ARGS) {
	NATIVE_OP(rbp, -, size_t);
}
void b_sub64RSP(GLOBL_ARGS) {
	NATIVE_OP(rsp, -, size_t);
}