#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#if defined(__linux__)
	#include <cstring>
	#include <stdio.h>
#endif

#include "../utility.h"
#include "../Compiler/variables.h"
#include "../Memory/memory_symbols.h"
#include "registers_symbols.h"

void b_getInput(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	std::string saved_sr = registers->sr->get(), input;

	std::getline(std::cin, input);
	registers->sr->set(input);

	pushMemSR(unused_p, registers, mem);
	registers->sr->set(saved_sr);
}
void b_toString(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	registries_def reg_id = ATTOREGID(reg, mem);
	registries_ptr_table ptr_table = registries_ptr_table(registers);
	size_t value = ((reg_int<size_t>*)ptr_table.access(reg_id))->get();

	std::stringstream ss;
	ss << value;

	registers->sr->set(ss.str());
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

	size_t value = sr.size();
	unsigned char* uc_n = new unsigned char[sizeof(size_t)];

	mp_memcpy(&value, uc_n);

	mem->push(uc_n, sizeof(size_t));
	delete[] uc_n;
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
	registries_def recv_id = ATTOREGID(receiver, mem);

	unsigned char* temp = new unsigned char[sizeof(size_t)];
	mem->pop(temp, sizeof(size_t));
	size_t value = 0;

	mp_memcpy(temp, &value);
	delete[] temp;

	registries_ptr_table ptr_table = registries_ptr_table(registers);

	if (recv_id >= registries_def::AX && recv_id <= registries_def::DX) {
		((reg_int<unsigned short>*)ptr_table.access(recv_id))->set((unsigned short)value);
	}
	else if (recv_id >= registries_def::EAX && recv_id <= registries_def::EDX) {
		((reg_int<unsigned int>*)ptr_table.access(recv_id))->set((unsigned int)value);
	}
	else {
		((reg_int<size_t>*)ptr_table.access(recv_id))->set((size_t)value);
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
	unsigned char* temp = new unsigned char[sizeof(size_t)];
	size_t value = 0;
	size_t recast_type = 0;

	mem->pop(temp, sizeof(size_t));
	mp_memcpy(temp, &value);

	mem->pop(temp, sizeof(size_t));
	mp_memcpy(temp, &recast_type);
	delete[] temp;

	if (recast_type == 0) {
		long long n_value = (long long)value;
		temp = new unsigned char[sizeof(long long)];

		mp_memcpy(&n_value, temp, sizeof(long long));
		mem->push(temp, sizeof(long long));
		delete[] temp;
	}
	else if (recast_type == 1) {
		size_t n_value = (size_t)((long long)value);
		temp = new unsigned char[sizeof(size_t)];

		mp_memcpy(&n_value, temp);
		mem->push(temp, sizeof(size_t));
		delete[] temp;
	}
	else if (recast_type == 2) {
		temp = new unsigned char[sizeof(size_t)];

		mp_memcpy(&value, temp);
		mem->push(temp, sizeof(size_t));
		delete[] temp;
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

	popMemSR(unused_p, registers, mem);
	std::string value = registers->sr->get();

	unsigned char* temp = new unsigned char[sizeof(size_t)];
	mem->pop(temp, sizeof(size_t));
	size_t cast_type = 0;

	mp_memcpy(temp, &cast_type);
	delete[] temp;

	if (cast_type == 0) {
		std::stringstream ss(value);
		size_t n_value = 0;
		ss >> n_value;

		temp = new unsigned char[sizeof(size_t)];		
		mp_memcpy(&n_value, temp);
		mem->push(temp, sizeof(size_t));
		registers->sr->set(saved_sr);
		delete[] temp;
	}
	else if (cast_type == 1) {
		std::stringstream ss(value);
		long long n_value = 0;
		ss >> n_value;

		temp = new unsigned char[sizeof(long long)];
		mp_memcpy(&n_value, temp, sizeof(long long));
		mem->push(temp, sizeof(long long));
		delete[] temp;

		registers->sr->set(saved_sr);
	}
	else if (cast_type == 2) {
		char saved_cr = registers->cr->get();
		if (value.size() < 1) {
			registers->cr->set('\0');
			pushMemCR(NULL, registers, mem);
			registers->sr->set(saved_sr);
			registers->cr->set(saved_cr);
			return;
		}

		char c = value[0];
		registers->cr->set(c);
		pushMemCR(NULL, registers, mem);
		registers->sr->set(saved_sr);
		registers->cr->set(saved_cr);
	}
	else if (cast_type == 3) {
		double saved_dr = registers->dr->get(), d_value = 0;
		std::stringstream ss(value);
		ss >> d_value;

		registers->dr->set(d_value);
		pushMemDR(nullptr, registers, mem);

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
	registers->sr->set(std::string(1, c));
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
	registers->sr->set(s_value);
}

/* Registers before calling:
*	DR: input value
*	With output pushed onto stack
* 
*   Convert value in DR to size_t
*/
void b_DRToULL(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	double d = registers->dr->get();
	size_t n = (size_t)std::llround(d);

	unsigned char* temp = new unsigned char[sizeof(size_t)];
	mp_memcpy(&n, temp);
	mem->push(temp, sizeof(size_t));

	delete[] temp;
}

// Same as DRToULL but it converts value in DR to long long
void b_DRToLL(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	double d = registers->dr->get();
	long long n = std::llround(d);

	unsigned char* temp = new unsigned char[sizeof(long long)];
	mp_memcpy(&n, temp, sizeof(long long));
	mem->push(temp, sizeof(long long));

	delete[] temp;
}