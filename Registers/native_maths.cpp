#include <iostream>
#include <memory>
#include <string>

#include "../utility.h"
#include "regs_decl.h"
#include "registers_symbols.h"

void b_inc(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	((reg_int<size_t>*)ptr_table.access(reg_id))->set(((reg_int<size_t>*)ptr_table.access(reg_id))->get() + 1);
}
void b_dec(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	((reg_int<size_t>*)ptr_table.access(reg_id))->set(((reg_int<size_t>*)ptr_table.access(reg_id))->get() - 1);
}

void b_incDR(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	registers->dr->set(registers->dr->get() + 1);
}
void b_decDR(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	registers->dr->set(registers->dr->get() - 1);
}


void b_mul16AX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg_id))->get();
	unsigned short ax = ((reg_int<unsigned short>*)ptr_table.access(registries_def::AX))->get();

	registers->ax->set(value * ax);
}
void b_mul16BX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg_id))->get();
	unsigned short bx = ((reg_int<unsigned short>*)ptr_table.access(registries_def::BX))->get();

	registers->bx->set(value * bx);
}
void b_mul16CX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg_id))->get();
	unsigned short cx = ((reg_int<unsigned short>*)ptr_table.access(registries_def::CX))->get();

	registers->cx->set(value * cx);
}
void b_mul16DX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg_id))->get();
	unsigned short dx = ((reg_int<unsigned short>*)ptr_table.access(registries_def::DX))->get();

	registers->dx->set(value * dx);
}

void b_mul32EAX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg_id))->get();
	unsigned int eax = ((reg_int<unsigned int>*)ptr_table.access(registries_def::EAX))->get();

	registers->eax->set(value * eax);
}
void b_mul32EBX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg_id))->get();
	unsigned int ebx = ((reg_int<unsigned int>*)ptr_table.access(registries_def::EBX))->get();

	registers->ebx->set(value * ebx);
}
void b_mul32ECX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg_id))->get();
	unsigned int ecx = ((reg_int<unsigned int>*)ptr_table.access(registries_def::ECX))->get();

	registers->ecx->set(value * ecx);
}
void b_mul32EDX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg_id))->get();
	unsigned int edx = ((reg_int<unsigned int>*)ptr_table.access(registries_def::EDX))->get();

	registers->edx->set(value * edx);
}

void b_mul64RAX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	size_t value = ((reg_int<size_t>*)ptr_table.access(reg_id))->get();
	size_t rax = ((reg_int<size_t>*)ptr_table.access(registries_def::RAX))->get();

	registers->rax->set(value * rax);
}
void b_mul64RBX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	size_t value = ((reg_int<size_t>*)ptr_table.access(reg_id))->get();
	size_t rbx = ((reg_int<size_t>*)ptr_table.access(registries_def::RBX))->get();

	registers->rbx->set(value * rbx);
}
void b_mul64RCX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	size_t value = ((reg_int<size_t>*)ptr_table.access(reg_id))->get();
	size_t rcx = ((reg_int<size_t>*)ptr_table.access(registries_def::RCX))->get();

	registers->rcx->set(value * rcx);
}
void b_mul64RDX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	size_t value = ((reg_int<size_t>*)ptr_table.access(reg_id))->get();
	size_t rdx = ((reg_int<size_t>*)ptr_table.access(registries_def::RDX))->get();

	registers->rdx->set(value * rdx);
}


void b_div16AX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg_id))->get();
	unsigned short ax = ((reg_int<unsigned short>*)ptr_table.access(registries_def::AX))->get();

	registers->ax->set(ax / value);
}
void b_div16BX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg_id))->get();
	unsigned short bx = ((reg_int<unsigned short>*)ptr_table.access(registries_def::BX))->get();

	registers->bx->set(bx / value);
}
void b_div16CX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg_id))->get();
	unsigned short cx = ((reg_int<unsigned short>*)ptr_table.access(registries_def::CX))->get();

	registers->cx->set(cx / value);
}
void b_div16DX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg_id))->get();
	unsigned short dx = ((reg_int<unsigned short>*)ptr_table.access(registries_def::DX))->get();

	registers->dx->set(dx / value);
}

void b_div32EAX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg_id))->get();
	unsigned int eax = ((reg_int<unsigned int>*)ptr_table.access(registries_def::EAX))->get();
	
	registers->eax->set(eax / value);
}
void b_div32EBX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg_id))->get();
	unsigned int ebx = ((reg_int<unsigned int>*)ptr_table.access(registries_def::EBX))->get();

	registers->ebx->set(ebx / value);
}
void b_div32ECX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg_id))->get();
	unsigned int ecx = ((reg_int<unsigned int>*)ptr_table.access(registries_def::ECX))->get();

	registers->ecx->set(ecx / value);
}
void b_div32EDX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg_id))->get();
	unsigned int edx = ((reg_int<unsigned int>*)ptr_table.access(registries_def::EDX))->get();

	registers->edx->set(edx / value);
}

void b_div64RAX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	size_t value = ((reg_int<size_t>*)ptr_table.access(reg_id))->get();
	size_t rax = ((reg_int<size_t>*)ptr_table.access(registries_def::RAX))->get();

	registers->rax->set(rax / value);
}
void b_div64RBX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	size_t value = ((reg_int<size_t>*)ptr_table.access(reg_id))->get();
	size_t rbx = ((reg_int<size_t>*)ptr_table.access(registries_def::RBX))->get();

	registers->rbx->set(rbx / value);
}
void b_div64RCX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	size_t value = ((reg_int<size_t>*)ptr_table.access(reg_id))->get();
	size_t rcx = ((reg_int<size_t>*)ptr_table.access(registries_def::RCX))->get();

	registers->rcx->set(rcx / value);
}
void b_div64RDX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	size_t value = ((reg_int<size_t>*)ptr_table.access(reg_id))->get();
	size_t rdx = ((reg_int<size_t>*)ptr_table.access(registries_def::RDX))->get();

	registers->rdx->set(rdx / value);
}


void b_add16AX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg_id))->get();
	unsigned short ax = ((reg_int<unsigned short>*)ptr_table.access(registries_def::AX))->get();

	registers->ax->set(ax + value);
}
void b_add16BX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg_id))->get();
	unsigned short bx = ((reg_int<unsigned short>*)ptr_table.access(registries_def::BX))->get();

	registers->bx->set(bx + value);
}
void b_add16CX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg_id))->get();
	unsigned short cx = ((reg_int<unsigned short>*)ptr_table.access(registries_def::CX))->get();

	registers->cx->set(cx + value);
}
void b_add16DX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg_id))->get();
	unsigned short dx = ((reg_int<unsigned short>*)ptr_table.access(registries_def::DX))->get();

	registers->dx->set(dx + value);
}

void b_add32EAX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg_id))->get();
	unsigned int eax = ((reg_int<unsigned int>*)ptr_table.access(registries_def::EAX))->get();

	registers->eax->set(eax + value);
}
void b_add32EBX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg_id))->get();
	unsigned int ebx = ((reg_int<unsigned int>*)ptr_table.access(registries_def::EBX))->get();

	registers->ebx->set(ebx + value);
}
void b_add32ECX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg_id))->get();
	unsigned int ecx = ((reg_int<unsigned int>*)ptr_table.access(registries_def::ECX))->get();

	registers->ecx->set(ecx + value);
}
void b_add32EDX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg_id))->get();
	unsigned int edx = ((reg_int<unsigned int>*)ptr_table.access(registries_def::EDX))->get();

	registers->edx->set(edx + value);
}

void b_add64RAX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	size_t value = ((reg_int<size_t>*)ptr_table.access(reg_id))->get();
	size_t rax = ((reg_int<size_t>*)ptr_table.access(registries_def::RAX))->get();

	registers->rax->set(rax + value);
}
void b_add64RBX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	size_t value = ((reg_int<size_t>*)ptr_table.access(reg_id))->get();
	size_t rbx = ((reg_int<size_t>*)ptr_table.access(registries_def::RBX))->get();

	registers->rbx->set(rbx + value);
}
void b_add64RCX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	size_t value = ((reg_int<size_t>*)ptr_table.access(reg_id))->get();
	size_t rcx = ((reg_int<size_t>*)ptr_table.access(registries_def::RCX))->get();

	registers->rcx->set(rcx + value);
}
void b_add64RDX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	size_t value = ((reg_int<size_t>*)ptr_table.access(reg_id))->get();
	size_t rdx = ((reg_int<size_t>*)ptr_table.access(registries_def::RDX))->get();

	registers->rdx->set(rdx + value);
}


void b_sub16AX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg_id))->get();
	unsigned short ax = ((reg_int<unsigned short>*)ptr_table.access(registries_def::AX))->get();

	registers->ax->set(ax - value);
}
void b_sub16BX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg_id))->get();
	unsigned short bx = ((reg_int<unsigned short>*)ptr_table.access(registries_def::BX))->get();

	registers->bx->set(bx - value);
}
void b_sub16CX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg_id))->get();
	unsigned short cx = ((reg_int<unsigned short>*)ptr_table.access(registries_def::CX))->get();

	registers->cx->set(cx - value);
}
void b_sub16DX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned short value = ((reg_int<unsigned short>*)ptr_table.access(reg_id))->get();
	unsigned short dx = ((reg_int<unsigned short>*)ptr_table.access(registries_def::DX))->get();

	registers->dx->set(dx - value);
}

void b_sub32EAX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg_id))->get();
	unsigned int eax = ((reg_int<unsigned int>*)ptr_table.access(registries_def::EAX))->get();

	registers->eax->set(eax - value);
}
void b_sub32EBX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg_id))->get();
	unsigned int ebx = ((reg_int<unsigned int>*)ptr_table.access(registries_def::EBX))->get();

	registers->ebx->set(ebx - value);
}
void b_sub32ECX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg_id))->get();
	unsigned int ecx = ((reg_int<unsigned int>*)ptr_table.access(registries_def::ECX))->get();

	registers->ecx->set(ecx - value);
}
void b_sub32EDX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned int value = ((reg_int<unsigned int>*)ptr_table.access(reg_id))->get();
	unsigned int edx = ((reg_int<unsigned int>*)ptr_table.access(registries_def::EDX))->get();

	registers->edx->set(edx - value);
}

void b_sub64RAX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	size_t value = ((reg_int<size_t>*)ptr_table.access(reg_id))->get();
	size_t rax = ((reg_int<size_t>*)ptr_table.access(registries_def::RAX))->get();

	registers->rax->set(rax - value);
}
void b_sub64RBX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	size_t value = ((reg_int<size_t>*)ptr_table.access(reg_id))->get();
	size_t rbx = ((reg_int<size_t>*)ptr_table.access(registries_def::RBX))->get();

	registers->rbx->set(rbx - value);
}
void b_sub64RCX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	size_t value = ((reg_int<size_t>*)ptr_table.access(reg_id))->get();
	size_t rcx = ((reg_int<size_t>*)ptr_table.access(registries_def::RCX))->get();

	registers->rcx->set(rcx - value);
}
void b_sub64RDX(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	size_t value = ((reg_int<size_t>*)ptr_table.access(reg_id))->get();
	size_t rdx = ((reg_int<size_t>*)ptr_table.access(registries_def::RDX))->get();

	registers->rdx->set(rdx - value);
}