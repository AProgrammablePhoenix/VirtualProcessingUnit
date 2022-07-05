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

// RDI: destination address
// RSI: maximum length of input/output
void b_getInput(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	uint64_t dest_addr = *registers->rdi;
	uint64_t dest_len = *registers->rsi;

	std::string input;
	nstd::ncin >> input;

	if (input.size() >= dest_len)
		input = input.substr(0, dest_len - 1);

	auto temp = std::make_unique<uint8_t[]>(dest_len);
	mp_memcpy<uint8_t, uint8_t>((uint8_t*)input.data(), temp.get(), input.size() + 1);

	mem->_MS(temp.get(), dest_len, dest_addr);
}
void b_toString(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	comn_registers reg_id = ATTOREGID(reg, mem);
	if (!comn_registers_table::is_num_reg(reg_id))
		return;

	uint64_t dest_addr = *registers->rdi;
	uint64_t dest_len = *registers->rsi;

	comn_registers_table ptr_table = comn_registers_table(registers);
	size_t value = ((reg_int<size_t>*)comn_registers_table(registers).access(reg_id))->get();

	std::string str = std::to_string(value);
	if (str.size() >= dest_len)
		str = str.substr(0, dest_len - 1);

	auto temp = std::make_unique<uint8_t[]>(dest_len);
	mp_memcpy<uint8_t, uint8_t>((uint8_t*)str.data(), temp.get(), str.size() + 1);

	mem->_MS(temp.get(), dest_len, dest_addr);
}

// RDI: str address
// RSI: str length
// Output in RSI
void b_strlen(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	uint64_t src_addr = *registers->rdi;
	uint64_t src_len = *registers->rsi;
	auto temp = std::make_unique<uint8_t[]>(src_len);

	mem->_MG(temp.get(), src_len, src_addr);
	size_t j = 0;
	for (size_t i = 0; i < *registers->rsi; ++i, ++j) {
		if (temp[i] == 0)
			break;
	}

	*registers->rsi = j;
}

// RDI: source length
// RSI: source address
void b_print(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	size_t len = *registers->rdi;

	auto temp = std::make_unique<uint8_t[]>(len);
	mem->_MG(temp.get(), len, *registers->rsi);

	nstd::ncout << std::string((char*)temp.get()) << nstd::nflush;
}
void b_println(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	b_print(nullptr, registers, mem);
	nstd::ncout << nstd::nendl;
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
*	... (only if recast_type = 3)[STR_ADDR STR_LEN] RECAST_TYPE VALUE
*	With output pushed in stack (only if recast_type != 3, otherwise, output is set on given target address)
* 
*	RECAST TYPES:
*	- 0 = unsigned number -> signed number
*	- 1 = signed_number -> unsigned number
*	- 2 = signed_number  -> (binary) -> unsigned number
*	- 3 = unsigned_number -> signed_number -> string
* 
*	Excluded conversions:
*	- [un]signed number -> string (reason: handled by "toString" instruction
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
		uint64_t str_len, str_addr;
		auto buffer = std::make_unique<uint8_t[]>(sizeof(size_t));

		mem->pop(buffer.get(), sizeof(size_t));
		mp_memcpy(buffer.get(), &str_len);

		mem->pop(buffer.get(), sizeof(size_t));
		mp_memcpy(buffer.get(), &str_addr);

		long long n_value = (long long)value;
		std::stringstream ss;
		ss << n_value;
		std::string output;
		ss >> output;

		if (output.size() > str_len)
			output = output.substr(0, str_len);

		mem->_MS((unsigned char*)output.c_str(), str_len, str_addr);
	}
}

/* STACK before calling:
*	... CAST_TYPE STR_ADDR STR_SIZE
*
*	CAST TYPES:
*	- 0 = string -> unsigned number
*	- 1 = string -> signed number
*	- 2 = string -> char (only first char)
*	- 3 = string -> long double (retrieve value by popFP RFPRx (where x varies from 0 to 3 included)
*/
void b_fromString(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	auto temp = std::make_unique<uint8_t[]>(sizeof(size_t));

	mem->pop(temp.get(), sizeof(uint64_t));
	uint64_t str_size = 0;
	mp_memcpy(temp.get(), &str_size);

	mem->pop(temp.get(), sizeof(uint64_t));
	uint64_t str_addr = 0;
	mp_memcpy(temp.get(), &str_addr);

	mem->pop(temp.get(), sizeof(uint64_t));
	uint64_t cast_type = 0;
	mp_memcpy(temp.get(), &cast_type);

	auto str_buffer = std::make_unique<uint8_t[]>(str_size);
	mem->_MG(str_buffer.get(), str_size, str_addr);
	std::string value((char*)str_buffer.get(), str_size);

	if (cast_type == 0) {
		std::stringstream ss(value);
		uint64_t n_value = 0;
		ss >> n_value;

		mp_memcpy(&n_value, temp.get());
		mem->push(temp.get(), sizeof(uint64_t));
	}
	else if (cast_type == 1) {
		std::stringstream ss(value);
		int64_t n_value = 0;
		ss >> n_value;

		temp = std::make_unique<uint8_t[]>(sizeof(int64_t));
		mp_memcpy(&n_value, temp.get(), sizeof(int64_t));
		mem->push(temp.get(), sizeof(int64_t));
	}
	else if (cast_type == 2) {
		char saved_cr = registers->cr->get();
		if (value.size() < 1) {
			registers->cr->set('\0');
			pushMemCR(NULL, registers, mem);
			registers->cr->set(saved_cr);
			return;
		}

		char c = value[0];
		registers->cr->set(c);
		pushMemCR(NULL, registers, mem);
		registers->cr->set(saved_cr);
	}
	else if (cast_type == 3) {
		long double d_value = 0;
		std::stringstream ss(value);
		ss >> d_value;
		
		auto uc_ld = std::make_unique<unsigned char[]>(sizeof(long double));
		mp_memcpy(&d_value, uc_ld.get(), sizeof(long double));

		mem->push(uc_ld.get(), sizeof(long double));
	}
}

// Copy value of CR to specified address
// RDI: output address
void b_CRToSR(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	uint8_t data[] = { static_cast<uint8_t>(registers->cr->get()) };
	mem->_MS(data, 1, *registers->rdi);
}

/* Reverse string located at address:
*	RDI: I/O buffer address
*	RSI: I/O buffer length
*/
void b_RevSR(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	uint64_t addr = *registers->rdi;
	uint64_t len = *registers->rsi;

	auto temp = std::make_unique<uint8_t[]>(len);
	mem->_MG(temp.get(), len, addr);

	std::reverse(temp.get(), temp.get() + len);

	mem->_MS(temp.get(), len, addr);
}

// Will be updated
/*	Stack before calling:
*	... FP_VALUE
*	
*	Registers before calling:
*		RDI: address to output string
*		RSI: output string length
*/
void b_FPToSR(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	uint64_t dest_addr = *registers->rdi;
	uint64_t dest_len = *registers->rsi;

	auto uc_ld = std::make_unique<unsigned char[]>(sizeof(long double));
	mem->pop(uc_ld.get(), sizeof(long double));

	long double d = 0;
	mp_memcpy(uc_ld.get(), &d, sizeof(long double));

	std::ostringstream ss;
	ss << std::setprecision(std::numeric_limits<long double>::digits10) << std::fixed << d;
	std::string s_value = ss.str().substr(0, dest_len - 1);

	mem->_MS((uint8_t*)s_value.data(), s_value.size() + 1, dest_addr);
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