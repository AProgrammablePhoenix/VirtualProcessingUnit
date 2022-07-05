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

#define GLOBL_ARGS CUSTOM_STD_ARGS(a, registers, mem)

void b_setGP(GLOBL_ARGS) {
	const comn_registers& reg_id = (comn_registers)std::get<2>(*std::static_pointer_cast<arg_tuple>(a)).raw_byte;
	if (comn_registers_table::is_num_reg(reg_id))
		b_set_num<size_t>(a, registers, mem, reg_id);
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