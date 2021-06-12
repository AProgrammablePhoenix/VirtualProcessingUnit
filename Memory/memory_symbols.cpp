#include <iostream>
#include <memory>
#include <string>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#define ISWIN 1
#endif

#if defined(__linux__)
#include <cstring>
#include <stdio.h>
#endif

#include "../Registers/regs_decl.h"
#include "../Registers/registers_symbols.h"
#include "mem_arrays.h"
#include "mem_structs.h"
#include "memory_decl.h"

// Memory stack symbols
void pushMem(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	size_t value = ((reg_int<size_t>*)ptr_table.access(reg_id))->get();

	mem->push((unsigned char*)value);
}
void popMem(std::shared_ptr<void> reg, regs* registers,  memory* mem) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned char* value = mem->pop();

	((reg_int<size_t>*)ptr_table.access(reg_id))->set((size_t)value);
}

void pushMemSR(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	std::shared_ptr<std::string> value = std::make_shared<std::string>("");
	b_getSR(value, registers, mem);

	unsigned char *uc_s = new unsigned char[value->size() + 1];
#if defined(ISWIN)
	memcpy_s(uc_s, value->size() + 1, value->c_str(), value->size() + 1);
#else
	std::memcpy(uc_s, value->c_str(), value->size() + 1);
#endif

	mem->push(uc_s, value->size() + 1);
}
void popMemSR(std::shared_ptr<void> unused_p, regs* registers,  memory* mem) {
	extra_registries_ptr_table ptr_table = extra_registries_ptr_table(registers);
	unsigned char* value = mem->pop();

	((reg_int<std::string>*)ptr_table.access(extra_registries::SR))->set(std::string((const char*)value));

	delete[] value;
}

void pushMemCR(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
std::shared_ptr<char> value = std::make_shared<char>('\0');
b_getCR(value, registers, mem);

unsigned char* cptr = new unsigned char(*value);

mem->push(cptr);
}
void popMemCR(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	extra_registries_ptr_table ptr_table = extra_registries_ptr_table(registers);
	unsigned char* v = mem->pop();

	((reg_int<char>*)ptr_table.access(extra_registries::CR))->set((char)*v);

	delete[] v;
}

void pushMemDR(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	std::shared_ptr<double> value = std::make_shared<double>(0);
	b_getDR(value, registers, mem);

	unsigned char* uc_d = new unsigned char[sizeof(double)];

#if defined(ISWIN)
	memcpy_s(uc_d, sizeof(double), value.get(), sizeof(double));
#else
	memcpy(uc_d, value.get(), sizeof(double));
#endif

	mem->push(uc_d, sizeof(double));
}
void popMemDR(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	extra_registries_ptr_table ptr_table = extra_registries_ptr_table(registers);
	unsigned char* v = mem->pop();
	double d = 0;

#if defined(ISWIN)
	memcpy_s(&d, sizeof(double), v, sizeof(double));
#else
	memcpy(&d, v, sizeof(double));
#endif

	((reg_int<double>*)ptr_table.access(extra_registries::DR))->set(d);

	delete[] v;
}

// New memory addressing symbols
void nsms(std::shared_ptr<void> nsize, regs* registers, memory* mem) {
	size_t newsize = *std::static_pointer_cast<size_t>(nsize);
	mem->_SMS(newsize);
}

void movsm(std::shared_ptr<void> reg_addr, regs* registers, memory* mem) {
	registries_def _reg = *std::static_pointer_cast<registries_def>(reg_addr);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	void* ptr = ptr_table.access(_reg);

	size_t _addr = ((reg_int<size_t>*)ptr)->get();

	size_t saved_rax = registers->rax->get();
	popMem(std::make_shared<registries_def>(registries_def::RAX), registers, mem);
	size_t value = registers->rax->get();

	unsigned char* temp = new unsigned char[sizeof(size_t)];
#if defined(ISWIN)
	memcpy_s(temp, sizeof(size_t), &value, sizeof(size_t));
#else
	std::memcpy(temp, &value, sizeof(size_t));
#endif

	mem->_NMS(temp, sizeof(size_t), _addr);
	delete[] temp;

	registers->rax->set(saved_rax);
}
void movgm(std::shared_ptr<void> reg_addr, regs* registers, memory* mem) {
	registries_def _reg = *std::static_pointer_cast<registries_def>(reg_addr);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	void* ptr = ptr_table.access(_reg);

	size_t _addr = ((reg_int<size_t>*)ptr)->get();

	unsigned char* temp = new unsigned char[sizeof(size_t)];
	mem->_NMG(temp, sizeof(size_t), _addr);
	size_t value = 0;

#if defined(ISWIN)
	memcpy_s(&value, sizeof(size_t), temp, sizeof(size_t));
#else
	std::memcpy(&value, temp, sizeof(size_t));
#endif
	delete[] temp;

	size_t saved_rax = registers->rax->get();
	registers->rax->set(value);
	pushMem(std::make_shared<registries_def>(registries_def::RAX), registers, mem);

	registers->rax->set(saved_rax);
}

void movsmSR(std::shared_ptr<void> reg_addr, regs* registers, memory* mem) {
	registries_def _reg = *std::static_pointer_cast<registries_def>(reg_addr);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	void* ptr = ptr_table.access(_reg);

	size_t _addr = ((reg_int<size_t>*)ptr)->get();

	std::string value = registers->sr->get();
	size_t rt_size = value.size() + 1;

	unsigned char* temp = new unsigned char[rt_size];
	unsigned char* t_size = new unsigned char[sizeof(size_t)];
#if defined(ISWIN)
	memcpy_s(temp, rt_size, value.c_str(), rt_size);
	memcpy_s(t_size, sizeof(size_t), &rt_size, sizeof(size_t));
#else
	std::memcpy(temp, value.c_str(), rt_size);
	std::memcpy(t_size, &rt_size, sizeof(size_t));
#endif
	mem->_NMS(t_size, sizeof(size_t), _addr);
	mem->_NMS(temp, rt_size, _addr + sizeof(size_t));
	delete[] t_size;
	delete[] temp;
}
void movgmSR(std::shared_ptr<void> reg_addr, regs* registers, memory* mem) {
	registries_def _reg = *std::static_pointer_cast<registries_def>(reg_addr);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	void* ptr = ptr_table.access(_reg);

	size_t _addr = ((reg_int<size_t>*)ptr)->get();
	size_t rt_size = 0;

	unsigned char* t_size = new unsigned char[sizeof(size_t)];
	mem->_NMG(t_size, sizeof(size_t), _addr);
#if defined(ISWIN)
	memcpy_s(&rt_size, sizeof(size_t), t_size, sizeof(size_t));
#else
	std::memcpy(&rt_size, t_size, sizeof(size_t));
#endif
	delete[] t_size;

	unsigned char* temp = new unsigned char[rt_size];
	mem->_NMG(temp, rt_size, _addr + sizeof(size_t));
	std::string value = std::string(reinterpret_cast<char*>(temp));
	delete[] temp;

	registers->sr->set(value);
}

void movsmCR(std::shared_ptr<void> reg_addr, regs* registers, memory* mem) {
	registries_def _reg = *std::static_pointer_cast<registries_def>(reg_addr);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	void* ptr = ptr_table.access(_reg);

	size_t _addr = ((reg_int<size_t>*)ptr)->get();
	unsigned char* temp = new unsigned char[1];
	temp[0] = registers->cr->get();
	mem->_NMS(temp, 1, _addr);
	delete[] temp;
}
void movgmCR(std::shared_ptr<void> reg_addr, regs* registers, memory* mem) {
	registries_def _reg = *std::static_pointer_cast<registries_def>(reg_addr);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	void* ptr = ptr_table.access(_reg);

	size_t _addr = ((reg_int<size_t>*)ptr)->get();
	unsigned char* temp = new unsigned char[1];
	mem->_NMG(temp, 1, _addr);
	registers->cr->set((char)temp[0]);
}

void movsmDR(std::shared_ptr<void> reg_addr, regs* registers, memory* mem) {
	registries_def _reg = *std::static_pointer_cast<registries_def>(reg_addr);
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
	mem->_NMS(temp, sizeof(double), _addr);
	delete[] temp;
}
void movgmDR(std::shared_ptr<void> reg_addr, regs* registers, memory* mem) {
	registries_def _reg = *std::static_pointer_cast<registries_def>(reg_addr);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	void* ptr = ptr_table.access(_reg);

	size_t _addr = ((reg_int<size_t>*)ptr)->get();
	unsigned char* temp = new unsigned char[sizeof(double)];
	mem->_NMG(temp, sizeof(double), _addr);

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
void m_declArray(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	std::string array_name = registers->sr->get();

	popMemSR(unused_p, registers, mem);
	std::string array_type = registers->sr->get();

	size_t saved_rax = registers->rax->get();
	popMem(std::make_shared<registries_def>(registries_def::RAX), registers, mem);
	size_t array_size = registers->rax->get();

	if (mem->_arrays.getArrayType(array_name) == "UNDEFINED_ARRAY") {
		mem->_arrays.makeArray(array_name, array_type, array_size);
	}

	registers->sr->set(array_name);
}

/* Stack structure before calling:
*	... INDEX ARRAY_NAME
*	With value to set/get in RDX or SR
*/
void m_setAt(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	std::string saved_sr = registers->sr->get();
	popMemSR(unused_p, registers, mem);
	std::string array_name = registers->sr->get();

	registers->sr->set(saved_sr);

	size_t saved_rax = registers->rax->get();
	popMem(std::make_shared<registries_def>(registries_def::RAX), registers, mem);
	size_t index = registers->rax->get();

	if (mem->_arrays.getArrayType(array_name) != "UNDEFINED_ARRAY") {
		mem->_arrays.setArray(array_name, index);
	}

	registers->rax->set(saved_rax);
}
void m_getAt(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	std::string saved_sr = registers->sr->get();
	popMemSR(unused_p, registers, mem);
	std::string array_name = registers->sr->get();

	// If array is not an array of strings, this instruction is needed because in this case the value of SR shouldn't be modified by
	// this method.
	registers->sr->set(saved_sr);

	size_t saved_rax = registers->rax->get();
	popMem(std::make_shared<registries_def>(registries_def::RAX), registers, mem);
	size_t index = registers->rax->get();

	if (mem->_arrays.getArrayType(array_name) != "UNDEFINED_ARRAY") {
		mem->_arrays.getArray(array_name, index);
	}

	registers->rax->set(saved_rax);
}

/* Stack structure before calling:
*	[...]
*	ARRAY_NAME in SR
*	Outputs value in RDX if success
*/
void m_getDynSize(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
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
void m_dyndecl(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
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
void m_dynset(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	std::string saved_sr = registers->sr->get();
	popMemSR(unused_p, registers, mem);
	std::string var_name = registers->sr->get(); 

	registers->sr->set(saved_sr);

	if (mem->_dynvars.getVarType(var_name) != "UNDEFINED_VARIABLE") {
		mem->_dynvars.dynSetVar(var_name);
	}
}
void m_dynget(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
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
void m_structdecl(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	std::string saved_sr = registers->sr->get();
	popMemSR(unused_p, registers, mem);
	std::string structure_name = registers->sr->get();

	registers->sr->set(saved_sr);

	mem->_structs.makeStruct(structure_name);
}
void m_structselect(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
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
void m_structdeclprop(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	std::string saved_sr = registers->sr->get();
	popMemSR(unused_p, registers, mem);
	std::string property_name = registers->sr->get();

	registers->sr->set(saved_sr);

	mem->_structs.declStructProperty(property_name);
}
void m_structset(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
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
void m_structget(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	std::string saved_sr = registers->sr->get();
	popMemSR(unused_p, registers, mem);
	std::string property_name = registers->sr->get();

	registers->sr->set(saved_sr);

	mem->_structs.getStructProperty(property_name);
}
