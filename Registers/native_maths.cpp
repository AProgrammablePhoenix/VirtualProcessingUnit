#include <iostream>
#include <string>

#include "regs_decl.h"
#include "registers_symbols.h"

void b_inc(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	((reg_int<unsigned long long>*)ptr_table.access(reg))->set(((reg_int<unsigned long long>*)ptr_table.access(reg))->get() + 1);
}
void b_dec(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	((reg_int<unsigned long long>*)ptr_table.access(reg))->set(((reg_int<unsigned long long>*)ptr_table.access(reg))->get() - 1);
}


void b_mul16AX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg))->get();
	unsigned short ax = ((reg_int<unsigned short>*)ptr_table.access(registries_def::AX))->get();

	std::shared_ptr<unsigned short> res = std::make_shared<unsigned short>(value * ax);
	b_set16AX(res, registers, unused_m);
}
void b_mul16BX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg))->get();
	unsigned short bx = ((reg_int<unsigned short>*)ptr_table.access(registries_def::BX))->get();

	std::shared_ptr<unsigned short> res = std::make_shared<unsigned short>(value * bx);
	b_get16BX(res, registers, unused_m);
}
void b_mul16CX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg))->get();
	unsigned short cx = ((reg_int<unsigned short>*)ptr_table.access(registries_def::CX))->get();

	std::shared_ptr<unsigned short> res = std::make_shared<unsigned short>(value * cx);
	b_set16CX(res, registers, unused_m);
}
void b_mul16DX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg))->get();
	unsigned short dx = ((reg_int<unsigned short>*)ptr_table.access(reg))->get();

	std::shared_ptr<unsigned short> res = std::make_shared<unsigned short>(value * dx);
	b_set16DX(res, registers, unused_m);
}

void b_mul32EAX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg))->get();
	unsigned int eax = ((reg_int<unsigned int>*)ptr_table.access(registries_def::EAX))->get();

	std::shared_ptr<unsigned int> res = std::make_shared<unsigned int>(value * eax);
	b_set32EAX(res, registers, unused_m);
}
void b_mul32EBX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg))->get();
	unsigned int ebx = ((reg_int<unsigned int>*)ptr_table.access(registries_def::EBX))->get();

	std::shared_ptr<unsigned int> res = std::make_shared<unsigned int>(value * ebx);
	b_set32EBX(res, registers, unused_m);
}
void b_mul32ECX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg))->get();
	unsigned int ecx = ((reg_int<unsigned int>*)ptr_table.access(registries_def::ECX))->get();

	std::shared_ptr<unsigned int> res = std::make_shared<unsigned int>(value * ecx);
	b_set32ECX(res, registers, unused_m);
}
void b_mul32EDX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg))->get();
	unsigned int edx = ((reg_int<unsigned int>*)ptr_table.access(registries_def::EDX))->get();

	std::shared_ptr<unsigned int> res = std::make_shared<unsigned int>(value * edx);
	b_set32EDX(res, registers, unused_m);
}

void b_mul64RAX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned long long value = ((reg_int<unsigned long long>*)ptr_table.access(reg))->get();
	unsigned long long rax = ((reg_int<unsigned long long>*)ptr_table.access(registries_def::RAX))->get();

	std::shared_ptr<unsigned long long> res = std::make_shared<unsigned long long>(value * rax);
	b_set64RAX(res, registers, unused_m);
}
void b_mul64RBX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned long long value = ((reg_int<unsigned long long>*)ptr_table.access(reg))->get();
	unsigned long long rbx = ((reg_int<unsigned long long>*)ptr_table.access(registries_def::RBX))->get();

	std::shared_ptr<unsigned long long> res = std::make_shared<unsigned long long>(value * rbx);
	b_set64RBX(res, registers, unused_m);
}
void b_mul64RCX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned long long value = ((reg_int<unsigned long long>*)ptr_table.access(reg))->get();
	unsigned long long rcx = ((reg_int<unsigned long long>*)ptr_table.access(registries_def::RCX))->get();

	std::shared_ptr<unsigned long long> res = std::make_shared<unsigned long long>(value * rcx);
	b_set64RCX(res, registers, unused_m);
}
void b_mul64RDX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned long long value = ((reg_int<unsigned long long>*)ptr_table.access(reg))->get();
	unsigned long long rdx = ((reg_int<unsigned long long>*)ptr_table.access(registries_def::RDX))->get();

	std::shared_ptr<unsigned long long> res = std::make_shared<unsigned long long>(value * rdx);
	b_set64RDX(res, registers, unused_m);
}


void b_div16AX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg))->get();
	unsigned short ax = ((reg_int<unsigned short>*)ptr_table.access(registries_def::AX))->get();

	std::shared_ptr<unsigned short> res = std::make_shared<unsigned short>(ax / value);
	b_set16AX(res, registers, unused_m);
}
void b_div16BX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg))->get();
	unsigned short bx = ((reg_int<unsigned short>*)ptr_table.access(registries_def::BX))->get();

	std::shared_ptr<unsigned short> res = std::make_shared<unsigned short>(bx / value);
	b_get16BX(res, registers, unused_m);
}
void b_div16CX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg))->get();
	unsigned short cx = ((reg_int<unsigned short>*)ptr_table.access(registries_def::CX))->get();

	std::shared_ptr<unsigned short> res = std::make_shared<unsigned short>(cx / value);
	b_set16CX(res, registers, unused_m);
}
void b_div16DX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg))->get();
	unsigned short dx = ((reg_int<unsigned short>*)ptr_table.access(reg))->get();

	std::shared_ptr<unsigned short> res = std::make_shared<unsigned short>(dx / value);
	b_set16DX(res, registers, unused_m);
}

void b_div32EAX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg))->get();
	unsigned int eax = ((reg_int<unsigned int>*)ptr_table.access(registries_def::EAX))->get();
	
	std::shared_ptr<unsigned int> res = std::make_shared<unsigned int>(eax / value);
	b_set32EAX(res, registers, unused_m);
}
void b_div32EBX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg))->get();
	unsigned int ebx = ((reg_int<unsigned int>*)ptr_table.access(registries_def::EBX))->get();

	std::shared_ptr<unsigned int> res = std::make_shared<unsigned int>(ebx / value);
	b_set32EBX(res, registers, unused_m);
}
void b_div32ECX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg))->get();
	unsigned int ecx = ((reg_int<unsigned int>*)ptr_table.access(registries_def::ECX))->get();

	std::shared_ptr<unsigned int> res = std::make_shared<unsigned int>(ecx / value);
	b_set32ECX(res, registers, unused_m);
}
void b_div32EDX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg))->get();
	unsigned int edx = ((reg_int<unsigned int>*)ptr_table.access(registries_def::EDX))->get();

	std::shared_ptr<unsigned int> res = std::make_shared<unsigned int>(edx / value);
	b_set32EDX(res, registers, unused_m);
}

void b_div64RAX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned long long value = ((reg_int<unsigned long long>*)ptr_table.access(reg))->get();
	unsigned long long rax = ((reg_int<unsigned long long>*)ptr_table.access(registries_def::RAX))->get();

	std::shared_ptr<unsigned long long> res = std::make_shared<unsigned long long>(rax / value);
	b_set64RAX(res, registers, unused_m);
}
void b_div64RBX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned long long value = ((reg_int<unsigned long long>*)ptr_table.access(reg))->get();
	unsigned long long rbx = ((reg_int<unsigned long long>*)ptr_table.access(registries_def::RBX))->get();

	std::shared_ptr<unsigned long long> res = std::make_shared<unsigned long long>(rbx / value);
	b_set64RBX(res, registers, unused_m);
}
void b_div64RCX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned long long value = ((reg_int<unsigned long long>*)ptr_table.access(reg))->get();
	unsigned long long rcx = ((reg_int<unsigned long long>*)ptr_table.access(registries_def::RCX))->get();

	std::shared_ptr<unsigned long long> res = std::make_shared<unsigned long long>(rcx / value);
	b_set64RCX(res, registers, unused_m);
}
void b_div64RDX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned long long value = ((reg_int<unsigned long long>*)ptr_table.access(reg))->get();
	unsigned long long rdx = ((reg_int<unsigned long long>*)ptr_table.access(registries_def::RDX))->get();

	std::shared_ptr<unsigned long long> res = std::make_shared<unsigned long long>(rdx / value);
	b_set64RDX(res, registers, unused_m);
}


void b_add16AX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg))->get();
	unsigned short ax = ((reg_int<unsigned short>*)ptr_table.access(registries_def::AX))->get();

	std::shared_ptr<unsigned short> res = std::make_shared<unsigned short>(ax + value);
	b_set16AX(res, registers, unused_m);
}
void b_add16BX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg))->get();
	unsigned short bx = ((reg_int<unsigned short>*)ptr_table.access(registries_def::BX))->get();

	std::shared_ptr<unsigned short> res = std::make_shared<unsigned short>(bx + value);
	b_set16BX(res, registers, unused_m);
}
void b_add16CX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg))->get();
	unsigned short cx = ((reg_int<unsigned short>*)ptr_table.access(registries_def::CX))->get();

	std::shared_ptr<unsigned short> res = std::make_shared<unsigned short>(cx + value);
	b_set16CX(res, registers, unused_m);
}
void b_add16DX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg))->get();
	unsigned short dx = ((reg_int<unsigned short>*)ptr_table.access(registries_def::DX))->get();

	std::shared_ptr<unsigned short> res = std::make_shared<unsigned short>(dx + value);
	b_set16DX(res, registers, unused_m);
}

void b_add32EAX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg))->get();
	unsigned int eax = ((reg_int<unsigned int>*)ptr_table.access(registries_def::EAX))->get();

	std::shared_ptr<unsigned int> res = std::make_shared<unsigned int>(eax + value);
	b_set32EAX(res, registers, unused_m);
}
void b_add32EBX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg))->get();
	unsigned int ebx = ((reg_int<unsigned int>*)ptr_table.access(registries_def::EBX))->get();

	std::shared_ptr<unsigned int> res = std::make_shared<unsigned int>(ebx + value);
	b_set32EBX(res, registers, unused_m);
}
void b_add32ECX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg))->get();
	unsigned int ecx = ((reg_int<unsigned int>*)ptr_table.access(registries_def::ECX))->get();

	std::shared_ptr<unsigned int> res = std::make_shared<unsigned int>(ecx + value);
	b_set32ECX(res, registers, unused_m);
}
void b_add32EDX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg))->get();
	unsigned int edx = ((reg_int<unsigned int>*)ptr_table.access(registries_def::EDX))->get();

	std::shared_ptr<unsigned int> res = std::make_shared<unsigned int>(edx + value);
	b_set32EDX(res, registers, unused_m);
}

void b_add64RAX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned long long value = ((reg_int<unsigned long long>*)ptr_table.access(reg))->get();
	unsigned long long rax = ((reg_int<unsigned long long>*)ptr_table.access(registries_def::RAX))->get();

	std::shared_ptr<unsigned long long> res = std::make_shared<unsigned long long>(rax + value);
	b_set64RAX(res, registers, unused_m);
}
void b_add64RBX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned long long value = ((reg_int<unsigned long long>*)ptr_table.access(reg))->get();
	unsigned long long rbx = ((reg_int<unsigned long long>*)ptr_table.access(registries_def::RBX))->get();

	std::shared_ptr<unsigned long long> res = std::make_shared<unsigned long long>(rbx + value);
	b_set64RBX(res, registers, unused_m);
}
void b_add64RCX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned long long value = ((reg_int<unsigned long long>*)ptr_table.access(reg))->get();
	unsigned long long rcx = ((reg_int<unsigned long long>*)ptr_table.access(registries_def::RCX))->get();

	std::shared_ptr<unsigned long long> res = std::make_shared<unsigned long long>(rcx + value);
	b_set64RCX(res, registers, unused_m);
}
void b_add64RDX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned long long value = ((reg_int<unsigned long long>*)ptr_table.access(reg))->get();
	unsigned long long rdx = ((reg_int<unsigned long long>*)ptr_table.access(registries_def::RDX))->get();

	std::shared_ptr<unsigned long long> res = std::make_shared<unsigned long long>(rdx + value);
	b_set64RDX(res, registers, unused_m);
}


void b_sub16AX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg))->get();
	unsigned short ax = ((reg_int<unsigned short>*)ptr_table.access(registries_def::AX))->get();

	std::shared_ptr<unsigned short> res = std::make_shared<unsigned short>(ax - value);
	b_set16AX(res, registers, unused_m);
}
void b_sub16BX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg))->get();
	unsigned short bx = ((reg_int<unsigned short>*)ptr_table.access(registries_def::BX))->get();

	std::shared_ptr<unsigned short> res = std::make_shared<unsigned short>(bx - value);
	b_set16BX(res, registers, unused_m);
}
void b_sub16CX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg))->get();
	unsigned short cx = ((reg_int<unsigned short>*)ptr_table.access(registries_def::CX))->get();

	std::shared_ptr<unsigned short> res = std::make_shared<unsigned short>(cx - value);
	b_set16CX(res, registers, unused_m);
}
void b_sub16DX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg))->get();
	unsigned short dx = ((reg_int<unsigned short>*)ptr_table.access(registries_def::DX))->get();

	std::shared_ptr<unsigned short> res = std::make_shared<unsigned short>(dx - value);
	b_set16DX(res, registers, unused_m);
}

void b_sub32EAX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg))->get();
	unsigned int eax = ((reg_int<unsigned int>*)ptr_table.access(registries_def::EAX))->get();

	std::shared_ptr<unsigned int> res = std::make_shared<unsigned int>(eax - value);
	b_set32EAX(res, registers, unused_m);
}
void b_sub32EBX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg))->get();
	unsigned int ebx = ((reg_int<unsigned int>*)ptr_table.access(registries_def::EBX))->get();

	std::shared_ptr<unsigned int> res = std::make_shared<unsigned int>(ebx - value);
	b_set32EBX(res, registers, unused_m);
}
void b_sub32ECX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg))->get();
	unsigned int ecx = ((reg_int<unsigned int>*)ptr_table.access(registries_def::ECX))->get();

	std::shared_ptr<unsigned int> res = std::make_shared<unsigned int>(ecx - value);
	b_set32ECX(res, registers, unused_m);
}
void b_sub32EDX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg))->get();
	unsigned int edx = ((reg_int<unsigned int>*)ptr_table.access(registries_def::EDX))->get();

	std::shared_ptr<unsigned int> res = std::make_shared<unsigned int>(edx - value);
	b_set32EDX(res, registers, unused_m);
}

void b_sub64RAX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned long long value = ((reg_int<unsigned long long>*)ptr_table.access(reg))->get();
	unsigned long long rax = ((reg_int<unsigned long long>*)ptr_table.access(registries_def::RAX))->get();

	std::shared_ptr<unsigned long long> res = std::make_shared<unsigned long long>(rax - value);
	b_set64RAX(res, registers, unused_m);
}
void b_sub64RBX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned long long value = ((reg_int<unsigned long long>*)ptr_table.access(reg))->get();
	unsigned long long rbx = ((reg_int<unsigned long long>*)ptr_table.access(registries_def::RBX))->get();

	std::shared_ptr<unsigned long long> res = std::make_shared<unsigned long long>(rbx - value);
	b_set64RBX(res, registers, unused_m);
}
void b_sub64RCX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned long long value = ((reg_int<unsigned long long>*)ptr_table.access(reg))->get();
	unsigned long long rcx = ((reg_int<unsigned long long>*)ptr_table.access(registries_def::RCX))->get();

	std::shared_ptr<unsigned long long> res = std::make_shared<unsigned long long>(rcx - value);
	b_set64RCX(res, registers, unused_m);
}
void b_sub64RDX(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned long long value = ((reg_int<unsigned long long>*)ptr_table.access(reg))->get();
	unsigned long long rdx = ((reg_int<unsigned long long>*)ptr_table.access(registries_def::RDX))->get();

	std::shared_ptr<unsigned long long> res = std::make_shared<unsigned long long>(rdx - value);
	b_set64RDX(res, registers, unused_m);
}