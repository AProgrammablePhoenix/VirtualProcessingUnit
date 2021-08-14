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
* [Same as above, but for FP registers (FP manipulations)]
*/
void b_dlog2(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	void* regptr = extra_registries_ptr_table(registers).access(ATTOXREGID(reg, mem));
	if (dynamic_cast<OrphanReg<float>*>((reg_int<float>*)regptr))
		*((OrphanReg<float>*)regptr) = log2(*((OrphanReg<float>*)regptr));
	else if (dynamic_cast<OrphanReg<double>*>((reg_int<double>*)regptr))
		*((OrphanReg<double>*)regptr) = log2(*((OrphanReg<double>*)regptr));
	else if (dynamic_cast<OrphanReg<long double>*>((reg_int<long double>*)regptr))
		*((OrphanReg<long double>*)regptr) = log2(*((OrphanReg<long double>*)regptr));
}
void b_dlog10(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	void* regptr = extra_registries_ptr_table(registers).access(ATTOXREGID(reg, mem));
	if (dynamic_cast<OrphanReg<float>*>((reg_int<float>*)regptr))
		*((OrphanReg<float>*)regptr) = log10(*((OrphanReg<float>*)regptr));
	else if (dynamic_cast<OrphanReg<double>*>((reg_int<double>*)regptr))
		*((OrphanReg<double>*)regptr) = log10(*((OrphanReg<double>*)regptr));
	else if (dynamic_cast<OrphanReg<long double>*>((reg_int<long double>*)regptr))
		*((OrphanReg<long double>*)regptr) = log10(*((OrphanReg<long double>*)regptr));
}
void b_dlog(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	void* regptr = extra_registries_ptr_table(registers).access(ATTOXREGID(reg, mem));
	if (dynamic_cast<OrphanReg<float>*>((reg_int<float>*)regptr))
		*((OrphanReg<float>*)regptr) = logb(*((OrphanReg<float>*)regptr));
	else if (dynamic_cast<OrphanReg<double>*>((reg_int<double>*)regptr))
		*((OrphanReg<double>*)regptr) = logb(*((OrphanReg<double>*)regptr));
	else if (dynamic_cast<OrphanReg<long double>*>((reg_int<long double>*)regptr))
		*((OrphanReg<long double>*)regptr) = logb(*((OrphanReg<long double>*)regptr));
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
	mp_memcpy(temp, &left);
	delete[] temp;

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	((reg_int<size_t>*)ptr_table.access(reg_id))->set(left & (((reg_int<size_t>*)ptr_table.access(reg_id))->get()));
}
void b_or(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);

	unsigned char* temp = new unsigned char[sizeof(size_t)];
	mem->pop(temp, sizeof(size_t));
	size_t left = 0;
	mp_memcpy(temp, &left);
	delete[] temp;

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	((reg_int<size_t>*)ptr_table.access(reg_id))->set(left | (((reg_int<size_t>*)ptr_table.access(reg_id))->get()));
}
void b_xor(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);

	unsigned char* temp = new unsigned char[sizeof(size_t)];
	mem->pop(temp, sizeof(size_t));
	size_t left = 0;
	mp_memcpy(temp, &left);
	delete[] temp;

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	((reg_int<size_t>*)ptr_table.access(reg_id))->set(left ^ (((reg_int<size_t>*)ptr_table.access(reg_id))->get()));
}
void b_shl(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);

	unsigned char* temp = new unsigned char[sizeof(size_t)];
	mem->pop(temp, sizeof(size_t));
	size_t left = 0;
	mp_memcpy(temp, &left);
	delete[] temp;

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	((reg_int<size_t>*)ptr_table.access(reg_id))->set(left << (((reg_int<size_t>*)ptr_table.access(reg_id))->get()));
}
void b_shr(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);

	unsigned char* temp = new unsigned char[sizeof(size_t)];
	mem->pop(temp, sizeof(size_t));
	size_t left = 0;
	mp_memcpy(temp, &left);
	delete[] temp;

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	((reg_int<size_t>*)ptr_table.access(reg_id))->set(left >> (((reg_int<size_t>*)ptr_table.access(reg_id))->get()));
}

void b_pow(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);

	unsigned char* temp = new unsigned char[sizeof(size_t)];
	mem->pop(temp, sizeof(size_t));
	size_t _pow = 0;
	mp_memcpy(temp, &_pow);
	delete[] temp;

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	((reg_int<size_t>*)ptr_table.access(reg_id))->set(
		(size_t)powl((long double)(((reg_int<size_t>*)ptr_table.access(reg_id))->get()), (long double)_pow));
}

/*
* [Same as above, but for FP registers (FP manipulations) ; Output in specified register]
*/
void b_dpow(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	extra_registries xreg_id = ATTOXREGID(reg, mem);

	void* regptr = extra_registries_ptr_table(registers).access(xreg_id);
	if (!is_reg_fpreg(xreg_id))
		return;

	auto uc_ld = std::make_unique<unsigned char[]>(sizeof(long double));

	long double _pow = 0;
	mem->pop(uc_ld.get(), sizeof(long double));
	mp_memcpy(uc_ld.get(), &_pow, sizeof(long long));
	

	if (dynamic_cast<OrphanReg<float>*>((reg_int<float>*)regptr)) {
		float& regv = ((OrphanReg<float>*)regptr)->data();
		regv = powf(regv, (float)regv);
	}
	else if (dynamic_cast<OrphanReg<double>*>((reg_int<double>*)regptr)) {
		double& regv = ((OrphanReg<double>*)regptr)->data();
		regv = pow(regv, (double)_pow);
	}
	else {
		long double& regv = ((OrphanReg<long double>*)regptr)->data();
		regv = powl(regv, _pow);
	}
}