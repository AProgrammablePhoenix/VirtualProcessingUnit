#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>

#include "../utility.h"
#include "../Memory/memory_decl.h"
#include "../Memory/memory_symbols.h"
#include "regs_decl.h"
#include "registers_symbols.h"

#ifndef REGS_PREPROC
	#define GLOBL_ARGS CUSTOM_STD_ARGS(a, registers, mem)

	#define FPR_DEF(io_type, fpr_id) 										\
		void b_##io_type##FPR##fpr_id(GLOBL_ARGS) { 						\
			bool exitcode = false; 											\
			float f = (float)b_fetch_dbl_args(a, mem, exitcode); \
			if (!exitcode) { 												\
				*registers->fpr##fpr_id = f; 								\
			}																\
		}
	#define EFPR_DEF(io_type, efpr_id) 											\
		void b_##io_type##EFPR##efpr_id(GLOBL_ARGS) { 							\
			bool exitcode = false;												\
			double d = (double)b_fetch_dbl_args(a, mem, exitcode);	\
			if (!exitcode) {													\
				*registers->efpr##efpr_id = d; 									\
			}																	\
		}
	#define RFPR_DEF(io_type, rfpr_id) 											\
		void b_##io_type##RFPR##rfpr_id(GLOBL_ARGS) { 							\
			bool exitcode = false; 												\
			long double ld = b_fetch_dbl_args(a, mem, exitcode);		\
			if (!exitcode) {													\
				*registers->rfpr##rfpr_id = ld; 								\
			} 																	\
		}	
	#define DEF_ALL_FPRs() 	\
		FPR_DEF(set, 0) 	\
		FPR_DEF(set, 1) 	\
		FPR_DEF(set, 2) 	\
		FPR_DEF(set, 3)
	#define DEF_ALL_EFPRs() \
		EFPR_DEF(set, 0) 	\
		EFPR_DEF(set, 1) 	\
		EFPR_DEF(set, 2) 	\
		EFPR_DEF(set, 3)
	#define DEF_ALL_RFPRs() \
		RFPR_DEF(set, 0) 	\
		RFPR_DEF(set, 1)	\
		RFPR_DEF(set, 2) 	\
		RFPR_DEF(set, 3)
	#define DEF_ALL_FPRegs 	\
		DEF_ALL_FPRs() 		\
		DEF_ALL_EFPRs()		\
		DEF_ALL_RFPRs() 

	#define REGS_PREPROC(...) (void)0
#endif

void b_set16AX(GLOBL_ARGS) {
	b_set_num<unsigned short>(a, registers, mem, comn_registers::AX);
}
void b_set16BX(GLOBL_ARGS) {
	b_set_num<unsigned short>(a, registers, mem, comn_registers::BX);
}
void b_set16CX(GLOBL_ARGS) {
	b_set_num<unsigned short>(a, registers, mem, comn_registers::CX);
}
void b_set16DX(GLOBL_ARGS) {
	b_set_num<unsigned short>(a, registers, mem, comn_registers::DX);
}

void b_set32EAX(GLOBL_ARGS) {
	b_set_num<unsigned int>(a, registers, mem, comn_registers::EAX);
}
void b_set32EBX(GLOBL_ARGS) {
	b_set_num<unsigned int>(a, registers, mem, comn_registers::EBX);
}
void b_set32ECX(GLOBL_ARGS) {
	b_set_num<unsigned int>(a, registers, mem, comn_registers::ECX);
}
void b_set32EDX(GLOBL_ARGS) {
	b_set_num<unsigned int>(a, registers, mem, comn_registers::EDX);
}

void b_set64RAX(GLOBL_ARGS) {
	b_set_num<size_t>(a, registers, mem, comn_registers::RAX);
}
void b_set64RBX(GLOBL_ARGS) {
	b_set_num<size_t>(a, registers, mem, comn_registers::RBX);
}
void b_set64RCX(GLOBL_ARGS) {
	b_set_num<size_t>(a, registers, mem, comn_registers::RCX);
}
void b_set64RDX(GLOBL_ARGS) {
	b_set_num<size_t>(a, registers, mem, comn_registers::RDX);
}

void b_setGP(GLOBL_ARGS) {
	const comn_registers& reg_id = (comn_registers)std::get<2>(*std::static_pointer_cast<arg_tuple>(a)).raw_byte;
	if (comn_registers_table::is_num_reg(reg_id))
		b_set_num<size_t>(a, registers, mem, reg_id);
	else if (reg_id == comn_registers::SR)
		b_set_str(a, registers, mem);
	else if (reg_id == comn_registers::CR)
		b_set_chr(a, registers, mem);
	else if (comn_registers_table::is_fp_reg(reg_id)) {
		void* regptr = comn_registers_table(registers).access(reg_id);
		bool exitcode;

		if (dynamic_cast<OrphanReg<float>*>((reg_int<float>*)regptr)) {
			float f = (float)b_fetch_dbl_args(a, mem, exitcode);
			if (!exitcode)
				((OrphanReg<float>*)regptr)->set(f);
		}
		else if (dynamic_cast<OrphanReg<double>*>((reg_int<double>*)regptr)) {
			double d = (double)b_fetch_dbl_args(a, mem, exitcode);
			if (!exitcode)
				((OrphanReg<double>*)regptr)->set(d);
		}
		else if (dynamic_cast<OrphanReg<long double>*>((reg_int<long double>*)regptr)) {
			long double ld = b_fetch_dbl_args(a, mem, exitcode);
			if (!exitcode)
				((OrphanReg<long double>*)regptr)->set(ld);
		}
	}
}

void b_setSR(GLOBL_ARGS) {
	b_set_str(a, registers, mem);
}
void b_setCR(GLOBL_ARGS) {
	b_set_chr(a, registers, mem);
}

DEF_ALL_FPRegs;