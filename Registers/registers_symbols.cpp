#include <iostream>
#include <string>

#include "../Memory/memory_decl.h"
#include "../Memory/memory_symbols.h"
#include "registers_symbols.h"
#include "regs_decl.h"

void b_set16AX(unsigned short *a, regs* regsiters, memory* unsued_m) {
	regsiters->ax->set(*a);
}
void b_set16BX(unsigned short *a, regs* registers, memory* unsued_m) {
	registers->bx->set(*a);
}
void b_set16CX(unsigned short *a, regs* registers, memory* unsued_m) {
	registers->cx->set(*a);
}
void b_set16DX(unsigned short *a, regs* registers, memory* unsued_m) {
	registers->dx->set(*a);
}

void b_set32EAX(unsigned int *a, regs* registers, memory* unsued_m) {
	registers->eax->set(*a);
}
void b_set32EBX(unsigned int *a, regs* registers, memory* unsued_m) {
	registers->ebx->set(*a);
}
void b_set32ECX(unsigned int *a, regs* registers, memory* unsued_m) {
	registers->ecx->set(*a);
}
void b_set32EDX(unsigned int *a, regs* registers, memory* unsued_m) {
	registers->edx->set(*a);
}

void b_set64RAX(unsigned long long *a, regs* registers, memory* unsued_m) {
	registers->rax->set(*a);
}
void b_set64RBX(unsigned long long *a, regs* registers, memory* unsued_m) {
	registers->rbx->set(*a);
}
void b_set64RCX(unsigned long long *a, regs* registers, memory* unsued_m) {
	registers->rcx->set(*a);
}
void b_set64RDX(unsigned long long *a, regs* registers, memory* unsued_m) {
	registers->rdx->set(*a);
}

void b_setSR(std::string* a, regs* registers, memory* unsued_m) {
	registers->sr->set(*a);
}


void b_get16AX(void* receiver, regs* registers, memory* unsued_m) {
	*((unsigned short*)receiver) = registers->ax->get();
}
void b_get16BX(void* receiver, regs* registers, memory* unsued_m) {
	*((unsigned short*)receiver) = registers->bx->get();
}
void b_get16CX(void* receiver, regs* registers, memory* unsued_m) {
	*((unsigned short*)receiver) = registers->cx->get();
}
void b_get16DX(void* receiver, regs* registers, memory* unsued_m) {
	*((unsigned short*)receiver) = registers->dx->get();
}

void b_get32EAX(void* receiver, regs* registers, memory* unsued_m) {
	*((unsigned int*)receiver) = registers->eax->get();
}
void b_get32EBX(void* receiver, regs* registers, memory* unsued_m) {
	*((unsigned int*)receiver) = registers->ebx->get();
}
void b_get32ECX(void* receiver, regs* registers, memory* unsued_m) {
	*((unsigned int*)receiver) = registers->ecx->get();
}
void b_get32EDX(void* receiver, regs* registers, memory* unsued_m) {
	*((unsigned int*)receiver) = registers->edx->get();
}

void b_get64RAX(void* receiver, regs* registers, memory* unsued_m) {
	*((unsigned long long*)receiver) = registers->rax->get();
}
void b_get64RBX(void* receiver, regs* registers, memory* unsued_m) {
	*((unsigned long long*)receiver) = registers->rbx->get();
}
void b_get64RCX(void* receiver, regs* registers, memory* unsued_m) {
	*((unsigned long long*)receiver) = registers->rcx->get();
}
void b_get64RDX(void* receiver, regs* registers, memory* unsued_m) {
	*((unsigned long long*)receiver) = registers->rdx->get();
}

void b_getSR(void* receiver, regs* registers, memory* unsued_m) {
	*((std::string*)receiver) = registers->sr->get();
}