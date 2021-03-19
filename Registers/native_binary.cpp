#include <iostream>
#include <string>
#include <cmath>

#include "regs_decl.h"
#include "registers_symbols.h"
#include "../Memory/memory_symbols.h"

/* STACK status before calling:
*	...
*	With value in specified registers
*	Output in specified register
*/
void b_not(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	((reg_int<unsigned long long>*)ptr_table.access(reg))->set(~(((reg_int<unsigned long long>*)ptr_table.access(reg)))->get());
}
void b_log2(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	((reg_int<unsigned long long>*)ptr_table.access(reg))->set(log2l((((reg_int<unsigned long long>*)ptr_table.access(reg)))->get()));
}
void b_log10(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	((reg_int<unsigned long long>*)ptr_table.access(reg))->set(log10l((((reg_int<unsigned long long>*)ptr_table.access(reg)))->get()));
}
void b_log(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	((reg_int<unsigned long long>*)ptr_table.access(reg))->set(logbl((((reg_int<unsigned long long>*)ptr_table.access(reg)))->get()));
}

/* STACK status before calling:
*	...	LEFT VALUE
*	With RIGHT value in specified register
*	Output in specified register
*/
void b_and(registries_def reg, regs* registers, memory* mem) {
	unsigned long long saved_rax = registers->rax->get();
	popMem(registries_def::RAX, registers, mem);
	unsigned long long left = registers->rax->get();
	registers->rax->set(saved_rax);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	((reg_int<unsigned long long>*)ptr_table.access(reg))->set(left & (((reg_int<unsigned long long>*)ptr_table.access(reg))->get()));
}
void b_or(registries_def reg, regs* registers, memory* mem) {
	unsigned long long saved_rax = registers->rax->get();
	popMem(registries_def::RAX, registers, mem);
	unsigned long long left = registers->rax->get();
	registers->rax->set(saved_rax);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	((reg_int<unsigned long long>*)ptr_table.access(reg))->set(left | (((reg_int<unsigned long long>*)ptr_table.access(reg))->get()));
}
void b_xor(registries_def reg, regs* registers, memory* mem) {
	unsigned long long saved_rax = registers->rax->get();
	popMem(registries_def::RAX, registers, mem);
	unsigned long long left = registers->rax->get();
	registers->rax->set(saved_rax);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	((reg_int<unsigned long long>*)ptr_table.access(reg))->set(left ^ (((reg_int<unsigned long long>*)ptr_table.access(reg))->get()));
}
void b_shl(registries_def reg, regs* registers, memory* mem) {
	unsigned long long saved_rax = registers->rax->get();
	popMem(registries_def::RAX, registers, mem);
	unsigned long long left = registers->rax->get();
	registers->rax->set(saved_rax);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	((reg_int<unsigned long long>*)ptr_table.access(reg))->set(left << (((reg_int<unsigned long long>*)ptr_table.access(reg))->get()));
}
void b_shr(registries_def reg, regs* registers, memory* mem) {
	unsigned long long saved_rax = registers->rax->get();
	popMem(registries_def::RAX, registers, mem);
	unsigned long long left = registers->rax->get();
	registers->rax->set(saved_rax);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	((reg_int<unsigned long long>*)ptr_table.access(reg))->set(left >> (((reg_int<unsigned long long>*)ptr_table.access(reg))->get()));
}

void b_pow(registries_def reg, regs* registers, memory* mem) {
	unsigned long long saved_rax = registers->rax->get();
	popMem(registries_def::RAX, registers, mem);
	unsigned long long pow = registers->rax->get();
	registers->rax->set(saved_rax);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	((reg_int<unsigned long long>*)ptr_table.access(reg))->set(powl((((reg_int<unsigned long long>*)ptr_table.access(reg))->get()), pow));
}