#include <cmath>
#include <iostream>
#include <memory>
#include <string>

#include "../utility.h"
#include "../Memory/memory_symbols.h"
#include "regs_decl.h"
#include "registers_symbols.h"

/* STACK status before calling:
*	...
*	With value in specified registers
*	Output in specified register
*/
void b_not(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	((reg_int<size_t>*)ptr_table.access(reg_id))->set(~(((reg_int<size_t>*)ptr_table.access(reg_id)))->get());
}
void b_log2(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	((reg_int<size_t>*)ptr_table.access(reg_id))->set(
		(size_t)log2l((long double)((((reg_int<size_t>*)ptr_table.access(reg_id)))->get())));
}
void b_log10(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	((reg_int<size_t>*)ptr_table.access(reg_id))->set(
		(size_t)log10l((long double)((((reg_int<size_t>*)ptr_table.access(reg_id)))->get())));
}
void b_log(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	((reg_int<size_t>*)ptr_table.access(reg_id))->set(
		(size_t)logbl((long double)((((reg_int<size_t>*)ptr_table.access(reg_id)))->get())));
}

/*
* [Same as above, but for DR registry (double manipulations)]
*/
void b_dlog2(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m) {
	registers->dr->set(log2(registers->dr->get()));
}
void b_dlog10(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m) {
	registers->dr->set(log10(registers->dr->get()));
}
void b_dlog(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m) {
	registers->dr->set(logb(registers->dr->get()));
}

/* STACK status before calling:
*	...	LEFT VALUE
*	With RIGHT value in specified register
*	Output in specified register
*/
void b_and(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);

	unsigned char* temp = new unsigned char[sizeof(size_t)];
	mem->pop(temp, sizeof(size_t));
	size_t left = 0;
	mp_memcpy(temp, left);
	delete[] temp;

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	((reg_int<size_t>*)ptr_table.access(reg_id))->set(left & (((reg_int<size_t>*)ptr_table.access(reg_id))->get()));
}
void b_or(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);

	unsigned char* temp = new unsigned char[sizeof(size_t)];
	mem->pop(temp, sizeof(size_t));
	size_t left = 0;
	mp_memcpy(temp, left);
	delete[] temp;

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	((reg_int<size_t>*)ptr_table.access(reg_id))->set(left | (((reg_int<size_t>*)ptr_table.access(reg_id))->get()));
}
void b_xor(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);

	unsigned char* temp = new unsigned char[sizeof(size_t)];
	mem->pop(temp, sizeof(size_t));
	size_t left = 0;
	mp_memcpy(temp, left);
	delete[] temp;

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	((reg_int<size_t>*)ptr_table.access(reg_id))->set(left ^ (((reg_int<size_t>*)ptr_table.access(reg_id))->get()));
}
void b_shl(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);

	unsigned char* temp = new unsigned char[sizeof(size_t)];
	mem->pop(temp, sizeof(size_t));
	size_t left = 0;
	mp_memcpy(temp, left);
	delete[] temp;

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	((reg_int<size_t>*)ptr_table.access(reg_id))->set(left << (((reg_int<size_t>*)ptr_table.access(reg_id))->get()));
}
void b_shr(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);

	unsigned char* temp = new unsigned char[sizeof(size_t)];
	mem->pop(temp, sizeof(size_t));
	size_t left = 0;
	mp_memcpy(temp, left);
	delete[] temp;

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	((reg_int<size_t>*)ptr_table.access(reg_id))->set(left >> (((reg_int<size_t>*)ptr_table.access(reg_id))->get()));
}

void b_pow(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);

	unsigned char* temp = new unsigned char[sizeof(size_t)];
	mem->pop(temp, sizeof(size_t));
	size_t _pow = 0;
	mp_memcpy(temp, _pow);
	delete[] temp;

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	((reg_int<size_t>*)ptr_table.access(reg_id))->set(
		(size_t)powl((long double)(((reg_int<size_t>*)ptr_table.access(reg_id))->get()), (long double)_pow));
}

/*
* [Same as above, but for DR registry (double manipulations) ; Output in stack]
*/
void b_dpow(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	double saved_dr = registers->dr->get();
	popMemDR(nullptr, registers, mem);
	double _pow = registers->dr->get();

	double res = pow(saved_dr, _pow);
	registers->dr->set(res);
	pushMemDR(nullptr, registers, mem);

	registers->dr->set(saved_dr);
}