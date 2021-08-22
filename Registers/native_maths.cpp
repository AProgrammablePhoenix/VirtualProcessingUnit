#include <iostream>
#include <memory>
#include <string>

#include "../utility.h"
#include "regs_decl.h"
#include "registers_symbols.h"

#ifndef NMATHS_PREPROC
	#define GLOBL_ARGS CUSTOM_STD_ARGS(reg, registers, mem)
	#define GLOBL_ARGS_D1 CUSTOM_STD_ARGS(unused_p, registers, mem)
	
	#define NATIVE_INCDEC(opsign)																									\
		comn_registers reg_id = ATTOREGID(reg, mem);																				\
		if (!comn_registers_table::is_num_reg(reg_id)) {																			\
			return;																													\
		}																															\
		comn_registers_table ptr_table = comn_registers_table(registers);															\
		((reg_int<size_t>*)ptr_table.access(reg_id))->set(((reg_int<size_t>*)ptr_table.access(reg_id))->get() opsign 1)

	#define FP_INCDEC(opsign)																						\
		comn_registers reg_id = ATTOREGID(reg, mem);																\
		if (comn_registers_table::is_fp_reg(reg_id)) {																\
			void* regptr = comn_registers_table(registers).access(reg_id);									\
			if (dynamic_cast<OrphanReg<float>*>((reg_int<float>*)regptr)) {											\
				*((OrphanReg<float>*)regptr) = (*((OrphanReg<float>*)regptr)) opsign 1;								\
			}																										\
			else if (dynamic_cast<OrphanReg<double>*>((reg_int<double>*)regptr)) {									\
				*((OrphanReg<double>*)regptr) = (*((OrphanReg<double>*)regptr)) opsign 1;							\
			}																										\
			else {																									\
				*((OrphanReg<long double>*)regptr)  = (*((OrphanReg<long double>*)regptr)) opsign 1;				\
			}																										\
		}
		

	#define NATIVE_OP(opreg, opsign, opsize)										\
		comn_registers reg_id = ATTOREGID(reg, mem);								\
		comn_registers_table ptr_table = comn_registers_table(registers);			\
		opsize value = ((reg_int<opsize>*)ptr_table.access(reg_id))->get();	\
		registers->opreg->set(registers->opreg->get() opsign value);

	#define FP_OP_SUB(opreg, opsign, dttype, basetype)										\
		basetype value = (basetype)((OrphanReg<dttype>*)ptr)->get();						\
		registers->opreg->set(registers->opreg->get() opsign value)

	#define FP_OP(opreg, opsign, dttype)														\
		comn_registers reg_id = ATTOREGID(reg, mem);											\
		void* ptr = comn_registers_table(registers).access(reg_id);						\
		if (comn_registers_table::is_fp_reg(reg_id)) {											\
			if (dynamic_cast<OrphanReg<float>*>((reg_int<float>*)ptr)) {						\
				FP_OP_SUB(opreg, opsign, float, dttype);										\
			}																					\
			else if (dynamic_cast<OrphanReg<double>*>((reg_int<double>*)ptr)) {					\
				FP_OP_SUB(opreg, opsign, double, dttype);										\
			}																					\
			else if (dynamic_cast<OrphanReg<long double>*>((reg_int<long double>*)ptr)) {		\
				FP_OP_SUB(opreg, opsign, long double, dttype);									\
			}																					\
		}
	
	#define DEF_FP_OP(opsuffix, regsuffix, opreg, opsign, dttype)	\
		void b_##opsuffix##regsuffix(GLOBL_ARGS) {					\
			FP_OP(opreg, opsign, dttype);							\
		}

	#define DEF_ALL_FP_OP(opsuffix, opsign)						\
		DEF_FP_OP(opsuffix, FPR0, fpr0, opsign, float)			\
		DEF_FP_OP(opsuffix, FPR1, fpr1, opsign, float)			\
		DEF_FP_OP(opsuffix, FPR2, fpr2, opsign, float)			\
		DEF_FP_OP(opsuffix, FPR3, fpr3, opsign, float)			\
		DEF_FP_OP(opsuffix, EFPR0, efpr0, opsign, double)		\
		DEF_FP_OP(opsuffix, EFPR1, efpr1, opsign, double)		\
		DEF_FP_OP(opsuffix, EFPR2, efpr2, opsign, double)		\
		DEF_FP_OP(opsuffix, EFPR3, efpr3, opsign, double)		\
		DEF_FP_OP(opsuffix, RFPR0, rfpr0, opsign, long double)	\
		DEF_FP_OP(opsuffix, RFPR1, rfpr1, opsign, long double)	\
		DEF_FP_OP(opsuffix, RFPR2, rfpr2, opsign, long double)	\
		DEF_FP_OP(opsuffix, RFPR3, rfpr3, opsign, long double)

	#define DEF_ALL_FP_MUL DEF_ALL_FP_OP(mul, *)
	#define DEF_ALL_FP_DIV DEF_ALL_FP_OP(div, /)
	#define DEF_ALL_FP_ADD DEF_ALL_FP_OP(add, +)
	#define DEF_ALL_FP_SUB DEF_ALL_FP_OP(sub, -)

	#define NMATH_PREPROC(...) (void)0
#endif

void b_inc(GLOBL_ARGS) {
	NATIVE_INCDEC(+);
}
void b_dec(GLOBL_ARGS) {
	NATIVE_INCDEC(-);
}

void b_incFP(GLOBL_ARGS) {
	FP_INCDEC(+);
}
void b_decFP(GLOBL_ARGS) {
	FP_INCDEC(-);
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

DEF_ALL_FP_MUL
DEF_ALL_FP_DIV
DEF_ALL_FP_ADD
DEF_ALL_FP_SUB


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