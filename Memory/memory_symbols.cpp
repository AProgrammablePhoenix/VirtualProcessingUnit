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

void sdzsMem(GLOBL_ARGS) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	void* regptr = ptr_table.access(reg_id);

	((reg_int<size_t>*)regptr)->set(mem->_SDZS());
}

// Memory stack symbols
void pushMem(GLOBL_ARGS) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	void* regptr = ptr_table.access(reg_id);
	size_t value = ((reg_int<size_t>*)regptr)->get();

	unsigned char* temp = new unsigned char[sizeof(size_t)];
	mp_memcpy(&value, temp);

	mem->push(temp, sizeof(size_t));
	delete[] temp;
}
void popMem(GLOBL_ARGS) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned char* temp = new unsigned char[sizeof(size_t)];

	mem->pop(temp, sizeof(size_t));
	size_t value = 0;

	mp_memcpy(temp, &value);
	((reg_int<size_t>*)ptr_table.access(reg_id))->set(value);

	delete[] temp;
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

void pushMemDR(GLOBL_ARGS_D1) {
	std::shared_ptr<double> value = std::make_shared<double>(0);
	b_getDR(value, registers, mem);

	unsigned char* uc_d = new unsigned char[sizeof(double)];

#if defined(ISWIN)
	memcpy_s(uc_d, sizeof(double), value.get(), sizeof(double));
#else
	memcpy(uc_d, value.get(), sizeof(double));
#endif

	mem->push(uc_d, sizeof(double));
	delete[] uc_d;
}
void popMemDR(GLOBL_ARGS_D1) {
	extra_registries_ptr_table ptr_table = extra_registries_ptr_table(registers);
	unsigned char* uc_d = new unsigned char[sizeof(double)];
	mem->pop(uc_d, sizeof(double));

	double d = 0;
	mp_memcpy(uc_d, &d, sizeof(double));

	((reg_int<double>*)ptr_table.access(extra_registries::DR))->set(d);

	delete[] uc_d;
}

void movsm(GLOBL_ARGS_D2) {
	registries_def _reg = ATTOREGID(reg_addr, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	void* ptr = ptr_table.access(_reg);

	size_t _addr = ((reg_int<size_t>*)ptr)->get();

	unsigned char* temp = new unsigned char[sizeof(size_t)];
	mem->pop(temp, sizeof(size_t));
	mem->_MS(temp, sizeof(size_t), _addr);
	delete[] temp;
}
void movgm(GLOBL_ARGS_D2) {
	registries_def _reg = ATTOREGID(reg_addr, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	void* ptr = ptr_table.access(_reg);

	size_t _addr = ((reg_int<size_t>*)ptr)->get();

	unsigned char* temp = new unsigned char[sizeof(size_t)];
	mem->_MG(temp, sizeof(size_t), _addr);	
	mem->push(temp, sizeof(size_t));
	delete[] temp;
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

void movsmDR(GLOBL_ARGS_D2) {
	registries_def _reg = ATTOREGID(reg_addr, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	void* ptr = ptr_table.access(_reg);

	size_t _addr = ((reg_int<size_t>*)ptr)->get();
	double value = registers->dr->get();
	unsigned char* temp = new unsigned char[sizeof(double)];

#if defined(ISWIN)
	memcpy_s(temp, sizeof(double), &value, sizeof(double));
#else
	std::memcpy(temp, &value, sizeof(double));
#endif
	mem->_MS(temp, sizeof(double), _addr);
	delete[] temp;
}
void movgmDR(GLOBL_ARGS_D2) {
	registries_def _reg = ATTOREGID(reg_addr, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	void* ptr = ptr_table.access(_reg);

	size_t _addr = ((reg_int<size_t>*)ptr)->get();
	unsigned char* temp = new unsigned char[sizeof(double)];
	mem->_MG(temp, sizeof(double), _addr);

	double value = 0;
#if defined(ISWIN)
	memcpy_s(&value, sizeof(double), temp, sizeof(double));
#else
	std::memcpy(&value, temp, sizeof(double));
#endif
	delete[] temp;

	registers->dr->set(value);
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