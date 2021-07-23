#include <iostream>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>

#include "../utility.h"
#include "../Memory/memory_decl.h"
#include "../Memory/memory_symbols.h"
#include "regs_decl.h"
#include "registers_symbols.h"

void b_set16AX(std::shared_ptr<void> a, regs* registers, memory* mem) {
	b_set_num<unsigned short>(a, registers, mem, registries_def::AX);
}
void b_set16BX(std::shared_ptr<void> a, regs* registers, memory* mem) {
	b_set_num<unsigned short>(a, registers, mem, registries_def::BX);
}
void b_set16CX(std::shared_ptr<void> a, regs* registers, memory* mem) {
	b_set_num<unsigned short>(a, registers, mem, registries_def::CX);
}
void b_set16DX(std::shared_ptr<void> a, regs* registers, memory* mem) {
	b_set_num<unsigned short>(a, registers, mem, registries_def::DX);
}

void b_set32EAX(std::shared_ptr<void> a, regs* registers, memory* mem) {
	b_set_num<unsigned int>(a, registers, mem, registries_def::EAX);
}
void b_set32EBX(std::shared_ptr<void> a, regs* registers, memory* mem) {
	b_set_num<unsigned int>(a, registers, mem, registries_def::EBX);
}
void b_set32ECX(std::shared_ptr<void> a, regs* registers, memory* mem) {
	b_set_num<unsigned int>(a, registers, mem, registries_def::ECX);
}
void b_set32EDX(std::shared_ptr<void> a, regs* registers, memory* mem) {
	b_set_num<unsigned int>(a, registers, mem, registries_def::EDX);
}

void b_set64RAX(std::shared_ptr<void> a, regs* registers, memory* mem) {
	b_set_num<size_t>(a, registers, mem, registries_def::RAX);
}
void b_set64RBX(std::shared_ptr<void> a, regs* registers, memory* mem) {
	b_set_num<size_t>(a, registers, mem, registries_def::RBX);
}
void b_set64RCX(std::shared_ptr<void> a, regs* registers, memory* mem) {
	b_set_num<size_t>(a, registers, mem, registries_def::RCX);
}
void b_set64RDX(std::shared_ptr<void> a, regs* registers, memory* mem) {
	b_set_num<size_t>(a, registers, mem, registries_def::RDX);
}

void b_setSR(std::shared_ptr<void> a, regs* registers, memory* mem) {
	b_set_str(a, registers, mem);
}
void b_setCR(std::shared_ptr<void> a, regs* registers, memory* mem) {
	b_set_chr(a, registers, mem);
}
void b_setDR(std::shared_ptr<void> a, regs* registers, memory* mem) {
	b_set_dbl(a, registers, mem);
}


void b_get16AX(std::shared_ptr<void> receiver, regs* registers, memory* unused_m) {
	*(std::static_pointer_cast<unsigned short>(receiver)) = registers->ax->get();
}
void b_get16BX(std::shared_ptr<void> receiver, regs* registers, memory* unused_m) {
	*(std::static_pointer_cast<unsigned short>(receiver)) = registers->bx->get();
}
void b_get16CX(std::shared_ptr<void> receiver, regs* registers, memory* unused_m) {
	*(std::static_pointer_cast<unsigned short>(receiver)) = registers->cx->get();
}
void b_get16DX(std::shared_ptr<void> receiver, regs* registers, memory* unused_m) {
	*(std::static_pointer_cast<unsigned short>(receiver)) = registers->dx->get();
}

void b_get32EAX(std::shared_ptr<void> receiver, regs* registers, memory* unused_m) {
	*(std::static_pointer_cast<unsigned int>(receiver)) = registers->eax->get();
}
void b_get32EBX(std::shared_ptr<void> receiver, regs* registers, memory* unused_m) {
	*(std::static_pointer_cast<unsigned int>(receiver)) = registers->ebx->get();
}
void b_get32ECX(std::shared_ptr<void> receiver, regs* registers, memory* unused_m) {
	*(std::static_pointer_cast<unsigned int>(receiver)) = registers->ecx->get();
}
void b_get32EDX(std::shared_ptr<void> receiver, regs* registers, memory* unused_m) {
	*(std::static_pointer_cast<unsigned int>(receiver)) = registers->edx->get();
}

void b_get64RAX(std::shared_ptr<void> receiver, regs* registers, memory* unused_m) {
	*(std::static_pointer_cast<size_t>(receiver)) = registers->rax->get();
}
void b_get64RBX(std::shared_ptr<void> receiver, regs* registers, memory* unused_m) {
	*(std::static_pointer_cast<size_t>(receiver)) = registers->rbx->get();
}
void b_get64RCX(std::shared_ptr<void> receiver, regs* registers, memory* unused_m) {
	*(std::static_pointer_cast<size_t>(receiver)) = registers->rcx->get();
}
void b_get64RDX(std::shared_ptr<void> receiver, regs* registers, memory* unused_m) {
	*(std::static_pointer_cast<size_t>(receiver)) = registers->rdx->get();
}

void b_getSR(std::shared_ptr<void> receiver, regs* registers, memory* unused_m) {
	*(std::static_pointer_cast<std::string>(receiver)) = registers->sr->get();
}
void b_getCR(std::shared_ptr<void> receiver, regs* registers, memory* unused_m) {
	*(std::static_pointer_cast<char>(receiver)) = registers->cr->get();
}
void b_getDR(std::shared_ptr<void> receiver, regs* registers, memory* unused_m) {
	*(std::static_pointer_cast<double>(receiver)) = registers->dr->get();
}