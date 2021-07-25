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
			const auto [vaddr, vsize] = *std::static_pointer_cast<std::tuple<size_t, size_t>>(args);
			uc_n = new unsigned char[sizeof(size_t)];
			mem->_ROZVG(uc_n, sizeof(size_t), vaddr);

			size_t _value = 0;
			mp_memcpy(uc_n, &_value);
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

void p_jmp(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m) {
	*(registers->process_step) += registers->rax->get();
}
void p_cmp(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	size_t rax = registers->rax->get();
	size_t rbx = registers->rbx->get();

	unsigned char* temp = new unsigned char[sizeof(size_t)];
	mem->pop(temp, sizeof(size_t)); // pop rax
	mp_memcpy(temp, &rax);

	mem->pop(temp, sizeof(size_t)); // pop rbx
	mp_memcpy(temp, &rbx);

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

	delete[] temp;
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

	registers->sr->set(saved_sr);
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

// Get Current working Adress [GCA] (adress of the current action) Put it on stack memory
void p_gca(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	size_t c_addr = *(registers->process_step);
	unsigned char* temp = new unsigned char[sizeof(size_t)];
	mp_memcpy(&c_addr, temp);
	mem->push(temp, sizeof(size_t));
}

// Exit program
void p_hlt(std::shared_ptr<void> unused_p, regs* registers, memory* unsused_m) {
	*registers->stopRequested = true;
}

// Get immediatly to pointed address (mainly used with tags and procedures)
void p_call(std::shared_ptr<void> value_ptr, regs* registers, memory* mem) {
	std::tuple<size_t, size_t> varinfos = *std::static_pointer_cast<std::tuple<size_t, size_t>>(value_ptr);
	unsigned char* uc_n = new unsigned char[sizeof(size_t)];
	mem->_ROZVG(uc_n, sizeof(size_t), std::get<0>(varinfos));

	*registers->process_call_address = *registers->process_step;
	*registers->process_step = ATOULL(uc_n);

	delete[] uc_n;
}
// Get immediatly to pointed address without saving caller address (mainly used with tags in loops)
void p_lcall(std::shared_ptr<void> value_ptr, regs* registers, memory* mem) {
	std::tuple<size_t, size_t> varinfos = *std::static_pointer_cast<std::tuple<size_t, size_t>>(value_ptr);
	unsigned char* uc_n = new unsigned char[sizeof(size_t)];
	mem->_ROZVG(uc_n, sizeof(size_t), std::get<0>(varinfos));

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

// Start (or resumes a stopped thread) a loaded thread by its id
void p_crtthread(std::shared_ptr<void> thread_id_ptr, regs* registers, memory* mem) {
	arg_tuple varinfos = *std::static_pointer_cast<arg_tuple>(thread_id_ptr);
	unsigned char* temp = new unsigned char[sizeof(size_t)];
	mem->_ROZVG(temp, sizeof(size_t), std::get<0>(varinfos));

	size_t thread_id = ATOULL(temp);
	delete[] temp;

	if (!registers->threadsStatuses->count(thread_id))
		return;
	if ((*registers->threadsStatuses)[thread_id] == THREAD_DEAD)
		return;

	(*registers->threadsStatuses)[thread_id] = THREAD_ALIVE;
}
// Pause a loaded and launched thread by its id
void p_rstthread(std::shared_ptr<void> thread_id_ptr, regs* registers, memory* mem) {
	arg_tuple varinfos = *std::static_pointer_cast<arg_tuple>(thread_id_ptr);
	unsigned char* temp = new unsigned char[sizeof(size_t)];
	mem->_ROZVG(temp, sizeof(size_t), std::get<0>(varinfos));

	size_t thread_id = ATOULL(temp);
	delete[] temp;

	if (!registers->threadsStatuses->count(thread_id))
		return;
	if ((*registers->threadsStatuses)[thread_id] == THREAD_DEAD)
		return;

	(*registers->threadsStatuses)[thread_id] = THREAD_STOPPED;
}
// Terminates a loaded and launched thread by its id
void p_endthread(std::shared_ptr<void> thread_id_ptr, regs* registers, memory* mem) {
	arg_tuple varinfos = *std::static_pointer_cast<arg_tuple>(thread_id_ptr);
	unsigned char* temp = new unsigned char[sizeof(size_t)];
	mem->_ROZVG(temp, sizeof(size_t), std::get<0>(varinfos));

	size_t thread_id = ATOULL(temp);
	delete[] temp;

	if (!registers->threadsStatuses->count(thread_id))
		return;
	if ((*registers->threadsStatuses)[thread_id] == THREAD_STOPPED)
		return;

	(*registers->threadsStatuses)[thread_id] = THREAD_SIGTERM;
}