#include <algorithm>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "../Compiler/variables.h"
#include "../Memory/memory_symbols.h"
#include "registers_symbols.h"

unsigned long long usr_var_counter = 0;

void b_getInput(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	std::string saved_sr = registers->sr->get(), input;

	std::getline(std::cin, input);
	registers->sr->set(input);

	pushMemSR(unused_p, registers, mem);
	registers->sr->set(saved_sr);
}
void b_toString(std::shared_ptr<void> reg, regs* registers, memory* unused_m) {
	registries_def reg_id = *std::static_pointer_cast<registries_def>(reg);

	registries_ptr_table ptr_table = registries_ptr_table(registers);
	unsigned long long value = ((reg_int<unsigned long long>*)ptr_table.access(reg_id))->get();

	std::stringstream ss;
	ss << value;

	std::shared_ptr<std::string> converted = std::make_shared<std::string>(ss.str());
	b_setSR(converted, registers, unused_m);
}
void b_mergeString(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	std::string pushed = registers->sr->get();
	popMemSR(NULL, registers, mem);
	std::string base = registers->sr->get();

	registers->sr->set(base + pushed);
}
void b_substring(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m) {
	std::string sr = registers->sr->get();
	std::shared_ptr<unsigned short> ax = std::make_shared<unsigned short>(0);
	std::shared_ptr<unsigned short> bx = std::make_shared<unsigned short>(0);

	b_get16AX(ax, registers, unused_m);
	b_get16BX(bx, registers, unused_m);

	registers->sr->set(sr.substr(*ax, *bx));
}
void b_strlen(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	std::string sr = registers->sr->get();
	unsigned long long saved_rax = registers->rax->get();

	registers->rax->set(sr.size());
	pushMem(std::make_shared<registries_def>(registries_def::RAX), registers, mem);
	registers->rax->set(saved_rax);
}

void b_print(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m) {
	std::string sr = registers->sr->get();
	std::cout << sr;
}
void b_println(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m) {
	std::string sr = registers->sr->get();
	std::cout << sr << std::endl;
}
void b_printEOL(std::shared_ptr<void> unused_p, regs* unused_r, memory* unused_m) {
	std::cout << std::endl;
}

void b_castreg(std::shared_ptr<void> receiver, regs* registers, memory* mem) {
	registries_def recv_id = *std::static_pointer_cast<registries_def>(receiver);

	unsigned long long savedRAX = registers->rax->get();
	popMem(std::make_shared<registries_def>(registries_def::RAX), registers, mem);
	unsigned long long value = registers->rax->get();
	registers->rax->set(savedRAX);

	registries_ptr_table ptr_table = registries_ptr_table(registers);

	if (recv_id >= registries_def::AX && recv_id <= registries_def::DX) {
		((reg_int<unsigned short>*)ptr_table.access(recv_id))->set((unsigned short)value);
	}
	else if (recv_id >= registries_def::EAX && recv_id <= registries_def::EDX) {
		((reg_int<unsigned int>*)ptr_table.access(recv_id))->set((unsigned int)value);
	}
	else {
		((reg_int<unsigned long long>*)ptr_table.access(recv_id))->set((unsigned long long)value);
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
* 
*	Excluded conversions:
*	- unsigned_number -> string (reason: handled by "toString" instruction
*	- string -> unsigned number | signed number (reason: handled by "fromString" instruction)
*/
void b_recast(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	unsigned long long savedRAX = registers->rax->get();

	popMem(std::make_shared<registries_def>(registries_def::RAX), registers, mem);
	unsigned long long value = registers->rax->get();

	popMem(std::make_shared<registries_def>(registries_def::RAX), registers, mem);
	unsigned long long recast_type = registers->rax->get();

	if (recast_type == 0) {
		long long n_value = (long long)value;
		registers->rax->set(n_value);
		pushMem(std::make_shared<registries_def>(registries_def::RAX), registers, mem);
		registers->rax->set(savedRAX);
	}
	else if (recast_type == 1) {
		unsigned long long n_value = (unsigned long long)((long long)value);
		registers->rax->set(n_value);
		pushMem(std::make_shared<registries_def>(registries_def::RAX), registers, mem);
		registers->rax->set(savedRAX);
	}
	else if (recast_type == 2) {
		registers->rax->set(value);
		pushMem(std::make_shared<registries_def>(registries_def::RAX), registers, mem);
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

/* STACK before calling:
*	... CAST_TYPE VALUE
*	With output pushed in stack
*
*	CAST TYPES:
*	- 0 = string -> unsigned number
*	- 1 = string -> signed number
*	- 2 = string -> char (only first char)
*	- 3 = string -> double
*/
void b_fromString(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	std::string saved_sr = registers->sr->get();
	unsigned long long saved_rax = registers->rax->get();

	popMemSR(unused_p, registers, mem);
	std::string value = registers->sr->get();

	popMem(std::make_shared<registries_def>(registries_def::RAX), registers, mem);
	unsigned long long cast_type = registers->rax->get();

	if (cast_type == 0) {
		std::stringstream ss(value);
		unsigned long long n_value = 0;
		ss >> n_value;
		registers->rax->set(n_value);
		pushMem(std::make_shared<registries_def>(registries_def::RAX), registers, mem);
		registers->rax->set(saved_rax);
		registers->sr->set(saved_sr);
	}
	else if (cast_type == 1) {
		std::stringstream ss(value);
		long long n_value = 0;
		ss >> n_value;
		registers->rax->set(n_value);
		pushMem(std::make_shared<registries_def>(registries_def::RAX), registers, mem);
		registers->rax->set(saved_rax);
		registers->sr->set(saved_sr);
	}
	else if (cast_type == 2) {
		char saved_cr = registers->cr->get();
		if (value.size() < 1) {
			registers->rax->set(saved_rax);
			registers->sr->set(saved_sr);
			return;
		}

		char c = value[0];
		registers->cr->set(c);
		pushMemCR(NULL, registers, mem);
		registers->rax->set(saved_rax);
		registers->sr->set(saved_sr);
		registers->cr->set(saved_cr);
	}
	else if (cast_type == 3) {
		double saved_dr = registers->dr->get(), d_value = 0;
		std::stringstream ss(value);
		ss >> d_value;
		registers->dr->set(d_value);
		pushMemDR(nullptr, registers, mem);
		registers->rax->set(saved_rax);
		registers->sr->set(saved_sr);
		registers->dr->set(saved_dr);
	}
}

/* Registers before calling:
*  CR: input value
*  SR: any value
*  With output set up on SR, and previous value of SR is lost (unless you pushed it on the stack before)
*/
void b_CRToSR(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	char c = registers->cr->get();
	b_setSR(std::make_shared<std::string>(std::string(1, c)), registers, mem);
}

/* Reverse string in SR:
*	- Input/Output in SR
*/
void b_RevSR(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	std::string vstr = registers->sr->get();
	std::reverse(vstr.begin(), vstr.end());
	registers->sr->set(vstr);
}

/*	Registers before calling:
*	DR: input value
*	SR: any value
*	With output set up on SR, and previous value of SR is lost (unlss you pushed it on the stack before)
*/
void b_DRToSR(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	double d = registers->dr->get();
	std::ostringstream ss;
	ss << std::fixed << d;
	std::string s_value = ss.str();
	b_setSR(std::make_shared<std::string>(s_value), registers, mem);
}

/* Registers before calling:
*	DR: input value
*	With output pushed onto stack
* 
*   Convert value in DR to unsigned long long
*/
void b_DRToULL(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	double d = registers->dr->get();
	unsigned long long n = (unsigned long long)std::llround(d);

	unsigned long long saved_rax = registers->rax->get();
	registers->rax->set(n);
	pushMem(std::make_shared<registries_def>(registries_def::RAX), registers, mem);

	registers->rax->set(saved_rax);
}

// Same as DRToULL but it converts value in DR to long long
void b_DRToLL(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	double d = registers->dr->get();
	long long n = std::llround(d);

	unsigned long long saved_rax = registers->rax->get();
	registers->rax->set((unsigned long long)n);
	pushMem(std::make_shared<registries_def>(registries_def::RAX), registers, mem);

	registers->rax->set(saved_rax);
}