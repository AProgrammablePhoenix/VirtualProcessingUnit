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
void b_not(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	((reg_int<unsigned long long>*)ptr_table.access(reg_id))->set(~(((reg_int<unsigned long long>*)ptr_table.access(reg_id)))->get());
}
void b_log2(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	((reg_int<unsigned long long>*)ptr_table.access(reg_id))->set(log2l((((reg_int<unsigned long long>*)ptr_table.access(reg_id)))->get()));
}
void b_log10(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	((reg_int<unsigned long long>*)ptr_table.access(reg_id))->set(log10l((((reg_int<unsigned long long>*)ptr_table.access(reg_id)))->get()));
}
void b_log(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	((reg_int<unsigned long long>*)ptr_table.access(reg_id))->set(logbl((((reg_int<unsigned long long>*)ptr_table.access(reg_id)))->get()));
}

/* STACK status before calling:
*	...	LEFT VALUE
*	With RIGHT value in specified register
*	Output in specified register
*/
void b_and(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	unsigned long long saved_rax = registers->rax->get();
	popMem(std::make_shared<registries_def>(registries_def::RAX), registers, mem);
	unsigned long long left = registers->rax->get();
	registers->rax->set(saved_rax);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	((reg_int<unsigned long long>*)ptr_table.access(reg_id))->set(left & (((reg_int<unsigned long long>*)ptr_table.access(reg_id))->get()));
}
void b_or(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	unsigned long long saved_rax = registers->rax->get();
	popMem(std::make_shared<registries_def>(registries_def::RAX), registers, mem);
	unsigned long long left = registers->rax->get();
	registers->rax->set(saved_rax);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	((reg_int<unsigned long long>*)ptr_table.access(reg_id))->set(left | (((reg_int<unsigned long long>*)ptr_table.access(reg_id))->get()));
}
void b_xor(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	unsigned long long saved_rax = registers->rax->get();
	popMem(std::make_shared<registries_def>(registries_def::RAX), registers, mem);
	unsigned long long left = registers->rax->get();
	registers->rax->set(saved_rax);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	((reg_int<unsigned long long>*)ptr_table.access(reg_id))->set(left ^ (((reg_int<unsigned long long>*)ptr_table.access(reg_id))->get()));
}
void b_shl(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	unsigned long long saved_rax = registers->rax->get();
	popMem(std::make_shared<registries_def>(registries_def::RAX), registers, mem);
	unsigned long long left = registers->rax->get();
	registers->rax->set(saved_rax);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	((reg_int<unsigned long long>*)ptr_table.access(reg_id))->set(left << (((reg_int<unsigned long long>*)ptr_table.access(reg_id))->get()));
}
void b_shr(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	unsigned long long saved_rax = registers->rax->get();
	popMem(std::make_shared<registries_def>(registries_def::RAX), registers, mem);
	unsigned long long left = registers->rax->get();
	registers->rax->set(saved_rax);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	((reg_int<unsigned long long>*)ptr_table.access(reg_id))->set(left >> (((reg_int<unsigned long long>*)ptr_table.access(reg_id))->get()));
}

void b_pow(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	unsigned long long saved_rax = registers->rax->get();
	popMem(std::make_shared<registries_def>(registries_def::RAX), registers, mem);
	unsigned long long pow = registers->rax->get();
	registers->rax->set(saved_rax);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	((reg_int<unsigned long long>*)ptr_table.access(reg_id))->set(powl((((reg_int<unsigned long long>*)ptr_table.access(reg_id))->get()), pow));
}