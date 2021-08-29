#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <string>

#if defined(__linux__)
	#include <cstring>
	#include <stdio.h>
#endif

#include "../CursesWrapper/wrapper.hpp"

#include "../utility.h"
#include "../Compiler/variables.h"
#include "../Memory/memory_symbols.h"
#include "registers_symbols.h"

void b_getInput(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	std::string saved_sr = registers->sr->get(), input;

	nstd::ncin >> input;
	registers->sr->set(input);

	pushMemSR(unused_p, registers, mem);
	registers->sr->set(saved_sr);
}
void b_toString(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	comn_registers reg_id = ATTOREGID(reg, mem);
	if (!comn_registers_table::is_num_reg(reg_id))
		return;

	comn_registers_table ptr_table = comn_registers_table(registers);
	size_t value = ((reg_int<size_t>*)comn_registers_table(registers).access(reg_id))->get();

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
	registers->sr->set(registers->sr->get().substr(registers->rax->get(), registers->rbx->get()));
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
	nstd::ncout << sr;
}
void b_println(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m) {
	std::string sr = registers->sr->get();
	nstd::ncout << sr << nstd::nendl;
}
void b_printEOL(std::shared_ptr<void> unused_p, regs* unused_r, memory* unused_m) {
	nstd::ncout << nstd::nendl;
}

void b_castreg(std::shared_ptr<void> receiver, regs* registers, memory* mem) {
	comn_registers recv_id = ATTOREGID(receiver, mem);

	unsigned char* temp = new unsigned char[sizeof(size_t)];
	mem->pop(temp, sizeof(size_t));
	size_t value = 0;

	mp_memcpy(temp, &value);
	delete[] temp;

	comn_registers_table ptr_table = comn_registers_table(registers);

	if (recv_id >= comn_registers::AX && recv_id <= comn_registers::DX)
		((reg_int<unsigned short>*)ptr_table.access(recv_id))->set((unsigned short)value);
	else if (recv_id >= comn_registers::EAX && recv_id <= comn_registers::EDX)
		((reg_int<unsigned int>*)ptr_table.access(recv_id))->set((unsigned int)value);
	else
		((reg_int<size_t>*)ptr_table.access(recv_id))->set((size_t)value);
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
*
*	CAST TYPES:
*	- 0 = string -> unsigned number
*	- 1 = string -> signed number
*	- 2 = string -> char (only first char)
*	- 3 = string -> long double (retrieve value by popFP RFPRx (where x varies from 0 to 3 included)
*/
void b_fromString(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	std::string saved_sr = registers->sr->get();

	popMemSR(nullptr, registers, mem);
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
		long double saved_rfpr0 = *registers->rfpr0, d_value = 0;
		std::stringstream ss(value);
		ss >> d_value;

		auto uc_ld = std::make_unique<unsigned char[]>(sizeof(long double));
		mp_memcpy(&d_value, uc_ld.get(), sizeof(long double));

		mem->push(uc_ld.get(), sizeof(long double));

		registers->sr->set(saved_sr);
		*registers->rfpr0 = saved_rfpr0;
	}
}

/* Registers before calling:
*  CR: input value
*  SR: any value
*  With output set up on SR, and previous value of SR is lost (unless you pushed it on the stack before)
*/
void b_CRToSR(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	registers->sr->set(std::string(1, registers->cr->get()));
}

/* Reverse string in SR:
*	- Input/Output in SR
*/
void b_RevSR(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	std::string vstr = registers->sr->get();
	std::reverse(vstr.begin(), vstr.end());
	registers->sr->set(vstr);
}

/*	Stack before calling:
*	... FP_VALUE
*	
*	Registers before calling:
*	SR: any value
*	With output set up on SR, and previous value of SR is lost (unlss you pushed it on the stack before)
*/
void b_FPToSR(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	auto uc_ld = std::make_unique<unsigned char[]>(sizeof(long double));
	mem->pop(uc_ld.get(), sizeof(long double));

	long double d = 0;
	mp_memcpy(uc_ld.get(), &d, sizeof(long double));

	std::ostringstream ss;
	ss << std::setprecision(std::numeric_limits<long double>::digits10) << std::fixed << d;
	std::string s_value = ss.str();

	registers->sr->set(s_value);
}

/*  Stack statis before calling:
*	... FP_VALUE
*	With output pushed onto stack
* 
*   Convert pushed value of FP register to size_t
*/
void b_FPToULL(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	auto uc_ld = std::make_unique<unsigned char[]>(sizeof(long double));
	mem->pop(uc_ld.get(), sizeof(long double));

	long double d = 0;
	mp_memcpy(uc_ld.get(), &d, sizeof(long double));

	size_t n = (size_t)std::llround(d);

	auto temp = std::make_unique<unsigned char[]>(sizeof(size_t));
	mp_memcpy(&n, temp.get(), sizeof(size_t));

	mem->push(temp.get(), sizeof(size_t));
}

// Same as FPToULL but it converts pushed value of FP register to long long
void b_FPToLL(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	auto uc_ld = std::make_unique<unsigned char[]>(sizeof(long double));
	mem->pop(uc_ld.get(), sizeof(long double));

	long double d = 0;
	mp_memcpy(uc_ld.get(), &d, sizeof(long double));

	long long n = std::llround(d);

	auto temp = std::make_unique<unsigned char[]>(sizeof(long double));
	mp_memcpy(&n, temp.get(), sizeof(long long));

	mem->push(temp.get(), sizeof(long long));
}