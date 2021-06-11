#include <iostream>
#include <memory>
#include <string>

#include "../Memory/memory_decl.h"
#include "../Memory/memory_symbols.h"
#include "../Registers/registers_symbols.h"
#include "process_symbols.h"

std::vector<size_t> calling_tree;

void p_inverseJmpSign(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m) {
	*(registers->jmp_sign) *= -1;
}
void p_jmp(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m) {
	long long count = (size_t)registers->ax->get() * *(registers->jmp_sign);
	*(registers->process_step) += count;
}
void p_cmp(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	size_t saved_rax = registers->rax->get();
	size_t saved_rbx = registers->rax->get();

	popMem(std::make_shared<registries_def>(registries_def::RAX), registers, mem);
	popMem(std::make_shared<registries_def>(registries_def::RBX), registers, mem);

	size_t rax = registers->rax->get();
	size_t rbx = registers->rbx->get();

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

	pushMem(std::make_shared<registries_def>(registries_def::RBX), registers, mem);
	pushMem(std::make_shared<registries_def>(registries_def::RAX), registers, mem);

	registers->rax->set(saved_rax);
	registers->rbx->set(saved_rbx);
}
void p_cmpstr(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
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
void p_je(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m) {
	if (*(registers->cmp_out) == 0) {
		long long count = (size_t)registers->ax->get() * *(registers->jmp_sign);
		*(registers->process_step) += count;
		*(registers->cmp_out) = 0xFF;
	}
}
// Jump if not equal
void p_jne(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m) {
	if (*(registers->cmp_out) != 0) {
		long long count = (size_t)registers->ax->get() * *(registers->jmp_sign);
		*(registers->process_step) += count;
		*(registers->cmp_out) = 0xFF;
	}
}
// Jump if less
void p_jl(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m) {
	if (*(registers->cmp_out) == 1) {
		long long count = (size_t)registers->ax->get() * *(registers->jmp_sign);
		*(registers->process_step) += count;
		*(registers->cmp_out) = 0xFF;
	}
}
// Jump if greater
void p_jg(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m) {
	if (*(registers->cmp_out) == 2) {
		long long count = (size_t)registers->ax->get() * *(registers->jmp_sign);
		*(registers->process_step) += count;
		*(registers->cmp_out) = 0xFF;
	}
}
// Jump if less or equal
void p_jle(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m) {
	if (*(registers->cmp_out) == 0 || *(registers->cmp_out) == 1) {
		long long count = (size_t)registers->ax->get() * *(registers->jmp_sign);
		*(registers->process_step) += count;
		*(registers->cmp_out) = 0xFF;
	}
}
//Jump if greater or equal
void p_jge(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m) {
	if (*(registers->cmp_out) == 0 || *(registers->cmp_out) == 2) {
		long long count = (size_t)registers->ax->get() * *(registers->jmp_sign);
		*(registers->process_step) += count;
		*(registers->cmp_out) = 0xFF;
	}
}

// Get Current working Adress [GCA] (adress of the current action) Put it on stack memory
void p_gca(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	size_t saved_rax = registers->rax->get();

	registers->rax->set(*(registers->process_step));
	pushMem(std::make_shared<registries_def>(registries_def::RAX), registers, mem);

	registers->rax->set(saved_rax);
}

// Exit program
void p_hlt(std::shared_ptr<void> unused_p, regs* registers, memory* unsused_m) {
	*registers->stopRequested = true;
}

// Get immediatly to pointed address (mainly used with tags and procedure)
void p_call(std::shared_ptr<void> value_ptr, regs* registers, memory* unused_m) {
	*registers->process_call_address = *registers->process_step;
	*registers->process_step = *std::static_pointer_cast<size_t>(value_ptr);
}
// Get immediatly to pointed address without saving caller address (mainly used with tags in loops)
void p_lcall(std::shared_ptr<void> value_ptr, regs* registers, memory* unused_m) {
	*registers->process_step = *std::static_pointer_cast<size_t>(value_ptr);
}
// Get back to caller address (should only be used with p_call)
void p_ret(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m) {
	*registers->process_step = *registers->process_call_address;
}

// Saves actual saved caller address into private stack
// Allow programmer to add a depth to calling tree
void p_svcall(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m) {
	calling_tree.push_back(*registers->process_call_address);
}
// Restores last saved caller address from private stack
// Pulls of one depth from calling tree
void p_rscall(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m) {
	if (!calling_tree.empty() || calling_tree.size() > 0) {
		*registers->process_call_address = calling_tree[calling_tree.size() - 1];
		calling_tree.pop_back();
	}
}