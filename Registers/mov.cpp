#include <iostream>
#include <memory>
#include <string>

#include "../utility.h"
#include "regs_decl.h"
#include "registers_symbols.h"

#ifndef MOV_PREPROC
	#define HIDDN_ARGS std::shared_ptr<void> reg_ptr, const regs* const registers
	#define GLOBL_ARGS CUSTOM_STD_ARGS(reg, registers, mem)

	#define HIDDN_BODY(reg, reg_ptr, opsize) \
		registers->reg->set(*std::static_pointer_cast<opsize>(reg_ptr))

	#define GLOBL_BODY(opname, opsize)																	\
		comn_registers reg_id = ATTOREGID(reg, mem);													\
		if (!comn_registers_table::is_num_reg(reg_id)) {													\
			return;																						\
		}																								\
		comn_registers_table ptr_table = comn_registers_table(registers);								\
		void* ptr = ptr_table.access(reg_id);																\
		std::shared_ptr<opsize> value = std::make_shared<opsize>(((reg_int<opsize>*)ptr)->get());		\
		c_b_mov##opname(value, registers)

	#define FP_GLOBL_SUB(regname, dttype, basetype) *registers->regname = (basetype)((OrphanReg<dttype>*)ptr)->get()
	
	#define FP_GLOBL_BODY(regname, dttype)																\
		comn_registers reg_id = ATTOREGID(reg, mem);													\
		if (comn_registers_table::is_fp_reg(reg_id)) {													\
			comn_registers_table ptr_table = comn_registers_table(registers);							\
			void* ptr = ptr_table.access(reg_id);														\
			if (dynamic_cast<OrphanReg<float>*>((reg_int<float>*)ptr)) {								\
				FP_GLOBL_SUB(regname, float, dttype);													\
			}																							\
			else if (dynamic_cast<OrphanReg<double>*>((reg_int<double>*)ptr)) {							\
				FP_GLOBL_SUB(regname, double, dttype);													\
			}																							\
			else if (dynamic_cast<OrphanReg<long double>*>((reg_int<long double>*)ptr)) {				\
				FP_GLOBL_SUB(regname, long double, dttype);												\
			}																							\
		}
	
	#define FP_MOV_DEF(func_suffix, regname, dttype)				\
		void b_mov##func_suffix(GLOBL_ARGS) {						\
			FP_GLOBL_BODY(regname, dttype)							\
		}
	
	#define MOV_PREPROC(...) (void)0
#endif

namespace {
	void c_b_mov16AX(HIDDN_ARGS) {
		HIDDN_BODY(ax, reg_ptr, unsigned short);
	}
	void c_b_mov16BX(HIDDN_ARGS) {
		HIDDN_BODY(bx, reg_ptr, unsigned short);
	}
	void c_b_mov16CX(HIDDN_ARGS) {
		HIDDN_BODY(cx, reg_ptr, unsigned short);
	}
	void c_b_mov16DX(HIDDN_ARGS) {
		HIDDN_BODY(dx, reg_ptr, unsigned short);
	}

	void c_b_mov32EAX(HIDDN_ARGS) {
		HIDDN_BODY(eax, reg_ptr, unsigned int);
	}
	void c_b_mov32EBX(HIDDN_ARGS) {
		HIDDN_BODY(ebx, reg_ptr, unsigned int);
	}
	void c_b_mov32ECX(HIDDN_ARGS) {
		HIDDN_BODY(ecx, reg_ptr, unsigned int);
	}
	void c_b_mov32EDX(HIDDN_ARGS) {
		HIDDN_BODY(edx, reg_ptr, unsigned int);
	}

	void c_b_mov64RAX(HIDDN_ARGS) {
		HIDDN_BODY(rax, reg_ptr, size_t);
	}
	void c_b_mov64RBX(HIDDN_ARGS) {
		HIDDN_BODY(rbx, reg_ptr, size_t);
	}
	void c_b_mov64RCX(HIDDN_ARGS) {
		HIDDN_BODY(rcx, reg_ptr, size_t);
	}
	void c_b_mov64RDX(HIDDN_ARGS) {
		HIDDN_BODY(rdx, reg_ptr, size_t);
	}

	void c_b_mov64RBP(HIDDN_ARGS) {
		HIDDN_BODY(rbp, reg_ptr, size_t);
	}
	void c_b_mov64RSP(HIDDN_ARGS) {
		HIDDN_BODY(rsp, reg_ptr, size_t);
	}
}

void b_mov16AX(GLOBL_ARGS) {
	GLOBL_BODY(16AX, unsigned short);
}
void b_mov16BX(GLOBL_ARGS) {
	GLOBL_BODY(16BX, unsigned short);
}
void b_mov16CX(GLOBL_ARGS) {
	GLOBL_BODY(16CX, unsigned short);
}
void b_mov16DX(GLOBL_ARGS) {
	GLOBL_BODY(16DX, unsigned short);
}

void b_mov32EAX(GLOBL_ARGS) {
	GLOBL_BODY(32EAX, unsigned int);
}
void b_mov32EBX(GLOBL_ARGS) {
	GLOBL_BODY(32EBX, unsigned int);
}
void b_mov32ECX(GLOBL_ARGS) {
	GLOBL_BODY(32ECX, unsigned int);
}
void b_mov32EDX(GLOBL_ARGS) {
	GLOBL_BODY(32EDX, unsigned int);
}

void b_mov64RAX(GLOBL_ARGS) {
	GLOBL_BODY(64RAX, size_t);
}
void b_mov64RBX(GLOBL_ARGS) {
	GLOBL_BODY(64RBX, size_t);
}
void b_mov64RCX(GLOBL_ARGS) {
	GLOBL_BODY(64RCX, size_t);
}
void b_mov64RDX(GLOBL_ARGS) {
	GLOBL_BODY(64RDX, size_t);
}

void b_mov64RBP(GLOBL_ARGS) {
	GLOBL_BODY(64RBP, size_t);
}
void b_mov64RSP(GLOBL_ARGS) {
	GLOBL_BODY(64RSP, size_t);
}

void b_movGP(GLOBL_ARGS) {
	const comn_registers& dest_reg = (comn_registers)std::get<2>(*std::static_pointer_cast<arg_tuple>(reg)).raw_byte;

	if (comn_registers_table::is_num_reg(dest_reg)) {
		comn_registers src_reg = ATTOREGID(reg, mem);
		if (!comn_registers_table::is_num_reg(src_reg))
			return;

		comn_registers_table ptr_table(registers);
		void *src_ptr = ptr_table.access(src_reg), *dest_ptr = ptr_table.access(dest_reg);
		((reg_int<size_t>*)dest_ptr)->set(((reg_int<size_t>*)src_ptr)->get());
	}
	else if (comn_registers_table::is_fp_reg(dest_reg)) {
		comn_registers src_reg = ATTOREGID(reg, mem);
		if (!comn_registers_table::is_fp_reg(src_reg))
			return;

		comn_registers_table ptr_table(registers);
		void *src_ptr = ptr_table.access(src_reg), *dest_ptr = ptr_table.access(dest_reg);

		long double ld = 0;
		if (dynamic_cast<OrphanReg<float>*>((reg_int<float>*)src_ptr))
			ld = (long double)((OrphanReg<float>*)src_ptr)->get();
		else if (dynamic_cast<OrphanReg<double>*>((reg_int<double>*)src_ptr))
			ld = (long double)((OrphanReg<double>*)src_ptr)->get();
		else
			ld = ((OrphanReg<long double>*)src_ptr)->get();

		if (dynamic_cast<OrphanReg<float>*>((reg_int<float>*)dest_ptr))
			*((OrphanReg<float>*)dest_ptr) = (float)ld;
		else if (dynamic_cast<OrphanReg<double>*>((reg_int<double>*)src_ptr))
			*((OrphanReg<double>*)dest_ptr) = (double)ld;
		else
			*((OrphanReg<long double>*)dest_ptr) = ld;
	}
}

FP_MOV_DEF(FPR0, fpr0, float)
FP_MOV_DEF(FPR1, fpr1, float)
FP_MOV_DEF(FPR2, fpr2, float)
FP_MOV_DEF(FPR3, fpr3, float)

FP_MOV_DEF(EFPR0, efpr0, double)
FP_MOV_DEF(EFPR1, efpr1, double)
FP_MOV_DEF(EFPR2, efpr2, double)
FP_MOV_DEF(EFPR3, efpr3, double)

FP_MOV_DEF(RFPR0, rfpr0, long double)
FP_MOV_DEF(RFPR1, rfpr1, long double)
FP_MOV_DEF(RFPR2, rfpr2, long double)
FP_MOV_DEF(RFPR3, rfpr3, long double)