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
#include "mem_arrays.h"
#include "mem_structs.h"
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
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	void* regptr = ptr_table.access(reg_id);

	((reg_int<size_t>*)regptr)->set(mem->_SDZS());
}

// Memory stack symbols
void pushMem(GLOBL_ARGS) {
	nbyte vopt = std::get<2>(*std::static_pointer_cast<arg_tuple>(reg));
	void* regptr = registries_ptr_table(registers).access(ATTOREGID(reg, mem));
	size_t value = ((reg_int<size_t>*)regptr)->get();

	size_t temp_sz = 0;
	uint8_t qt_det = VOPTTOTQ(vopt);
	if (!qt_det)
		temp_sz = sizeof(size_t);
	else
		temp_sz = qt_det;

	auto temp = std::make_unique<uint8_t[]>(temp_sz);

	mp_memcpy(&value, temp.get());
	mem->push(temp.get(), temp_sz);
}
void popMem(GLOBL_ARGS) {
	nbyte vopt = std::get<2>(*std::static_pointer_cast<arg_tuple>(reg));
	void* regptr = registries_ptr_table(registers).access(ATTOREGID(reg, mem));
	
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
	std::shared_ptr<std::string> value = std::make_shared<std::string>("");
	b_getSR(value, registers, mem);

	size_t rssize = value->size() + 1;

	unsigned char* ssize = new unsigned char[sizeof(size_t)];
	unsigned char *uc_s = new unsigned char[rssize];
	mp_memcpy(&rssize, ssize);

#if defined(ISWIN)
	memcpy_s(uc_s, rssize, value->c_str(), rssize);
#else
	std::memcpy(uc_s, value->c_str(), value->size() + 1);
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

	extra_registries_ptr_table ptr_table = extra_registries_ptr_table(registers);
	((reg_int<std::string>*)ptr_table.access(extra_registries::SR))->set(std::string((const char*)value));

	delete[] value;
	delete[] ssize;
}

void pushMemCR(GLOBL_ARGS_D1) {
	std::shared_ptr<char> value = std::make_shared<char>('\0');
	b_getCR(value, registers, mem);

	unsigned char* cptr = new unsigned char[1];
	cptr[0] = *value;

	mem->push(cptr, 1);
	delete[] cptr;
}
void popMemCR(GLOBL_ARGS_D1) {
	unsigned char* cptr = new unsigned char[1];
	mem->pop(cptr, 1);

	char c = cptr[0];

	extra_registries_ptr_table ptr_table = extra_registries_ptr_table(registers);
	((reg_int<char>*)ptr_table.access(extra_registries::CR))->set(c);

	delete[] cptr;
}

void pushMemFPR(GLOBL_ARGS) {
	void* regptr = extra_registries_ptr_table(registers).access(ATTOXREGID(reg, mem));
	if (dynamic_cast<OrphanReg<float>*>((reg_int<float>*)regptr))
		FPR_push_helper((long double)((OrphanReg<float>*)regptr)->get(),mem);
	else if (dynamic_cast<OrphanReg<double>*>((reg_int<double>*)regptr))
		FPR_push_helper((long double)((OrphanReg<double>*)regptr)->get(), mem);
	else if (dynamic_cast<OrphanReg<long double>*>((reg_int<long double>*)regptr))
		FPR_push_helper(((OrphanReg<long double>*)regptr)->get(), mem);
}
void popMemFPR(GLOBL_ARGS) {
	void* regptr = extra_registries_ptr_table(registers).access(ATTOXREGID(reg, mem));
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
	registries_def _reg = ATTOREGID(reg_addr, mem);
	void* ptr = registries_ptr_table(registers).access(_reg);

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
	registries_def _reg = ATTOREGID(reg_addr, mem);
	void* ptr = registries_ptr_table(registers).access(_reg);

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
	registries_def _reg = ATTOREGID(reg_addr, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	void* ptr = ptr_table.access(_reg);

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
	registries_def _reg = ATTOREGID(reg_addr, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	void* ptr = ptr_table.access(_reg);

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
	registries_def _reg = ATTOREGID(reg_addr, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	void* ptr = ptr_table.access(_reg);

	size_t _addr = ((reg_int<size_t>*)ptr)->get();
	unsigned char* temp = new unsigned char[1];
	temp[0] = registers->cr->get();
	mem->_MS(temp, 1, _addr);
	delete[] temp;
}
void movgmCR(GLOBL_ARGS_D2) {
	registries_def _reg = ATTOREGID(reg_addr, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	void* ptr = ptr_table.access(_reg);

	size_t _addr = ((reg_int<size_t>*)ptr)->get();
	unsigned char* temp = new unsigned char[1];
	mem->_MG(temp, 1, _addr);
	registers->cr->set((char)temp[0]);
}

// Input/Output addr into RAX (into RDI soon with the implementation of RSI/RDI (and their respective lower size versions registers))
void movsmFPR(GLOBL_ARGS) {
	size_t addr = registers->rax->get();

	void* regptr = extra_registries_ptr_table(registers).access(ATTOXREGID(reg, mem));
	if (dynamic_cast<OrphanReg<float>*>((reg_int<float>*)regptr))
		FPR_mem_set((long double)((OrphanReg<float>*)regptr)->get(), addr, mem);
	else if (dynamic_cast<OrphanReg<double>*>((reg_int<double>*)regptr))
		FPR_mem_set((long double)((OrphanReg<double>*)regptr)->get(), addr, mem);
	else if (dynamic_cast<OrphanReg<long double>*>((reg_int<long double>*)regptr))
		FPR_mem_set(((OrphanReg<long double>*)regptr)->get(), addr, mem);
}
void movgmFPR(GLOBL_ARGS) {
	size_t addr = registers->rax->get();
	void* regptr = extra_registries_ptr_table(registers).access(ATTOXREGID(reg, mem));
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

// Memory arrays symbols
/* Stack structure before calling:
*	... ARRAY_SIZE ARRAY_TYPE
*	With array_name in SR
*/
void m_declArray(GLOBL_ARGS_D1) {
	std::string array_name = registers->sr->get();

	popMemSR(unused_p, registers, mem);
	std::string array_type = registers->sr->get();

	unsigned char* temp = new unsigned char[sizeof(size_t)];
	mem->pop(temp, sizeof(size_t));
	size_t array_size = 0;

	mp_memcpy(temp, &array_size);
	delete[] temp;

	if (mem->_arrays.getArrayType(array_name) == "UNDEFINED_ARRAY") {
		mem->_arrays.makeArray(array_name, array_type, array_size);
	}
}

/* Stack structure before calling:
*	... INDEX ARRAY_NAME
*	With value to set/get in RDX or SR
*/
void m_setAt(GLOBL_ARGS_D1) {
	std::string saved_sr = registers->sr->get();
	popMemSR(unused_p, registers, mem);
	std::string array_name = registers->sr->get();

	registers->sr->set(saved_sr);

	unsigned char* temp = new unsigned char[sizeof(size_t)];
	mem->pop(temp, sizeof(size_t));
	size_t index = 0;

	mp_memcpy(temp, &index);
	delete[] temp;

	if (mem->_arrays.getArrayType(array_name) != "UNDEFINED_ARRAY") {
		mem->_arrays.setArray(array_name, index);
	}
}
void m_getAt(GLOBL_ARGS_D1) {
	std::string saved_sr = registers->sr->get();
	popMemSR(unused_p, registers, mem);
	std::string array_name = registers->sr->get();

	// If array is not an array of strings, this instruction is needed because in this case the value of SR shouldn't be modified by
	// this method.
	registers->sr->set(saved_sr);

	unsigned char* temp = new unsigned char[sizeof(size_t)];
	mem->pop(temp, sizeof(size_t));
	size_t index = 0;

	mp_memcpy(temp, &index);
	delete[] temp;

	if (mem->_arrays.getArrayType(array_name) != "UNDEFINED_ARRAY") {
		mem->_arrays.getArray(array_name, index);
	}
}

/* Stack structure before calling:
*	[...]
*	ARRAY_NAME in SR
*	Output value in RDX if success
*/
void m_getDynSize(GLOBL_ARGS_D1) {
	std::string array_name = registers->sr->get();

	if (mem->_arrays.getArrayType(array_name) != "UNDEFINED_ARRAY") {
		mem->_arrays.getDynSize(array_name);
	}
}

// Memory dynamic variables symbols
/* Stack structure before calling:
*	... VAR_TYPE
*	With var_name in SR
*/
void m_dyndecl(GLOBL_ARGS_D1) {
	std::string var_name = registers->sr->get();

	popMemSR(unused_p, registers, mem);
	std::string var_type = registers->sr->get();

	if (mem->_dynvars.getVarType(var_name) == "UNDEFINED_VARIABLE") {
		mem->_dynvars.makeDynVar(var_name, var_type);
	}

	registers->sr->set(var_name);
}

/* Stack structure before calling:
*	... VAR_NAME
*	With value to set/get in RDX or SR
*/
void m_dynset(GLOBL_ARGS_D1) {
	std::string saved_sr = registers->sr->get();
	popMemSR(unused_p, registers, mem);
	std::string var_name = registers->sr->get(); 

	registers->sr->set(saved_sr);

	if (mem->_dynvars.getVarType(var_name) != "UNDEFINED_VARIABLE") {
		mem->_dynvars.dynSetVar(var_name);
	}
}
void m_dynget(GLOBL_ARGS_D1) {
	std::string saved_sr = registers->sr->get();
	popMemSR(unused_p, registers, mem);
	std::string var_name = registers->sr->get();

	// If variable is not a string, this instruction is needed because in this case the value of SR shouldn't be modified by
	// this method.
	registers->sr->set(saved_sr);

	if (mem->_dynvars.getVarType(var_name) != "UNDEFINED_VARIABLE") {
		mem->_dynvars.dynGetVar(var_name);
	}
}

// Memory structs symbols
/* Stack structure before calling:
*	... STRUCT_NAME 
*/
void m_structdecl(GLOBL_ARGS_D1) {
	std::string saved_sr = registers->sr->get();
	popMemSR(unused_p, registers, mem);
	std::string structure_name = registers->sr->get();

	registers->sr->set(saved_sr);

	mem->_structs.makeStruct(structure_name);
}
void m_structselect(GLOBL_ARGS_D1) {
	std::string saved_sr = registers->sr->get();
	popMemSR(unused_p, registers, mem);
	std::string structure_name = registers->sr->get();

	registers->sr->set(saved_sr);

	mem->_structs.selectStruct(structure_name);
}

/* Stack structure before calling:
*	... PROPERTY_VALUE/PROPERTY_TYPE PROPERTY_NAME
*	With structure name in 'structPtr' register by calling corresponding struct select method
*/
void m_structdeclprop(GLOBL_ARGS_D1) {
	std::string saved_sr = registers->sr->get();
	popMemSR(unused_p, registers, mem);
	std::string property_name = registers->sr->get();

	registers->sr->set(saved_sr);

	mem->_structs.declStructProperty(property_name);
}
void m_structset(GLOBL_ARGS_D1) {
	std::string saved_sr = registers->sr->get();
	popMemSR(unused_p, registers, mem);
	std::string property_name = registers->sr->get();

	registers->sr->set(saved_sr);

	mem->_structs.setStructProperty(property_name);
}

/* Stack structure before calling:
*	... PROPERTY_NAME
*  Stack structure after calling:
*	... PROPERTY_VALUE
*/
void m_structget(GLOBL_ARGS_D1) {
	std::string saved_sr = registers->sr->get();
	popMemSR(unused_p, registers, mem);
	std::string property_name = registers->sr->get();

	registers->sr->set(saved_sr);

	mem->_structs.getStructProperty(property_name);
}
