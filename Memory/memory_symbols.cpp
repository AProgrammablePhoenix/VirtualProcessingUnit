#include <iostream>
#include <memory>
#include <string>

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
	unsigned long long value = ((reg_int<unsigned long long>*)ptr_table.access(reg_id))->get();

	mem->push((unsigned char*)value);
}
void popMem(std::shared_ptr<void> reg, regs* registers,  memory* mem) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned char* value = mem->pop();

	((reg_int<unsigned long long>*)ptr_table.access(reg_id))->set((unsigned long long)value);
}

void pushMemSR(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	std::shared_ptr<std::string> value = std::make_shared<std::string>("");
	b_getSR(value, registers, mem);

	unsigned char *uc_s = new unsigned char[value->size() + 1];
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
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

	unsigned long long saved_rax = registers->rax->get();
	popMem(std::make_shared<registries_def>(registries_def::RAX), registers, mem);
	unsigned long long array_size = registers->rax->get();

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

	unsigned long long saved_rax = registers->rax->get();
	popMem(std::make_shared<registries_def>(registries_def::RAX), registers, mem);
	unsigned long long index = registers->rax->get();

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

	unsigned long long saved_rax = registers->rax->get();
	popMem(std::make_shared<registries_def>(registries_def::RAX), registers, mem);
	unsigned long long index = registers->rax->get();

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