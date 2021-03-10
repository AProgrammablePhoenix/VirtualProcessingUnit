#include <iostream>
#include <string>
#include <sstream>

#include "registers_symbols.h"
#include "../Memory/memory_symbols.h"

void b_getInput(void* unused_p, regs* registers, memory* mem) {
	std::string saved_sr = registers->sr->get(), input;

	std::getline(std::cin, input);
	registers->sr->set(input);

	pushMemSR(unused_p, registers, mem);
	registers->sr->set(saved_sr);
}
void b_toString(registries_def reg, regs* registers, memory* unused_m) {
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned long long value = ((reg_int<unsigned long long>*)ptr_table.access(reg))->get();

	std::stringstream ss;
	ss << value;

	std::string converted = ss.str();
	b_setSR(&converted, registers, unused_m);
}
void b_mergeString(void* unused_p, regs* registers, memory* mem) {
	std::string pushed = registers->sr->get();
	popMemSR(NULL, registers, mem);
	std::string base = registers->sr->get();

	registers->sr->set(base + pushed);
}
void b_substring(void* unused_p, regs* registers, memory* unused_m) {
	std::string sr = registers->sr->get();
	unsigned short ax = 0;
	unsigned short bx = 0;

	b_get16AX(&ax, registers, unused_m);
	b_get16BX(&bx, registers, unused_m);

	registers->sr->set(sr.substr(ax, bx));
}
void b_strlen(void* unused_p, regs* registers, memory* mem) {
	std::string sr = registers->sr->get();
	unsigned long long saved_rax = registers->rax->get();

	registers->rax->set(sr.size());
	pushMem(registries_def::RAX, registers, mem);
	registers->rax->set(saved_rax);
}

void b_print(void* unused_p, regs* registers, memory* unused_m) {
	std::string sr = registers->sr->get();
	std::cout << sr;
}
void b_println(void* unused_p, regs* registers, memory* unused_m) {
	std::string sr = registers->sr->get();
	std::cout << sr << std::endl;
}
void b_printEOL(void* unused_p, regs* unused_r, memory* unused_m) {
	std::cout << std::endl;
}

void b_castreg(registries_def receiver, regs* registers, memory* mem) {
	unsigned long long savedRAX = registers->rax->get();
	popMem(registries_def::RAX, registers, mem);
	unsigned long long value = registers->rax->get();
	registers->rax->set(savedRAX);

	registries_ptr_table ptr_table = registries_ptr_table(registers);

	if (receiver >= registries_def::AX && receiver <= registries_def::DX) {
		((reg_int<unsigned short>*)ptr_table.access(receiver))->set((unsigned short)value);
	}
	else if (receiver >= registries_def::EAX && receiver <= registries_def::EDX) {
		((reg_int<unsigned int>*)ptr_table.access(receiver))->set((unsigned int)value);
	}
	else {
		((reg_int<unsigned long long>*)ptr_table.access(receiver))->set((unsigned long long)value);
	}
}
/* STACK before calling:
*	... RECAST_TYPE VALUE
*	With output pushed in stack
* 
*	RECAST TYPES:
*	- 0 = unsigned number -> signed number
*	- 1 = signed_number -> unsigned number
*	- 2 = signed_number  -> (binary) -> unsigned number
*	- 3 = unsigned_number -> signed_number -> string
*/
void b_recast(void* unused_p, regs* registers, memory* mem) {
	unsigned long long savedRAX = registers->rax->get();

	popMem(registries_def::RAX, registers, mem);
	unsigned long long value = registers->rax->get();

	popMem(registries_def::RAX, registers, mem);
	unsigned long long recast_type = registers->rax->get();

	if (recast_type == 0) {
		long long n_value = (long long)value;
		registers->rax->set(n_value);
		pushMem(registries_def::RAX, registers, mem);
		registers->rax->set(savedRAX);
	}
	else if (recast_type == 1) {
		unsigned long long n_value = (unsigned long long)((long long)value);
		registers->rax->set(n_value);
		pushMem(registries_def::RAX, registers, mem);
		registers->rax->set(savedRAX);
	}
	else if (recast_type == 2) {
		registers->rax->set(value);
		pushMem(registries_def::RAX, registers, mem);
		registers->rax->set(savedRAX);
	}
	else if (recast_type == 3) {
		long long n_value = (long long)value;
		std::stringstream ss;
		ss << n_value;
		std::string output;
		ss >> output;

		std::string saved_sr = registers->sr->get();
		registers->sr->set(output);
		pushMemSR(unused_p, registers, mem);

		registers->sr->set(saved_sr);
		registers->rax->set(savedRAX);
	}
}