#include <iostream>
#include <memory>
#include <string>
#include <tuple>

#include "../utility.h"
#include "../Memory/memory_decl.h"
#include "../Memory/memory_symbols.h"
#include "../Registers/registers_symbols.h"
#include "process_symbols.h"

namespace {
	inline size_t ARGTOULL(const std::shared_ptr<void>& args, memory* const mem) {
		unsigned char* uc_n = nullptr;
		try {
			const auto [vaddr, vsize, vopt] = *std::static_pointer_cast<arg_tuple>(args);
			uc_n = new unsigned char[sizeof(size_t)];
			mem->_MG(uc_n, sizeof(size_t), vaddr);

			size_t _value = ATOULL(uc_n);
			delete[] uc_n;

			return _value;
		}
		catch (...) {
			if (uc_n)
				delete[] uc_n;
			return 0;
		}
	}
}

std::vector<size_t> calling_tree;

void p_jmp(std::shared_ptr<void> args_p, regs* registers, memory* mem) {
	*(registers->process_step) += ARGTOULL(args_p, mem);
}
void p_cmp(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	size_t c1, c2;

	unsigned char* temp = new unsigned char[sizeof(size_t)];
	mem->pop(temp, sizeof(size_t));
	mp_memcpy(temp, &c1);

	mem->pop(temp, sizeof(size_t));
	mp_memcpy(temp, &c2);

	if (c1 == c2) {
		*(registers->cmp_out) = 0;
	}
	else if (c1 < c2) {
		*(registers->cmp_out) = 1;
	}
	else if (c1 > c2) {
		*(registers->cmp_out) = 2;
	}
	else {
		// Undefined behavior: unknown comparison
		*(registers->cmp_out) = 0xFE;
	}

	delete[] temp;
}
void p_cmpdbl(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	long double d1, d2;

	auto temp = std::make_unique<unsigned char[]>(sizeof(long double));
	mem->pop(temp.get(), sizeof(long double));
	mp_memcpy(temp.get(), &d1, sizeof(long double));

	mem->pop(temp.get(), sizeof(long double));
	mp_memcpy(temp.get(), &d2, sizeof(long double));

	if (d1 == d2)
		*(registers->cmp_out) = 0;
	else if (d1 < d2)
		*(registers->cmp_out) = 1;
	else if (d1 > d2)
		*(registers->cmp_out) = 2;
	else
		*(registers->cmp_out) = 0xFE; // Unknown comparison
}

// Jump if equal
void p_je(std::shared_ptr<void> args_p, regs* registers, memory* mem) {
	size_t move = ARGTOULL(args_p, mem);

	if (*(registers->cmp_out) == 0) {
		*(registers->process_step) += move;
		*(registers->cmp_out) = 0xFF;
	}
}
// Jump if not equal
void p_jne(std::shared_ptr<void> args_p, regs* registers, memory* mem) {
	size_t move = ARGTOULL(args_p, mem);

	if (*(registers->cmp_out) != 0) {
		*(registers->process_step) += move;
		*(registers->cmp_out) = 0xFF;
	}
}
// Jump if less
void p_jl(std::shared_ptr<void> args_p, regs* registers, memory* mem) {
	size_t move = ARGTOULL(args_p, mem);

	if (*(registers->cmp_out) == 1) {
		*(registers->process_step) += move;
		*(registers->cmp_out) = 0xFF;
	}
}
// Jump if greater
void p_jg(std::shared_ptr<void> args_p, regs* registers, memory* mem) {
	size_t move = ARGTOULL(args_p, mem);

	if (*(registers->cmp_out) == 2) {
		*(registers->process_step) += move;
		*(registers->cmp_out) = 0xFF;
	}
}
// Jump if less or equal
void p_jle(std::shared_ptr<void> args_p, regs* registers, memory* mem) {
	size_t move = ARGTOULL(args_p, mem);

	if (*(registers->cmp_out) == 0 || *(registers->cmp_out) == 1) {
		*(registers->process_step) += move;
		*(registers->cmp_out) = 0xFF;
	}
}
//Jump if greater or equal
void p_jge(std::shared_ptr<void> args_p, regs* registers, memory* mem) {
	size_t move = ARGTOULL(args_p, mem);

	if (*(registers->cmp_out) == 0 || *(registers->cmp_out) == 2) {
		*(registers->process_step) += move;
		*(registers->cmp_out) = 0xFF;
	}
}

// Will be soon removed, since this instruction is deprecated due to presence of labels (A RIP register may still be added in the future)
// Get Current working Adress [GCA] (adress of the current action) Put it on stack memory
void p_gca(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	if (!registers->exec_level) {
		size_t c_addr = *(registers->process_step);
		unsigned char* temp = new unsigned char[sizeof(size_t)];
		mp_memcpy(&c_addr, temp);
		mem->push(temp, sizeof(size_t));
	}
}

// Exit program
void p_hlt(std::shared_ptr<void> unused_p, regs* registers, memory* unsused_m) {
	if (!registers->exec_level)
		*registers->stopRequested = true;
}

// Get immediatly to pointed address (mainly used with tags and procedures)
void p_call(std::shared_ptr<void> value_ptr, regs* registers, memory* mem) {
	const auto [vaddr, vsize, vopt] = *std::static_pointer_cast<arg_tuple>(value_ptr);
	unsigned char* uc_n = new unsigned char[sizeof(size_t)];
	mem->_MG(uc_n, sizeof(size_t), vaddr);

	*registers->process_call_address = *registers->process_step;
	*registers->process_step = ATOULL(uc_n);

	delete[] uc_n;
}
// Get immediatly to pointed address without saving caller address (mainly used with tags in loops)
void p_lcall(std::shared_ptr<void> value_ptr, regs* registers, memory* mem) {
	const auto [vaddr, vsize, vopt] = *std::static_pointer_cast<arg_tuple>(value_ptr);
	unsigned char* uc_n = new unsigned char[sizeof(size_t)];
	mem->_MG(uc_n, sizeof(size_t), vaddr);

	*registers->process_step = ATOULL(uc_n);
	delete[] uc_n;
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

// Enable interrupts (sets IF to 1)
void p_sti(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m) {
	if (!registers->exec_level)
		registers->IF = 1;
}
// Disable interrupts (set IF to 0)
void p_cli(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m) {
	if (!registers->exec_level)
		registers->IF = 0;
}