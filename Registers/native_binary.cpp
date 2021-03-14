#include <iostream>
#include <string>

#include "regs_decl.h"
#include "registers_symbols.h"
#include "../Memory/memory_symbols.h"

void b_not(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	((reg_int<unsigned long long>*)ptr_table.access(reg))->set(~(((reg_int<unsigned long long>*)ptr_table.access(reg)))->get());
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