#include <iostream>
#include <string>

#include "registers_symbols.h"
#include "regs_decl.h"

#pragma warning (push)
#pragma warning (disable:26812)

void c_b_mov16AX(void* reg_ptr, regs* registers) {
	registers->ax->set(*(unsigned short*)reg_ptr);
}
void c_b_mov16BX(void* reg_ptr, regs* registers) {
	registers->bx->set(*(unsigned short*)reg_ptr);
}
void c_b_mov16CX(void* reg_ptr, regs* registers) {
	registers->cx->set(*(unsigned short*)reg_ptr);
}
void c_b_mov16DX(void* reg_ptr, regs* registers) {
	registers->dx->set(*(unsigned short*)reg_ptr);
}

void b_mov16AX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	void* ptr = ptr_table.access(reg);

	unsigned short value = ((reg_int<unsigned short>*)ptr)->get();
	c_b_mov16AX(&value, registers);
}
void b_mov16BX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	void* ptr = ptr_table.access(reg);

	unsigned short value = ((reg_int<unsigned short>*)ptr)->get();
	c_b_mov16BX(&value, registers);
}
void b_mov16CX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	void* ptr = ptr_table.access(reg);

	unsigned short value = ((reg_int<unsigned short>*)ptr)->get();
	c_b_mov16CX(&value, registers);
}
void b_mov16DX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	void* ptr = ptr_table.access(reg);

	unsigned short value = ((reg_int<unsigned short>*)ptr)->get();
	c_b_mov16DX(&value, registers);
}

void c_b_mov32EAX(void* reg_ptr, regs* registers) {
	registers->eax->set(*(unsigned int*)reg_ptr);
}
void c_b_mov32EBX(void* reg_ptr, regs* registers) {
	registers->ebx->set(*(unsigned int*)reg_ptr);
}
void c_b_mov32ECX(void* reg_ptr, regs* registers) {
	registers->ecx->set(*(unsigned int*)reg_ptr);
}
void c_b_mov32EDX(void* reg_ptr, regs* registers) {
	registers->edx->set(*(unsigned int*)reg_ptr);
}

void b_mov32EAX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	void* ptr = ptr_table.access(reg);

	unsigned int value = ((reg_int<unsigned int>*)ptr)->get();
	c_b_mov32EAX(&value, registers);
}
void b_mov32EBX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	void* ptr = ptr_table.access(reg);

	unsigned int value = ((reg_int<unsigned int>*)ptr)->get();
	c_b_mov32EBX(&value, registers);
}
void b_mov32ECX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	void* ptr = ptr_table.access(reg);

	unsigned int value = ((reg_int<unsigned int>*)ptr)->get();
	c_b_mov32ECX(&value, registers);
}
void b_mov32EDX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	void* ptr = ptr_table.access(reg);

	unsigned int value = ((reg_int<unsigned int>*)ptr)->get();
	c_b_mov32EDX(&value, registers);
}

void c_b_mov64RAX(void* reg_ptr, regs* registers) {
	registers->rax->set(*(unsigned long long*)reg_ptr);
}
void c_b_mov64RBX(void* reg_ptr, regs* registers) {
	registers->rbx->set(*(unsigned long long*)reg_ptr);
}
void c_b_mov64RCX(void* reg_ptr, regs* registers) {
	registers->rcx->set(*(unsigned long long*)reg_ptr);
}
void c_b_mov64RDX(void* reg_ptr, regs* registers) {
	registers->rdx->set(*(unsigned long long*)reg_ptr);
}

void b_mov64RAX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	void* ptr = ptr_table.access(reg);

	unsigned long long value = ((reg_int<unsigned long long>*)ptr)->get();
	c_b_mov64RAX(&value, registers);
}
void b_mov64RBX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	void* ptr = ptr_table.access(reg);

	unsigned long long value = ((reg_int<unsigned long long>*)ptr)->get();
	c_b_mov64RBX(&value, registers);
}
void b_mov64RCX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	void* ptr = ptr_table.access(reg);

	unsigned long long value = ((reg_int<unsigned long long>*)ptr)->get();
	c_b_mov64RCX(&value, registers);
}
void b_mov64RDX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	void* ptr = ptr_table.access(reg);

	unsigned long long value = ((reg_int<unsigned long long>*)ptr)->get();
	c_b_mov64RDX(&value, registers);
}

#pragma warning (pop)