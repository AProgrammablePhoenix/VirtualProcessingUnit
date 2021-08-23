#include <iostream>
#include <memory>
#include <string>

#include "../utility.h"
#include "regs_decl.h"
#include "registers_symbols.h"

#ifndef NMATHS_PREPROC
	#define GLOBL_ARGS CUSTOM_STD_ARGS(reg, registers, mem)
	
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

void b_mulGP(GLOBL_ARGS) {
	const comn_registers& dest_id = (comn_registers)std::get<2>(*std::static_pointer_cast<arg_tuple>(reg)).raw_byte;
	if (comn_registers_table::is_num_reg(dest_id)) {
		comn_registers src_id = ATTOREGID(reg, mem);
		if (!comn_registers_table::is_num_reg(src_id))
			return;

		comn_registers_table ptr_table = comn_registers_table(registers);
		void* dest_ptr = ptr_table.access(dest_id);
		size_t multiplier = ((reg_int<size_t>*)ptr_table.access(src_id))->get();

		((reg_int<size_t>*)dest_ptr)->set(((reg_int<size_t>*)dest_ptr)->get() * multiplier);
	}
	else if (comn_registers_table::is_fp_reg(dest_id)) {
		comn_registers src_id = ATTOREGID(reg, mem);
		if (!comn_registers_table::is_fp_reg(src_id))
			return;

		void* src_ptr = comn_registers_table(registers).access(src_id);
		void* dest_ptr = comn_registers_table(registers).access(dest_id);

		long double ld = 0;
		if (dynamic_cast<OrphanReg<float>*>((reg_int<float>*)src_ptr))
			ld = (long double)((OrphanReg<float>*)src_ptr)->get();
		else if (dynamic_cast<OrphanReg<double>*>((reg_int<double>*)src_ptr))
			ld = (long double)((OrphanReg<double>*)src_ptr)->get();
		else
			ld = ((OrphanReg<long double>*)src_ptr)->get();

		if (dynamic_cast<OrphanReg<float>*>((reg_int<float>*)dest_ptr))
			*((OrphanReg<float>*)dest_ptr) *= (float)ld;
		else if (dynamic_cast<OrphanReg<double>*>((reg_int<double>*)src_ptr))
			*((OrphanReg<double>*)dest_ptr) *= (double)ld;
		else
			*((OrphanReg<long double>*)dest_ptr) *= ld;
	}
}
void b_divGP(GLOBL_ARGS) {
	const comn_registers& dest_id = (comn_registers)std::get<2>(*std::static_pointer_cast<arg_tuple>(reg)).raw_byte;
	if (comn_registers_table::is_num_reg(dest_id)) {
		comn_registers src_id = ATTOREGID(reg, mem);
		if (!comn_registers_table::is_num_reg(src_id))
			return;

		comn_registers_table ptr_table = comn_registers_table(registers);
		void* dest_ptr = ptr_table.access(dest_id);
		size_t multiplier = ((reg_int<size_t>*)ptr_table.access(src_id))->get();

		((reg_int<size_t>*)dest_ptr)->set(((reg_int<size_t>*)dest_ptr)->get() / multiplier);
	}
	else if (comn_registers_table::is_fp_reg(dest_id)) {
		comn_registers src_id = ATTOREGID(reg, mem);
		if (!comn_registers_table::is_fp_reg(src_id))
			return;

		void* src_ptr = comn_registers_table(registers).access(src_id);
		void* dest_ptr = comn_registers_table(registers).access(dest_id);

		long double ld = 0;
		if (dynamic_cast<OrphanReg<float>*>((reg_int<float>*)src_ptr))
			ld = (long double)((OrphanReg<float>*)src_ptr)->get();
		else if (dynamic_cast<OrphanReg<double>*>((reg_int<double>*)src_ptr))
			ld = (long double)((OrphanReg<double>*)src_ptr)->get();
		else
			ld = ((OrphanReg<long double>*)src_ptr)->get();

		if (dynamic_cast<OrphanReg<float>*>((reg_int<float>*)dest_ptr))
			*((OrphanReg<float>*)dest_ptr) /= (float)ld;
		else if (dynamic_cast<OrphanReg<double>*>((reg_int<double>*)src_ptr))
			*((OrphanReg<double>*)dest_ptr) /= (double)ld;
		else
			*((OrphanReg<long double>*)dest_ptr) /= ld;
	}
}
void b_addGP(GLOBL_ARGS) {
	const comn_registers& dest_id = (comn_registers)std::get<2>(*std::static_pointer_cast<arg_tuple>(reg)).raw_byte;
	if (comn_registers_table::is_num_reg(dest_id)) {
		comn_registers src_id = ATTOREGID(reg, mem);
		if (!comn_registers_table::is_num_reg(src_id))
			return;

		comn_registers_table ptr_table = comn_registers_table(registers);
		void* dest_ptr = ptr_table.access(dest_id);
		size_t multiplier = ((reg_int<size_t>*)ptr_table.access(src_id))->get();

		((reg_int<size_t>*)dest_ptr)->set(((reg_int<size_t>*)dest_ptr)->get() + multiplier);
	}
	else if (comn_registers_table::is_fp_reg(dest_id)) {
		comn_registers src_id = ATTOREGID(reg, mem);
		if (!comn_registers_table::is_fp_reg(src_id))
			return;

		void* src_ptr = comn_registers_table(registers).access(src_id);
		void* dest_ptr = comn_registers_table(registers).access(dest_id);

		long double ld = 0;
		if (dynamic_cast<OrphanReg<float>*>((reg_int<float>*)src_ptr))
			ld = (long double)((OrphanReg<float>*)src_ptr)->get();
		else if (dynamic_cast<OrphanReg<double>*>((reg_int<double>*)src_ptr))
			ld = (long double)((OrphanReg<double>*)src_ptr)->get();
		else
			ld = ((OrphanReg<long double>*)src_ptr)->get();

		if (dynamic_cast<OrphanReg<float>*>((reg_int<float>*)dest_ptr))
			*((OrphanReg<float>*)dest_ptr) += (float)ld;
		else if (dynamic_cast<OrphanReg<double>*>((reg_int<double>*)src_ptr))
			*((OrphanReg<double>*)dest_ptr) += (double)ld;
		else
			*((OrphanReg<long double>*)dest_ptr) += ld;
	}
}
void b_subGP(GLOBL_ARGS) {
	const comn_registers& dest_id = (comn_registers)std::get<2>(*std::static_pointer_cast<arg_tuple>(reg)).raw_byte;
	if (comn_registers_table::is_num_reg(dest_id)) {
		comn_registers src_id = ATTOREGID(reg, mem);
		if (!comn_registers_table::is_num_reg(src_id))
			return;

		comn_registers_table ptr_table = comn_registers_table(registers);
		void* dest_ptr = ptr_table.access(dest_id);
		size_t multiplier = ((reg_int<size_t>*)ptr_table.access(src_id))->get();

		((reg_int<size_t>*)dest_ptr)->set(((reg_int<size_t>*)dest_ptr)->get() - multiplier);
	}
	else if (comn_registers_table::is_fp_reg(dest_id)) {
		comn_registers src_id = ATTOREGID(reg, mem);
		if (!comn_registers_table::is_fp_reg(src_id))
			return;

		void* src_ptr = comn_registers_table(registers).access(src_id);
		void* dest_ptr = comn_registers_table(registers).access(dest_id);

		long double ld = 0;
		if (dynamic_cast<OrphanReg<float>*>((reg_int<float>*)src_ptr))
			ld = (long double)((OrphanReg<float>*)src_ptr)->get();
		else if (dynamic_cast<OrphanReg<double>*>((reg_int<double>*)src_ptr))
			ld = (long double)((OrphanReg<double>*)src_ptr)->get();
		else
			ld = ((OrphanReg<long double>*)src_ptr)->get();

		if (dynamic_cast<OrphanReg<float>*>((reg_int<float>*)dest_ptr))
			*((OrphanReg<float>*)dest_ptr) -= (float)ld;
		else if (dynamic_cast<OrphanReg<double>*>((reg_int<double>*)src_ptr))
			*((OrphanReg<double>*)dest_ptr) -= (double)ld;
		else
			*((OrphanReg<long double>*)dest_ptr) -= ld;
	}
}