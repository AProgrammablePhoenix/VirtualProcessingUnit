#include <iostream>
#include <memory>
#include <string>

#include "regs_decl.h"
#include "registers_symbols.h"

void c_b_mov16AX(std::shared_ptr<void> reg_ptr, regs* registers) {
	registers->ax->set(*std::static_pointer_cast<unsigned short>(reg_ptr));
}
void c_b_mov16BX(std::shared_ptr<void> reg_ptr, regs* registers) {
	registers->bx->set(*std::static_pointer_cast<unsigned short>(reg_ptr));
}
void c_b_mov16CX(std::shared_ptr<void> reg_ptr, regs* registers) {
	registers->cx->set(*std::static_pointer_cast<unsigned short>(reg_ptr));
}
void c_b_mov16DX(std::shared_ptr<void> reg_ptr, regs* registers) {
	registers->dx->set(*std::static_pointer_cast<unsigned short>(reg_ptr));
}

void b_mov16AX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	void* ptr = ptr_table.access(reg_id);

	std::shared_ptr<unsigned short> value = std::make_shared<unsigned short>(((reg_int<unsigned short>*)ptr)->get());
	c_b_mov16AX(value, registers);
}
void b_mov16BX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	void* ptr = ptr_table.access(reg_id);

	std::shared_ptr<unsigned short> value = std::make_shared<unsigned short>(((reg_int<unsigned short>*)ptr)->get());
	c_b_mov16BX(value, registers);
}
void b_mov16CX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	void* ptr = ptr_table.access(reg_id);

	std::shared_ptr<unsigned short> value = std::make_shared<unsigned short>(((reg_int<unsigned short>*)ptr)->get());
	c_b_mov16CX(value, registers);
}
void b_mov16DX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	void* ptr = ptr_table.access(reg_id);

	std::shared_ptr<unsigned short> value = std::make_shared<unsigned short>(((reg_int<unsigned short>*)ptr)->get());
	c_b_mov16DX(value, registers);
}

void c_b_mov32EAX(std::shared_ptr<void> reg_ptr, regs* registers) {
	registers->eax->set(*std::static_pointer_cast<unsigned short>(reg_ptr));
}
void c_b_mov32EBX(std::shared_ptr<void> reg_ptr, regs* registers) {
	registers->ebx->set(*std::static_pointer_cast<unsigned short>(reg_ptr));
}
void c_b_mov32ECX(std::shared_ptr<void> reg_ptr, regs* registers) {
	registers->ecx->set(*std::static_pointer_cast<unsigned short>(reg_ptr));
}
void c_b_mov32EDX(std::shared_ptr<void> reg_ptr, regs* registers) {
	registers->edx->set(*std::static_pointer_cast<unsigned short>(reg_ptr));
}

void b_mov32EAX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	void* ptr = ptr_table.access(reg_id);

	std::shared_ptr<unsigned int> value = std::make_shared<unsigned int>(((reg_int<unsigned int>*)ptr)->get());
	c_b_mov32EAX(value, registers);
}
void b_mov32EBX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	void* ptr = ptr_table.access(reg_id);

	std::shared_ptr<unsigned int> value = std::make_shared<unsigned int>(((reg_int<unsigned int>*)ptr)->get());
	c_b_mov32EBX(value, registers);
}
void b_mov32ECX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	void* ptr = ptr_table.access(reg_id);

	std::shared_ptr<unsigned int> value = std::make_shared<unsigned int>(((reg_int<unsigned int>*)ptr)->get());
	c_b_mov32ECX(value, registers);
}
void b_mov32EDX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	void* ptr = ptr_table.access(reg_id);

	std::shared_ptr<unsigned int> value = std::make_shared<unsigned int>(((reg_int<unsigned int>*)ptr)->get());
	c_b_mov32EDX(value, registers);
}

void c_b_mov64RAX(std::shared_ptr<void> reg_ptr, regs* registers) {
	registers->rax->set(*std::static_pointer_cast<unsigned long long>(reg_ptr));
}
void c_b_mov64RBX(std::shared_ptr<void> reg_ptr, regs* registers) {
	registers->rbx->set(*std::static_pointer_cast<unsigned long long>(reg_ptr));
}
void c_b_mov64RCX(std::shared_ptr<void> reg_ptr, regs* registers) {
	registers->rcx->set(*std::static_pointer_cast<unsigned long long>(reg_ptr));
}
void c_b_mov64RDX(std::shared_ptr<void> reg_ptr, regs* registers) {
	registers->rdx->set(*std::static_pointer_cast<unsigned long long>(reg_ptr));
}

void b_mov64RAX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	void* ptr = ptr_table.access(reg_id);

	std::shared_ptr<unsigned long long> value = std::make_shared<unsigned long long>(((reg_int<unsigned long long>*)ptr)->get());
	c_b_mov64RAX(value, registers);
}
void b_mov64RBX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	void* ptr = ptr_table.access(reg_id);

	std::shared_ptr<unsigned long long> value = std::make_shared<unsigned long long>(((reg_int<unsigned long long>*)ptr)->get());
	c_b_mov64RBX(value, registers);
}
void b_mov64RCX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	void* ptr = ptr_table.access(reg_id);

	std::shared_ptr<unsigned long long> value = std::make_shared<unsigned long long>(((reg_int<unsigned long long>*)ptr)->get());
	c_b_mov64RCX(value, registers);
}
void b_mov64RDX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	void* ptr = ptr_table.access(reg_id);

	std::shared_ptr<unsigned long long> value = std::make_shared<unsigned long long>(((reg_int<unsigned long long>*)ptr)->get());
	c_b_mov64RDX(value, registers);
}