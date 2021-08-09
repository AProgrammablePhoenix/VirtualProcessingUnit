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

#ifndef REGS_PREPROC
	#define GLOBL_ARGS CUSTOM_STD_ARGS(a, registers, mem)

	#define FPR_DEF(io_type, fpr_id) 										\
		void b_##io_type##FPR##fpr_id(GLOBL_ARGS) { 						\
			bool exitcode = false; 											\
			float f = (float)b_fetch_dbl_args(a, registers, mem, exitcode); \
			if (!exitcode) { 												\
				*registers->fpr##fpr_id = f; 								\
			}																\
		}
	#define EFPR_DEF(io_type, efpr_id) 											\
		void b_##io_type##EFPR##efpr_id(GLOBL_ARGS) { 							\
			bool exitcode = false;												\
			double d = (double)b_fetch_dbl_args(a, registers, mem, exitcode);	\
			if (!exitcode) {													\
				*registers->efpr##efpr_id = d; 									\
			}																	\
		}
	#define RFPR_DEF(io_type, rfpr_id) 											\
		void b_##io_type##RFPR##rfpr_id(GLOBL_ARGS) { 							\
			bool exitcode = false; 												\
			long double ld = b_fetch_dbl_args(a, registers, mem, exitcode);		\
			if (!exitcode) {													\
				*registers->rfpr##rfpr_id = ld; 								\
			} 																	\
		}	
	#define DEF_ALL_FPRs() 	\
		FPR_DEF(set, 0) 	\
		FPR_DEF(set, 1) 	\
		FPR_DEF(set, 2) 	\
		FPR_DEF(set, 3)
	#define DEF_ALL_EFPRs() \
		EFPR_DEF(set, 0) 	\
		EFPR_DEF(set, 1) 	\
		EFPR_DEF(set, 2) 	\
		EFPR_DEF(set, 3)
	#define DEF_ALL_RFPRs() \
		RFPR_DEF(set, 0) 	\
		RFPR_DEF(set, 1)	\
		RFPR_DEF(set, 2) 	\
		RFPR_DEF(set, 3)
	#define DEF_ALL_FPRegs 	\
		DEF_ALL_FPRs() 		\
		DEF_ALL_EFPRs()		\
		DEF_ALL_RFPRs() 

	#define REGS_PREPROC(...) (void)0
#endif

void b_set16AX(GLOBL_ARGS) {
	b_set_num<unsigned short>(a, registers, mem, registries_def::AX);
}
void b_set16BX(GLOBL_ARGS) {
	b_set_num<unsigned short>(a, registers, mem, registries_def::BX);
}
void b_set16CX(GLOBL_ARGS) {
	b_set_num<unsigned short>(a, registers, mem, registries_def::CX);
}
void b_set16DX(GLOBL_ARGS) {
	b_set_num<unsigned short>(a, registers, mem, registries_def::DX);
}

void b_set32EAX(GLOBL_ARGS) {
	b_set_num<unsigned int>(a, registers, mem, registries_def::EAX);
}
void b_set32EBX(GLOBL_ARGS) {
	b_set_num<unsigned int>(a, registers, mem, registries_def::EBX);
}
void b_set32ECX(GLOBL_ARGS) {
	b_set_num<unsigned int>(a, registers, mem, registries_def::ECX);
}
void b_set32EDX(GLOBL_ARGS) {
	b_set_num<unsigned int>(a, registers, mem, registries_def::EDX);
}

void b_set64RAX(GLOBL_ARGS) {
	b_set_num<size_t>(a, registers, mem, registries_def::RAX);
}
void b_set64RBX(GLOBL_ARGS) {
	b_set_num<size_t>(a, registers, mem, registries_def::RBX);
}
void b_set64RCX(GLOBL_ARGS) {
	b_set_num<size_t>(a, registers, mem, registries_def::RCX);
}
void b_set64RDX(GLOBL_ARGS) {
	b_set_num<size_t>(a, registers, mem, registries_def::RDX);
}

void b_setSR(GLOBL_ARGS) {
	b_set_str(a, registers, mem);
}
void b_setCR(GLOBL_ARGS) {
	b_set_chr(a, registers, mem);
}
void b_setDR(GLOBL_ARGS) {
	b_set_dbl(a, registers, mem);
}

DEF_ALL_FPRegs;


// get functions are deprecated and will be soon totally removed from codebase
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