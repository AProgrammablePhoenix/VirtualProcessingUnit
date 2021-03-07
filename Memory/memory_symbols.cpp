#include <iostream>
#include <string>

#include "memory_decl.h"
#include "../Registers/regs_decl.h"
#include "../Registers/registers_symbols.h"
#include "mem_arrays.h"

// Memory stack symbols
void pushMem(registries_def reg, regs* registers, memory* mem) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned long long value = ((reg_int<unsigned long long>*)ptr_table.access(reg))->get();

	mem->push((unsigned char*)value);
}
void popMem(registries_def reg, regs* registers,  memory* mem) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned char* value = mem->pop();

	((reg_int<unsigned long long>*)ptr_table.access(reg))->set((unsigned long long)value);
}

void pushMemSR(void* unused_p, regs* registers, memory* mem) {
	std::string value = "";
	b_getSR(&value, registers, mem);

	unsigned char *uc_s = new unsigned char[value.size() + 1];
	memcpy_s(uc_s, value.size() + 1, value.c_str(), value.size() + 1);

	mem->push(uc_s, value.size() + 1);
}
void popMemSR(void* unused_p, regs* registers,  memory* mem) {
	extra_registries_ptr_table ptr_table = extra_registries_ptr_table(registers);
	unsigned char* value = mem->pop();

	((reg_int<std::string>*)ptr_table.access(extra_registries::SR))->set(std::string((const char*)value));
}


// Memory arrays symbols
/* Stack structure before calling:
*	... ARRAY_SIZE ARRAY_TYPE
*	With array_name in SR
*/
void m_declArray(void* unused_p, regs* registers, memory* mem) {
	std::string array_name = registers->sr->get();

	popMemSR(unused_p, registers, mem);
	std::string array_type = registers->sr->get();

	unsigned long long saved_rax = registers->rax->get();
	popMem(registries_def::RAX, registers, mem);
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
void m_setAt(void* unused_p, regs* registers, memory* mem) {
	std::string saved_sr = registers->sr->get();
	popMemSR(unused_p, registers, mem);
	std::string array_name = registers->sr->get();

	registers->sr->set(saved_sr);

	unsigned long long saved_rax = registers->rax->get();
	popMem(registries_def::RAX, registers, mem);
	unsigned long long index = registers->rax->get();

	if (mem->_arrays.getArrayType(array_name) != "UNDEFINED_ARRAY") {
		mem->_arrays.setArray(array_name, index);
	}

	registers->rax->set(saved_rax);
}
void m_getAt(void* unused_p, regs* registers, memory* mem) {
	popMemSR(unused_p, registers, mem);
	std::string array_name = registers->sr->get();

	unsigned long long saved_rax = registers->rax->get();
	popMem(registries_def::RAX, registers, mem);
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
void m_getDynSize(void* unused_p, regs* registers, memory* mem) {
	std::string array_name = registers->sr->get();

	if (mem->_arrays.getArrayType(array_name) != "UNDEFINED_ARRAY") {
		mem->_arrays.getDynSize(array_name);
	}
}