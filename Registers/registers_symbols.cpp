#include <iostream>
#include <memory>
#include <string>
#include <tuple>

#include "../utility.h"
#include "../Memory/memory_decl.h"
#include "../Memory/memory_symbols.h"
#include "regs_decl.h"
#include "registers_symbols.h"

void b_set16AX(std::shared_ptr<void> a, regs* registers, memory* mem) {
	std::tuple<size_t, size_t> varinfos = *std::static_pointer_cast<std::tuple<size_t, size_t>>(a);
	unsigned char* uc_n = new unsigned char[sizeof(size_t)];
	mem->_ROZVG(uc_n, sizeof(size_t), std::get<0>(varinfos));

	registers->ax->set((unsigned short)ATOULL(uc_n));
	delete[] uc_n;
}
void b_set16BX(std::shared_ptr<void> a, regs* registers, memory* mem) {
	std::tuple<size_t, size_t> varinfos = *std::static_pointer_cast<std::tuple<size_t, size_t>>(a);
	unsigned char* uc_n = new unsigned char[sizeof(size_t)];
	mem->_ROZVG(uc_n, sizeof(size_t), std::get<0>(varinfos));

	registers->bx->set((unsigned short)ATOULL(uc_n));
	delete[] uc_n;
}
void b_set16CX(std::shared_ptr<void> a, regs* registers, memory* mem) {
	std::tuple<size_t, size_t> varinfos = *std::static_pointer_cast<std::tuple<size_t, size_t>>(a);
	unsigned char* uc_n = new unsigned char[sizeof(size_t)];
	mem->_ROZVG(uc_n, sizeof(size_t), std::get<0>(varinfos));

	registers->cx->set((unsigned short)ATOULL(uc_n));
	delete[] uc_n;
}
void b_set16DX(std::shared_ptr<void> a, regs* registers, memory* mem) {
	std::tuple<size_t, size_t> varinfos = *std::static_pointer_cast<std::tuple<size_t, size_t>>(a);
	unsigned char* uc_n = new unsigned char[sizeof(size_t)];
	mem->_ROZVG(uc_n, sizeof(size_t), std::get<0>(varinfos));

	registers->dx->set((unsigned short)ATOULL(uc_n));
	delete[] uc_n;
}

void b_set32EAX(std::shared_ptr<void> a, regs* registers, memory* mem) {
	std::tuple<size_t, size_t> varinfos = *std::static_pointer_cast<std::tuple<size_t, size_t>>(a);
	unsigned char* uc_n = new unsigned char[sizeof(size_t)];
	mem->_ROZVG(uc_n, sizeof(size_t), std::get<0>(varinfos));

	registers->eax->set((unsigned int)ATOULL(uc_n));
	delete[] uc_n;
}
void b_set32EBX(std::shared_ptr<void> a, regs* registers, memory* mem) {
	std::tuple<size_t, size_t> varinfos = *std::static_pointer_cast<std::tuple<size_t, size_t>>(a);
	unsigned char* uc_n = new unsigned char[sizeof(size_t)];
	mem->_ROZVG(uc_n, sizeof(size_t), std::get<0>(varinfos));

	registers->ebx->set((unsigned int)ATOULL(uc_n));
	delete[] uc_n;
}
void b_set32ECX(std::shared_ptr<void> a, regs* registers, memory* mem) {
	std::tuple<size_t, size_t> varinfos = *std::static_pointer_cast<std::tuple<size_t, size_t>>(a);
	unsigned char* uc_n = new unsigned char[sizeof(size_t)];
	mem->_ROZVG(uc_n, sizeof(size_t), std::get<0>(varinfos));

	registers->ecx->set((unsigned int)ATOULL(uc_n));
	delete[] uc_n;
}
void b_set32EDX(std::shared_ptr<void> a, regs* registers, memory* mem) {
	std::tuple<size_t, size_t> varinfos = *std::static_pointer_cast<std::tuple<size_t, size_t>>(a);
	unsigned char* uc_n = new unsigned char[sizeof(size_t)];
	mem->_ROZVG(uc_n, sizeof(size_t), std::get<0>(varinfos));

	registers->edx->set((unsigned int)ATOULL(uc_n));
	delete[] uc_n;
}

void b_set64RAX(std::shared_ptr<void> a, regs* registers, memory* mem) {
	std::tuple<size_t, size_t> varinfos = *std::static_pointer_cast<std::tuple<size_t, size_t>>(a);
	unsigned char* uc_n = new unsigned char[sizeof(size_t)];
	mem->_ROZVG(uc_n, sizeof(size_t), std::get<0>(varinfos));

	registers->rax->set(ATOULL(uc_n));
	delete[] uc_n;
}
void b_set64RBX(std::shared_ptr<void> a, regs* registers, memory* mem) {
	std::tuple<size_t, size_t> varinfos = *std::static_pointer_cast<std::tuple<size_t, size_t>>(a);
	unsigned char* uc_n = new unsigned char[sizeof(size_t)];
	mem->_ROZVG(uc_n, sizeof(size_t), std::get<0>(varinfos));

	registers->rbx->set(ATOULL(uc_n));
	delete[] uc_n;
}
void b_set64RCX(std::shared_ptr<void> a, regs* registers, memory* mem) {
	std::tuple<size_t, size_t> varinfos = *std::static_pointer_cast<std::tuple<size_t, size_t>>(a);
	unsigned char* uc_n = new unsigned char[sizeof(size_t)];
	mem->_ROZVG(uc_n, sizeof(size_t), std::get<0>(varinfos));

	registers->rcx->set(ATOULL(uc_n));
	delete[] uc_n;
}
void b_set64RDX(std::shared_ptr<void> a, regs* registers, memory* mem) {
	std::tuple<size_t, size_t> varinfos = *std::static_pointer_cast<std::tuple<size_t, size_t>>(a);
	unsigned char* uc_n = new unsigned char[sizeof(size_t)];
	mem->_ROZVG(uc_n, sizeof(size_t), std::get<0>(varinfos));

	registers->rdx->set(ATOULL(uc_n));
	delete[] uc_n;
}

void b_setSR(std::shared_ptr<void> a, regs* registers, memory* mem) {
	std::tuple<size_t, size_t> varinfos = *std::static_pointer_cast<std::tuple<size_t, size_t>>(a);
	size_t str_size = std::get<1>(varinfos);

	unsigned char* uc_s = new unsigned char[str_size];
	mem->_ROZVG(uc_s, str_size, std::get<0>(varinfos));

	registers->sr->set(std::string((const char*)uc_s));
	delete[] uc_s;
}
void b_setCR(std::shared_ptr<void> a, regs* registers, memory* mem) {
	std::tuple<size_t, size_t> varinfos = *std::static_pointer_cast<std::tuple<size_t, size_t>>(a);
	unsigned char* uc_c = new unsigned char[1];
	mem->_ROZVG(uc_c, 1, std::get<0>(varinfos));

	registers->cr->set((char)uc_c[0]);
	delete[] uc_c;
}
void b_setDR(std::shared_ptr<void> a, regs* registers, memory* mem) {
	std::tuple<size_t, size_t> varinfos = *std::static_pointer_cast<std::tuple<size_t, size_t>>(a);
	unsigned char* uc_d = new unsigned char[sizeof(double)];
	mem->_ROZVG(uc_d, sizeof(double), std::get<0>(varinfos));

	registers->dr->set(ATOD(uc_d));
	delete[] uc_d;
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