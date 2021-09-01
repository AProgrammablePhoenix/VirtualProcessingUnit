#include <concepts>
#include <iostream>
#include <memory>
#include <string>
#include <tuple>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#define ISWIN 1
#endif

#if defined(__linux__)
#include <cstring>
#include <stdio.h>
#endif

#include "../utility.h"
#include "../Registers/regs_decl.h"
#include "../Registers/registers_symbols.h"
#include "memory_decl.h"

#ifndef MEMORY_PREPROC
	#define GLOBL_ARGS CUSTOM_STD_ARGS(reg, registers, mem)
	#define GLOBL_ARGS_D1 CUSTOM_STD_ARGS(unused_p, registers, mem)
	#define GLOBL_ARGS_D2 CUSTOM_STD_ARGS(reg_addr, registers, mem)

	#define MEMORY_PREPROC(...) (void)0
#endif

namespace {
	void FPR_push_helper(const long double& value, memory* const& mem) {
		unsigned char* uc_fp = new unsigned char[sizeof(long double)];
#if defined(ISWIN)
		memcpy_s(uc_fp, sizeof(long double), &value, sizeof(long double));
#else
		memcpy(uc_fp, &value, sizeof(long double));
#endif
		mem->push(uc_fp, sizeof(long double));
		delete[] uc_fp;
	}
	void FPR_pop_helper(memory* const& mem, long double& output) {
		unsigned char* uc_fp = new unsigned char[sizeof(long double)];
		mem->pop(uc_fp, sizeof(long double));

#if defined(ISWIN)
		memcpy_s(&output, sizeof(long double), uc_fp, sizeof(long double));
#else
		memcpy(&output, uc_fp, sizeof(long double));
#endif
		delete[] uc_fp;
	}

	void FPR_mem_set(const long double& value, const size_t& addr, memory* const& mem) {
		unsigned char* uc_fp = new unsigned char[sizeof(long double)];
#if defined(ISWIN)
		memcpy_s(uc_fp, sizeof(long double), &value, sizeof(long double));
#else
		memcpy(uc_fp, &value, sizeof(long double));
#endif
		mem->_MS(uc_fp, sizeof(long double), addr);
		delete[] uc_fp;
	}
	void FPR_mem_get(const size_t& addr, memory* const& mem, long double& output) {
		unsigned char* uc_fp = new unsigned char[sizeof(long double)];
		mem->_MG(uc_fp, sizeof(long double), addr);
#if defined(ISWIN)
		memcpy_s(&output, sizeof(long double), uc_fp, sizeof(long double));
#else
		memcpy(&output, uc_fp, sizeof(long double));
#endif
		delete[] uc_fp;
	}
}

void sdzsMem(GLOBL_ARGS) {
	if (!registers->exec_level) {
		comn_registers reg_id = ATTOREGID(reg, mem);
		if (!comn_registers_table::is_num_reg(reg_id))
			return;

		((reg_int<size_t>*)comn_registers_table(registers).access(reg_id))->set(mem->_SDZS());
	}
}

// Memory stack symbols
void pushMem(GLOBL_ARGS) {
	comn_registers reg_id = ATTOREGID(reg, mem);
	if (!comn_registers_table::is_num_reg(reg_id))
		return;

	nbyte vopt = std::get<2>(*std::static_pointer_cast<arg_tuple>(reg));
	void* regptr = comn_registers_table(registers).access(reg_id);
	size_t value = ((reg_int<size_t>*)regptr)->get();

	size_t temp_sz = 0;
	uint8_t qt_det = VOPTTOTQ(vopt);
	if (!qt_det)
		temp_sz = sizeof(size_t);
	else
		temp_sz = qt_det;

	size_t mask = 0;
	for (size_t i = 0; i < temp_sz; ++i)
		mask |= (size_t)0xff << (i * 8);

	value &= mask;

	auto temp = std::make_unique<uint8_t[]>(temp_sz);
	for (size_t j = 0, i = temp_sz; i > 0; --i, ++j)
		temp[j] = (value >> ((i - 1) * 8)) & 0xff;

	mem->push(temp.get(), temp_sz, false);
}
void popMem(GLOBL_ARGS) {
	comn_registers reg_id = ATTOREGID(reg, mem);
	if (!comn_registers_table::is_num_reg(reg_id))
		return;

	nbyte vopt = std::get<2>(*std::static_pointer_cast<arg_tuple>(reg));
	void* regptr = comn_registers_table(registers).access(reg_id);
	
	size_t temp_sz = 0;
	uint8_t qt_det = VOPTTOTQ(vopt);
	if (!qt_det)
		temp_sz = sizeof(size_t);
	else
		temp_sz = qt_det;

	auto temp = std::make_unique<uint8_t[]>(temp_sz);

	mem->pop(temp.get(), temp_sz);
	size_t value = 0;

	mp_memcpy(temp.get(), &value);
	((reg_int<size_t>*)regptr)->set(value);
}

void pushMemSR(GLOBL_ARGS_D1) {
	std::string value = registers->sr->get();

	size_t rssize = value.size() + 1;

	unsigned char* ssize = new unsigned char[sizeof(size_t)];
	unsigned char* uc_s = new unsigned char[rssize];
	mp_memcpy(&rssize, ssize);

#if defined(ISWIN)
	memcpy_s(uc_s, rssize, value.c_str(), rssize);
#else
	std::memcpy(uc_s, value.c_str(), value.size() + 1);
#endif

	mem->push(uc_s, rssize);
	mem->push(ssize, sizeof(size_t));

	delete[] uc_s;
	delete[] ssize;
}
void popMemSR(GLOBL_ARGS_D1) {
	unsigned char* ssize = new unsigned char[sizeof(size_t)];
	unsigned char* value = nullptr;

	mem->pop(ssize, sizeof(size_t));
	size_t rssize = 0;

	mp_memcpy(ssize, &rssize);
	value = new unsigned char[rssize];
	mem->pop(value, rssize);

	registers->sr->set(std::string((const char*)value));

	delete[] value;
	delete[] ssize;
}

void pushMemCR(GLOBL_ARGS_D1) {
	unsigned char* cptr = new unsigned char[1];
	cptr[0] = registers->cr->get();

	mem->push(cptr, 1);
	delete[] cptr;
}
void popMemCR(GLOBL_ARGS_D1) {
	unsigned char* cptr = new unsigned char[1];
	mem->pop(cptr, 1);

	registers->cr->set(cptr[0]);

	delete[] cptr;
}

void pushMemFPR(GLOBL_ARGS) {
	void* regptr = comn_registers_table(registers).access(ATTOREGID(reg, mem));
	if (dynamic_cast<OrphanReg<float>*>((reg_int<float>*)regptr))
		FPR_push_helper((long double)((OrphanReg<float>*)regptr)->get(),mem);
	else if (dynamic_cast<OrphanReg<double>*>((reg_int<double>*)regptr))
		FPR_push_helper((long double)((OrphanReg<double>*)regptr)->get(), mem);
	else if (dynamic_cast<OrphanReg<long double>*>((reg_int<long double>*)regptr))
		FPR_push_helper(((OrphanReg<long double>*)regptr)->get(), mem);
}
void popMemFPR(GLOBL_ARGS) {
	void* regptr = comn_registers_table(registers).access(ATTOREGID(reg, mem));
	long double ld = 0;

	if (dynamic_cast<OrphanReg<float>*>((reg_int<float>*)regptr)) {
		FPR_pop_helper(mem, ld);
		*((OrphanReg<float>*)regptr) = (float)ld;
	}
	else if (dynamic_cast<OrphanReg<double>*>((reg_int<double>*)regptr)) {
		FPR_pop_helper(mem, ld);
		*((OrphanReg<double>*)regptr) = (double)ld;
	}
	else if (dynamic_cast<OrphanReg<long double>*>((reg_int<long double>*)regptr))
		FPR_pop_helper(mem, ((OrphanReg<long double>*)regptr)->data());
}

void movsm(GLOBL_ARGS_D2) {
	nbyte vopt = std::get<2>(*std::static_pointer_cast<arg_tuple>(reg_addr));
	comn_registers _reg = ATTOREGID(reg_addr, mem);
	if (!comn_registers_table::is_num_reg(_reg))
		return;

	void* ptr = comn_registers_table(registers).access(_reg);

	size_t _addr = ((reg_int<size_t>*)ptr)->get();

	size_t temp_sz = 0;
	uint8_t qt_det = VOPTTOTQ(vopt);
	if (!qt_det)
		temp_sz = sizeof(size_t);
	else
		temp_sz = qt_det;

	auto temp = std::make_unique<uint8_t[]>(temp_sz);
	mem->pop(temp.get(), temp_sz);	
	mem->_MS(temp.get(), temp_sz, _addr);
}
void movgm(GLOBL_ARGS_D2) {
	nbyte vopt = std::get<2>(*std::static_pointer_cast<arg_tuple>(reg_addr));
	comn_registers _reg = ATTOREGID(reg_addr, mem);
	if (!comn_registers_table::is_num_reg(_reg))
		return;

	void* ptr = comn_registers_table(registers).access(_reg);

	size_t _addr = ((reg_int<size_t>*)ptr)->get();
	size_t temp_sz = 0;
	uint8_t qt_det = VOPTTOTQ(vopt);
	if (!qt_det)
		temp_sz = sizeof(size_t);
	else
		temp_sz = qt_det;

	auto temp = std::make_unique<uint8_t[]>(temp_sz);
	mem->_MG(temp.get(), temp_sz, _addr);
	mem->push(temp.get(), temp_sz);
}

void movsmSR(GLOBL_ARGS_D2) {
	comn_registers _reg = ATTOREGID(reg_addr, mem);
	if (!comn_registers_table::is_num_reg(_reg))
		return;

	void* ptr = comn_registers_table(registers).access(_reg);
	size_t _addr = ((reg_int<size_t>*)ptr)->get();

	std::string value = registers->sr->get();
	size_t rt_size = value.size() + 1;

	unsigned char* temp = new unsigned char[rt_size];
	unsigned char* t_size = new unsigned char[sizeof(size_t)];

	mp_memcpy(&rt_size, t_size, sizeof(size_t));
#if defined(ISWIN)
	memcpy_s(temp, rt_size, value.c_str(), rt_size);
#else
	std::memcpy(temp, value.c_str(), rt_size);
#endif
	mem->_MS(t_size, sizeof(size_t), _addr);
	mem->_MS(temp, rt_size, _addr + sizeof(size_t));
	delete[] t_size;
	delete[] temp;
}
void movgmSR(GLOBL_ARGS_D2) {
	comn_registers _reg = ATTOREGID(reg_addr, mem);
	if (!comn_registers_table::is_num_reg(_reg))
		return;

	void* ptr = comn_registers_table(registers).access(_reg);
	size_t _addr = ((reg_int<size_t>*)ptr)->get();
	size_t rt_size = 0;

	unsigned char* t_size = new unsigned char[sizeof(size_t)];
	mem->_MG(t_size, sizeof(size_t), _addr);
	mp_memcpy(t_size, &rt_size);
	delete[] t_size;

	unsigned char* temp = new unsigned char[rt_size];
	mem->_MG(temp, rt_size, _addr + sizeof(size_t));
	std::string value = std::string(reinterpret_cast<char*>(temp));
	delete[] temp;

	registers->sr->set(value);
}

void movsmCR(GLOBL_ARGS_D2) {
	comn_registers _reg = ATTOREGID(reg_addr, mem);
	if (!comn_registers_table::is_num_reg(_reg))
		return;

	void* ptr = comn_registers_table(registers).access(_reg);

	size_t _addr = ((reg_int<size_t>*)ptr)->get();
	unsigned char* temp = new unsigned char[1];
	temp[0] = registers->cr->get();
	mem->_MS(temp, 1, _addr);
	delete[] temp;
}
void movgmCR(GLOBL_ARGS_D2) {
	comn_registers _reg = ATTOREGID(reg_addr, mem);
	if (!comn_registers_table::is_num_reg(_reg))
		return;

	void* ptr = comn_registers_table(registers).access(_reg);

	size_t _addr = ((reg_int<size_t>*)ptr)->get();
	unsigned char* temp = new unsigned char[1];
	mem->_MG(temp, 1, _addr);
	registers->cr->set((char)temp[0]);
}

// Input/Output addr into RAX (into RDI soon with the implementation of RSI/RDI (and their respective lower size versions registers))
void movsmFPR(GLOBL_ARGS) {
	size_t addr = registers->rax->get();

	void* regptr = comn_registers_table(registers).access(ATTOREGID(reg, mem));
	if (dynamic_cast<OrphanReg<float>*>((reg_int<float>*)regptr))
		FPR_mem_set((long double)((OrphanReg<float>*)regptr)->get(), addr, mem);
	else if (dynamic_cast<OrphanReg<double>*>((reg_int<double>*)regptr))
		FPR_mem_set((long double)((OrphanReg<double>*)regptr)->get(), addr, mem);
	else if (dynamic_cast<OrphanReg<long double>*>((reg_int<long double>*)regptr))
		FPR_mem_set(((OrphanReg<long double>*)regptr)->get(), addr, mem);
}
void movgmFPR(GLOBL_ARGS) {
	size_t addr = registers->rax->get();
	void* regptr = comn_registers_table(registers).access(ATTOREGID(reg, mem));
	long double ld = 0;

	if (dynamic_cast<OrphanReg<float>*>((reg_int<float>*)regptr)) {
		FPR_mem_get(addr, mem, ld);
		*((OrphanReg<float>*)regptr) = (float)ld;
	}
	else if (dynamic_cast<OrphanReg<double>*>((reg_int<double>*)regptr)) {
		FPR_mem_get(addr, mem, ld);
		*((OrphanReg<double>*)regptr) = (double)ld;
	}
	else if (dynamic_cast<OrphanReg<long double>*>((reg_int<long double>*)regptr))
		FPR_mem_get(addr, mem, ((OrphanReg<long double>*)regptr)->data());
}