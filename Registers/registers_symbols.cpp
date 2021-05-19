#include <iostream>
#include <memory>
#include <string>

#include "../Memory/memory_decl.h"
#include "../Memory/memory_symbols.h"
#include "regs_decl.h"
#include "registers_symbols.h"

void b_set16AX(std::shared_ptr<void> a, regs* regsiters, memory* unsued_m) {
	regsiters->ax->set(*std::static_pointer_cast<unsigned short>(a));
}
void b_set16BX(std::shared_ptr<void> a, regs* registers, memory* unsued_m) {
	registers->bx->set(*std::static_pointer_cast<unsigned short>(a));
}
void b_set16CX(std::shared_ptr<void> a, regs* registers, memory* unsued_m) {
	registers->cx->set(*std::static_pointer_cast<unsigned short>(a));
}
void b_set16DX(std::shared_ptr<void> a, regs* registers, memory* unsued_m) {
	registers->dx->set(*std::static_pointer_cast<unsigned short>(a));
}

void b_set32EAX(std::shared_ptr<void> a, regs* registers, memory* unsued_m) {
	registers->eax->set(*std::static_pointer_cast<unsigned int>(a));
}
void b_set32EBX(std::shared_ptr<void> a, regs* registers, memory* unsued_m) {
	registers->ebx->set(*std::static_pointer_cast<unsigned int>(a));
}
void b_set32ECX(std::shared_ptr<void> a, regs* registers, memory* unsued_m) {
	registers->ecx->set(*std::static_pointer_cast<unsigned int>(a));
}
void b_set32EDX(std::shared_ptr<void> a, regs* registers, memory* unsued_m) {
	registers->edx->set(*std::static_pointer_cast<unsigned int>(a));
}

void b_set64RAX(std::shared_ptr<void> a, regs* registers, memory* unsued_m) {
	registers->rax->set(*std::static_pointer_cast<unsigned long long>(a));
}
void b_set64RBX(std::shared_ptr<void> a, regs* registers, memory* unsued_m) {
	registers->rbx->set(*std::static_pointer_cast<unsigned long long>(a));
}
void b_set64RCX(std::shared_ptr<void> a, regs* registers, memory* unsued_m) {
	registers->rcx->set(*std::static_pointer_cast<unsigned long long>(a));
}
void b_set64RDX(std::shared_ptr<void> a, regs* registers, memory* unsued_m) {
	registers->rdx->set(*std::static_pointer_cast<unsigned long long>(a));
}

void b_setSR(std::shared_ptr<void> a, regs* registers, memory* unsued_m) {
	registers->sr->set(*std::static_pointer_cast<std::string>(a));
}
void b_setCR(std::shared_ptr<void> a, regs* registers, memory* unused_m) {
	registers->cr->set(*std::static_pointer_cast<char>(a));

}
void b_setDR(std::shared_ptr<void> a, regs* registers, memory* unused_m) {
	registers->dr->set(*std::static_pointer_cast<double>(a));
}


void b_get16AX(std::shared_ptr<void> receiver, regs* registers, memory* unsued_m) {
	*(std::static_pointer_cast<unsigned short>(receiver)) = registers->ax->get();
}
void b_get16BX(std::shared_ptr<void> receiver, regs* registers, memory* unsued_m) {
	*(std::static_pointer_cast<unsigned short>(receiver)) = registers->bx->get();
}
void b_get16CX(std::shared_ptr<void> receiver, regs* registers, memory* unsued_m) {
	*(std::static_pointer_cast<unsigned short>(receiver)) = registers->cx->get();
}
void b_get16DX(std::shared_ptr<void> receiver, regs* registers, memory* unsued_m) {
	*(std::static_pointer_cast<unsigned short>(receiver)) = registers->dx->get();
}

void b_get32EAX(std::shared_ptr<void> receiver, regs* registers, memory* unsued_m) {
	*(std::static_pointer_cast<unsigned int>(receiver)) = registers->eax->get();
}
void b_get32EBX(std::shared_ptr<void> receiver, regs* registers, memory* unsued_m) {
	*(std::static_pointer_cast<unsigned int>(receiver)) = registers->ebx->get();
}
void b_get32ECX(std::shared_ptr<void> receiver, regs* registers, memory* unsued_m) {
	*(std::static_pointer_cast<unsigned int>(receiver)) = registers->ecx->get();
}
void b_get32EDX(std::shared_ptr<void> receiver, regs* registers, memory* unsued_m) {
	*(std::static_pointer_cast<unsigned int>(receiver)) = registers->edx->get();
}

void b_get64RAX(std::shared_ptr<void> receiver, regs* registers, memory* unsued_m) {
	*(std::static_pointer_cast<unsigned long long>(receiver)) = registers->rax->get();
}
void b_get64RBX(std::shared_ptr<void> receiver, regs* registers, memory* unsued_m) {
	*(std::static_pointer_cast<unsigned long long>(receiver)) = registers->rbx->get();
}
void b_get64RCX(std::shared_ptr<void> receiver, regs* registers, memory* unsued_m) {
	*(std::static_pointer_cast<unsigned long long>(receiver)) = registers->rcx->get();
}
void b_get64RDX(std::shared_ptr<void> receiver, regs* registers, memory* unsued_m) {
	*(std::static_pointer_cast<unsigned long long>(receiver)) = registers->rdx->get();
}

void b_getSR(std::shared_ptr<void> receiver, regs* registers, memory* unsued_m) {
	*(std::static_pointer_cast<std::string>(receiver)) = registers->sr->get();
}
void b_getCR(std::shared_ptr<void> receiver, regs* registers, memory* unused_m) {
	*(std::static_pointer_cast<char>(receiver)) = registers->cr->get();
}
void b_getDR(std::shared_ptr<void> receiver, regs* registers, memory* unused_m) {
	*(std::static_pointer_cast<double>(receiver)) = registers->dr->get();
}