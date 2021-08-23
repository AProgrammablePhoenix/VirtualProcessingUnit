#include <iostream>
#include <memory>
#include <string>

#include "../utility.h"
#include "regs_decl.h"
#include "registers_symbols.h"

#define GLOBL_ARGS CUSTOM_STD_ARGS(reg, registers, mem)

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