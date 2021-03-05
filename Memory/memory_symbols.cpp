#include <iostream>
#include <string>

#include "memory_decl.h"
#include "../Registers/regs_decl.h"
#include "../Registers/registers_symbols.h"

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