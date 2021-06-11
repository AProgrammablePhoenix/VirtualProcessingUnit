#include <iostream>
#include <memory>
#include <string>

#include "regs_decl.h"
#include "registers_symbols.h"

void b_inc(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	((reg_int<size_t>*)ptr_table.access(reg_id))->set(((reg_int<size_t>*)ptr_table.access(reg_id))->get() + 1);
}
void b_dec(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	((reg_int<size_t>*)ptr_table.access(reg_id))->set(((reg_int<size_t>*)ptr_table.access(reg_id))->get() - 1);
}

void b_incDR(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m) {
	registers->dr->set(registers->dr->get() + 1);
}
void b_decDR(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m) {
	registers->dr->set(registers->dr->get() - 1);
}


void b_mul16AX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg_id))->get();
	unsigned short ax = ((reg_int<unsigned short>*)ptr_table.access(registries_def::AX))->get();

	std::shared_ptr<unsigned short> res = std::make_shared<unsigned short>(value * ax);
	b_set16AX(res, registers, unused_m);
}
void b_mul16BX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg_id))->get();
	unsigned short bx = ((reg_int<unsigned short>*)ptr_table.access(registries_def::BX))->get();

	std::shared_ptr<unsigned short> res = std::make_shared<unsigned short>(value * bx);
	b_get16BX(res, registers, unused_m);
}
void b_mul16CX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg_id))->get();
	unsigned short cx = ((reg_int<unsigned short>*)ptr_table.access(registries_def::CX))->get();

	std::shared_ptr<unsigned short> res = std::make_shared<unsigned short>(value * cx);
	b_set16CX(res, registers, unused_m);
}
void b_mul16DX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg_id))->get();
	unsigned short dx = ((reg_int<unsigned short>*)ptr_table.access(registries_def::DX))->get();

	std::shared_ptr<unsigned short> res = std::make_shared<unsigned short>(value * dx);
	b_set16DX(res, registers, unused_m);
}

void b_mul32EAX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg_id))->get();
	unsigned int eax = ((reg_int<unsigned int>*)ptr_table.access(registries_def::EAX))->get();

	std::shared_ptr<unsigned int> res = std::make_shared<unsigned int>(value * eax);
	b_set32EAX(res, registers, unused_m);
}
void b_mul32EBX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg_id))->get();
	unsigned int ebx = ((reg_int<unsigned int>*)ptr_table.access(registries_def::EBX))->get();

	std::shared_ptr<unsigned int> res = std::make_shared<unsigned int>(value * ebx);
	b_set32EBX(res, registers, unused_m);
}
void b_mul32ECX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg_id))->get();
	unsigned int ecx = ((reg_int<unsigned int>*)ptr_table.access(registries_def::ECX))->get();

	std::shared_ptr<unsigned int> res = std::make_shared<unsigned int>(value * ecx);
	b_set32ECX(res, registers, unused_m);
}
void b_mul32EDX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg_id))->get();
	unsigned int edx = ((reg_int<unsigned int>*)ptr_table.access(registries_def::EDX))->get();

	std::shared_ptr<unsigned int> res = std::make_shared<unsigned int>(value * edx);
	b_set32EDX(res, registers, unused_m);
}

void b_mul64RAX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	size_t value = ((reg_int<size_t>*)ptr_table.access(reg_id))->get();
	size_t rax = ((reg_int<size_t>*)ptr_table.access(registries_def::RAX))->get();

	std::shared_ptr<size_t> res = std::make_shared<size_t>(value * rax);
	b_set64RAX(res, registers, unused_m);
}
void b_mul64RBX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	size_t value = ((reg_int<size_t>*)ptr_table.access(reg_id))->get();
	size_t rbx = ((reg_int<size_t>*)ptr_table.access(registries_def::RBX))->get();

	std::shared_ptr<size_t> res = std::make_shared<size_t>(value * rbx);
	b_set64RBX(res, registers, unused_m);
}
void b_mul64RCX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	size_t value = ((reg_int<size_t>*)ptr_table.access(reg_id))->get();
	size_t rcx = ((reg_int<size_t>*)ptr_table.access(registries_def::RCX))->get();

	std::shared_ptr<size_t> res = std::make_shared<size_t>(value * rcx);
	b_set64RCX(res, registers, unused_m);
}
void b_mul64RDX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	size_t value = ((reg_int<size_t>*)ptr_table.access(reg_id))->get();
	size_t rdx = ((reg_int<size_t>*)ptr_table.access(registries_def::RDX))->get();

	std::shared_ptr<size_t> res = std::make_shared<size_t>(value * rdx);
	b_set64RDX(res, registers, unused_m);
}


void b_div16AX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg_id))->get();
	unsigned short ax = ((reg_int<unsigned short>*)ptr_table.access(registries_def::AX))->get();

	std::shared_ptr<unsigned short> res = std::make_shared<unsigned short>(ax / value);
	b_set16AX(res, registers, unused_m);
}
void b_div16BX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg_id))->get();
	unsigned short bx = ((reg_int<unsigned short>*)ptr_table.access(registries_def::BX))->get();

	std::shared_ptr<unsigned short> res = std::make_shared<unsigned short>(bx / value);
	b_get16BX(res, registers, unused_m);
}
void b_div16CX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg_id))->get();
	unsigned short cx = ((reg_int<unsigned short>*)ptr_table.access(registries_def::CX))->get();

	std::shared_ptr<unsigned short> res = std::make_shared<unsigned short>(cx / value);
	b_set16CX(res, registers, unused_m);
}
void b_div16DX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg_id))->get();
	unsigned short dx = ((reg_int<unsigned short>*)ptr_table.access(registries_def::DX))->get();

	std::shared_ptr<unsigned short> res = std::make_shared<unsigned short>(dx / value);
	b_set16DX(res, registers, unused_m);
}

void b_div32EAX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg_id))->get();
	unsigned int eax = ((reg_int<unsigned int>*)ptr_table.access(registries_def::EAX))->get();
	
	std::shared_ptr<unsigned int> res = std::make_shared<unsigned int>(eax / value);
	b_set32EAX(res, registers, unused_m);
}
void b_div32EBX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg_id))->get();
	unsigned int ebx = ((reg_int<unsigned int>*)ptr_table.access(registries_def::EBX))->get();

	std::shared_ptr<unsigned int> res = std::make_shared<unsigned int>(ebx / value);
	b_set32EBX(res, registers, unused_m);
}
void b_div32ECX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg_id))->get();
	unsigned int ecx = ((reg_int<unsigned int>*)ptr_table.access(registries_def::ECX))->get();

	std::shared_ptr<unsigned int> res = std::make_shared<unsigned int>(ecx / value);
	b_set32ECX(res, registers, unused_m);
}
void b_div32EDX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg_id))->get();
	unsigned int edx = ((reg_int<unsigned int>*)ptr_table.access(registries_def::EDX))->get();

	std::shared_ptr<unsigned int> res = std::make_shared<unsigned int>(edx / value);
	b_set32EDX(res, registers, unused_m);
}

void b_div64RAX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	size_t value = ((reg_int<size_t>*)ptr_table.access(reg_id))->get();
	size_t rax = ((reg_int<size_t>*)ptr_table.access(registries_def::RAX))->get();

	std::shared_ptr<size_t> res = std::make_shared<size_t>(rax / value);
	b_set64RAX(res, registers, unused_m);
}
void b_div64RBX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	size_t value = ((reg_int<size_t>*)ptr_table.access(reg_id))->get();
	size_t rbx = ((reg_int<size_t>*)ptr_table.access(registries_def::RBX))->get();

	std::shared_ptr<size_t> res = std::make_shared<size_t>(rbx / value);
	b_set64RBX(res, registers, unused_m);
}
void b_div64RCX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	size_t value = ((reg_int<size_t>*)ptr_table.access(reg_id))->get();
	size_t rcx = ((reg_int<size_t>*)ptr_table.access(registries_def::RCX))->get();

	std::shared_ptr<size_t> res = std::make_shared<size_t>(rcx / value);
	b_set64RCX(res, registers, unused_m);
}
void b_div64RDX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	size_t value = ((reg_int<size_t>*)ptr_table.access(reg_id))->get();
	size_t rdx = ((reg_int<size_t>*)ptr_table.access(registries_def::RDX))->get();

	std::shared_ptr<size_t> res = std::make_shared<size_t>(rdx / value);
	b_set64RDX(res, registers, unused_m);
}


void b_add16AX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg_id))->get();
	unsigned short ax = ((reg_int<unsigned short>*)ptr_table.access(registries_def::AX))->get();

	std::shared_ptr<unsigned short> res = std::make_shared<unsigned short>(ax + value);
	b_set16AX(res, registers, unused_m);
}
void b_add16BX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg_id))->get();
	unsigned short bx = ((reg_int<unsigned short>*)ptr_table.access(registries_def::BX))->get();

	std::shared_ptr<unsigned short> res = std::make_shared<unsigned short>(bx + value);
	b_set16BX(res, registers, unused_m);
}
void b_add16CX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg_id))->get();
	unsigned short cx = ((reg_int<unsigned short>*)ptr_table.access(registries_def::CX))->get();

	std::shared_ptr<unsigned short> res = std::make_shared<unsigned short>(cx + value);
	b_set16CX(res, registers, unused_m);
}
void b_add16DX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg_id))->get();
	unsigned short dx = ((reg_int<unsigned short>*)ptr_table.access(registries_def::DX))->get();

	std::shared_ptr<unsigned short> res = std::make_shared<unsigned short>(dx + value);
	b_set16DX(res, registers, unused_m);
}

void b_add32EAX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg_id))->get();
	unsigned int eax = ((reg_int<unsigned int>*)ptr_table.access(registries_def::EAX))->get();

	std::shared_ptr<unsigned int> res = std::make_shared<unsigned int>(eax + value);
	b_set32EAX(res, registers, unused_m);
}
void b_add32EBX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg_id))->get();
	unsigned int ebx = ((reg_int<unsigned int>*)ptr_table.access(registries_def::EBX))->get();

	std::shared_ptr<unsigned int> res = std::make_shared<unsigned int>(ebx + value);
	b_set32EBX(res, registers, unused_m);
}
void b_add32ECX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg_id))->get();
	unsigned int ecx = ((reg_int<unsigned int>*)ptr_table.access(registries_def::ECX))->get();

	std::shared_ptr<unsigned int> res = std::make_shared<unsigned int>(ecx + value);
	b_set32ECX(res, registers, unused_m);
}
void b_add32EDX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg_id))->get();
	unsigned int edx = ((reg_int<unsigned int>*)ptr_table.access(registries_def::EDX))->get();

	std::shared_ptr<unsigned int> res = std::make_shared<unsigned int>(edx + value);
	b_set32EDX(res, registers, unused_m);
}

void b_add64RAX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	size_t value = ((reg_int<size_t>*)ptr_table.access(reg_id))->get();
	size_t rax = ((reg_int<size_t>*)ptr_table.access(registries_def::RAX))->get();

	std::shared_ptr<size_t> res = std::make_shared<size_t>(rax + value);
	b_set64RAX(res, registers, unused_m);
}
void b_add64RBX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	size_t value = ((reg_int<size_t>*)ptr_table.access(reg_id))->get();
	size_t rbx = ((reg_int<size_t>*)ptr_table.access(registries_def::RBX))->get();

	std::shared_ptr<size_t> res = std::make_shared<size_t>(rbx + value);
	b_set64RBX(res, registers, unused_m);
}
void b_add64RCX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	size_t value = ((reg_int<size_t>*)ptr_table.access(reg_id))->get();
	size_t rcx = ((reg_int<size_t>*)ptr_table.access(registries_def::RCX))->get();

	std::shared_ptr<size_t> res = std::make_shared<size_t>(rcx + value);
	b_set64RCX(res, registers, unused_m);
}
void b_add64RDX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	size_t value = ((reg_int<size_t>*)ptr_table.access(reg_id))->get();
	size_t rdx = ((reg_int<size_t>*)ptr_table.access(registries_def::RDX))->get();

	std::shared_ptr<size_t> res = std::make_shared<size_t>(rdx + value);
	b_set64RDX(res, registers, unused_m);
}


void b_sub16AX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg_id))->get();
	unsigned short ax = ((reg_int<unsigned short>*)ptr_table.access(registries_def::AX))->get();

	std::shared_ptr<unsigned short> res = std::make_shared<unsigned short>(ax - value);
	b_set16AX(res, registers, unused_m);
}
void b_sub16BX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg_id))->get();
	unsigned short bx = ((reg_int<unsigned short>*)ptr_table.access(registries_def::BX))->get();

	std::shared_ptr<unsigned short> res = std::make_shared<unsigned short>(bx - value);
	b_set16BX(res, registers, unused_m);
}
void b_sub16CX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg_id))->get();
	unsigned short cx = ((reg_int<unsigned short>*)ptr_table.access(registries_def::CX))->get();

	std::shared_ptr<unsigned short> res = std::make_shared<unsigned short>(cx - value);
	b_set16CX(res, registers, unused_m);
}
void b_sub16DX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg_id))->get();
	unsigned short dx = ((reg_int<unsigned short>*)ptr_table.access(registries_def::DX))->get();

	std::shared_ptr<unsigned short> res = std::make_shared<unsigned short>(dx - value);
	b_set16DX(res, registers, unused_m);
}

void b_sub32EAX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg_id))->get();
	unsigned int eax = ((reg_int<unsigned int>*)ptr_table.access(registries_def::EAX))->get();

	std::shared_ptr<unsigned int> res = std::make_shared<unsigned int>(eax - value);
	b_set32EAX(res, registers, unused_m);
}
void b_sub32EBX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg_id))->get();
	unsigned int ebx = ((reg_int<unsigned int>*)ptr_table.access(registries_def::EBX))->get();

	std::shared_ptr<unsigned int> res = std::make_shared<unsigned int>(ebx - value);
	b_set32EBX(res, registers, unused_m);
}
void b_sub32ECX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg_id))->get();
	unsigned int ecx = ((reg_int<unsigned int>*)ptr_table.access(registries_def::ECX))->get();

	std::shared_ptr<unsigned int> res = std::make_shared<unsigned int>(ecx - value);
	b_set32ECX(res, registers, unused_m);
}
void b_sub32EDX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg_id))->get();
	unsigned int edx = ((reg_int<unsigned int>*)ptr_table.access(registries_def::EDX))->get();

	std::shared_ptr<unsigned int> res = std::make_shared<unsigned int>(edx - value);
	b_set32EDX(res, registers, unused_m);
}

void b_sub64RAX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	size_t value = ((reg_int<size_t>*)ptr_table.access(reg_id))->get();
	size_t rax = ((reg_int<size_t>*)ptr_table.access(registries_def::RAX))->get();

	std::shared_ptr<size_t> res = std::make_shared<size_t>(rax - value);
	b_set64RAX(res, registers, unused_m);
}
void b_sub64RBX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	size_t value = ((reg_int<size_t>*)ptr_table.access(reg_id))->get();
	size_t rbx = ((reg_int<size_t>*)ptr_table.access(registries_def::RBX))->get();

	std::shared_ptr<size_t> res = std::make_shared<size_t>(rbx - value);
	b_set64RBX(res, registers, unused_m);
}
void b_sub64RCX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	size_t value = ((reg_int<size_t>*)ptr_table.access(reg_id))->get();
	size_t rcx = ((reg_int<size_t>*)ptr_table.access(registries_def::RCX))->get();

	std::shared_ptr<size_t> res = std::make_shared<size_t>(rcx - value);
	b_set64RCX(res, registers, unused_m);
}
void b_sub64RDX(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	size_t value = ((reg_int<size_t>*)ptr_table.access(reg_id))->get();
	size_t rdx = ((reg_int<size_t>*)ptr_table.access(registries_def::RDX))->get();

	std::shared_ptr<size_t> res = std::make_shared<size_t>(rdx - value);
	b_set64RDX(res, registers, unused_m);
}