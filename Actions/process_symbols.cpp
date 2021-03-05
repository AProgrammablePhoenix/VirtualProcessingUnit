#include <iostream>
#include <string>

#include "../Memory/memory_decl.h"
#include "../Memory/memory_symbols.h"
#include "../Registers/registers_symbols.h"
#include "process_symbols.h"

void p_inverseJmpSign(void* unused_p, regs* registers, memory* unused_m) {
	*(registers->jmp_sign) *= -1;
}
void p_jmp(void* unused_p, regs* registers, memory* unused_m) {
	long long count = (unsigned long long)registers->ax->get() * *(registers->jmp_sign);
	*(registers->process_step) += count;
}
void p_cmp(void* unused_p, regs* registers, memory* mem) {
	unsigned long long saved_rax = registers->rax->get();
	unsigned long long saved_rbx = registers->rax->get();

	popMem(registries_def::RAX, registers, mem);
	popMem(registries_def::RBX, registers, mem);

	unsigned long long rax = registers->rax->get();
	unsigned long long rbx = registers->rbx->get();

	if (rax == rbx) {
		*(registers->cmp_out) = 0;
	}
	else if (rax < rbx) {
		*(registers->cmp_out) = 1;
	}
	else if (rax > rbx) {
		*(registers->cmp_out) = 2;
	}
	else {
		// Undefined behavior: unknown comparison
		*(registers->cmp_out) = 0xFE;
	}

	pushMem(registries_def::RBX, registers, mem);
	pushMem(registries_def::RAX, registers, mem);

	registers->rax->set(saved_rax);
	registers->rbx->set(saved_rbx);
}
void p_cmpstr(void* unused_p, regs* registers, memory* mem) {
	std::string saved_sr = registers->sr->get();
	std::string s1, s2;

	popMemSR(unused_p, registers, mem);
	s1 = registers->sr->get();
	popMemSR(unused_p, registers, mem);
	s2 = registers->sr->get();

	if (s1 == s2) {
		*(registers->cmp_out) = 0;
	}
	else if (s1 < s2) {
		*(registers->cmp_out) = 1;
	}
	else if (s1 > s2) {
		*(registers->cmp_out) = 2;
	}
	else {
		// Undefined behavior: unknown comparison
		*(registers->cmp_out) = 0xFE;
	}

	pushMemSR(unused_p, registers, mem);
	registers->sr->set(s1);
	pushMemSR(unused_p, registers, mem);

	registers->sr->set(saved_sr);
}

// Jump if equal
void p_je(void* unused_p, regs* registers, memory* unused_m) {
	if (*(registers->cmp_out) == 0) {
		long long count = (unsigned long long)registers->ax->get() * *(registers->jmp_sign);
		*(registers->process_step) += count;
		*(registers->cmp_out) = 0xFF;
	}
}
// Jump if not equal
void p_jne(void* unused_p, regs* registers, memory* unused_m) {
	if (*(registers->cmp_out) != 0) {
		long long count = (unsigned long long)registers->ax->get() * *(registers->jmp_sign);
		*(registers->process_step) += count;
		*(registers->cmp_out) = 0xFF;
	}
}
// Jump if less
void p_jl(void* unused_p, regs* registers, memory* unused_m) {
	if (*(registers->cmp_out) == 1) {
		long long count = (unsigned long long)registers->ax->get() * *(registers->jmp_sign);
		*(registers->process_step) += count;
		*(registers->cmp_out) = 0xFF;
	}
}
// Jump if greater
void p_jg(void* unused_p, regs* registers, memory* unused_m) {
	if (*(registers->cmp_out) == 2) {
		long long count = (unsigned long long)registers->ax->get() * *(registers->jmp_sign);
		*(registers->process_step) += count;
		*(registers->cmp_out) = 0xFF;
	}
}
// Jump if less or equal
void p_jle(void* unused_p, regs* registers, memory* unused_m) {
	if (*(registers->cmp_out) == 0 || *(registers->cmp_out) == 1) {
		long long count = (unsigned long long)registers->ax->get() * *(registers->jmp_sign);
		*(registers->process_step) += count;
		*(registers->cmp_out) = 0xFF;
	}
}
//Jump if greater or equal
void p_jge(void* unused_p, regs* registers, memory* unused_m) {
	if (*(registers->cmp_out) == 0 || *(registers->cmp_out) == 2) {
		long long count = (unsigned long long)registers->ax->get() * *(registers->jmp_sign);
		*(registers->process_step) += count;
		*(registers->cmp_out) = 0xFF;
	}
}

// Get Current working Adress [GCA] (adress of the current action) Put it on stack memory
void p_gca(void* unused_p, regs* registers, memory* mem) {
	unsigned long long saved_rax = registers->rax->get();

	registers->rax->set(*(registers->process_step));
	pushMem(registries_def::RAX, registers, mem);

	registers->rax->set(saved_rax);
}

// Exit program
void p_hlt(void* unused_p, regs* registers, memory* unsused_m) {
	*registers->stopRequested = true;
}

// Get immediatly to pointed address (mainly used with tags and procedure)
void p_call(void* value_ptr, regs* registers, memory* unused_m) {
	*registers->process_call_address = *registers->process_step;
	*registers->process_step = *(unsigned long long*)value_ptr;
}
// Get immediatly to pointed address without saving caller address (mainly used with tags in loops)
void p_lcall(void* value_ptr, regs* registers, memory* unused_m) {
	*registers->process_step = *(unsigned long long*)value_ptr;
}
// Get back to caller address (should only be used with p_call)
void p_ret(void* unused_p, regs* registers, memory* unused_m) {
	*registers->process_step = *registers->process_call_address;
}