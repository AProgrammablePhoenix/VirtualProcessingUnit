#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <unordered_set>
#include <string>
#include <tuple>
#include <vector>

#if defined(__linux__)
#include <cstring>
#include <stdio.h>
#endif

#include "../CursesWrapper/wrapper.hpp"

#include "../utility.h"
#include "../Compiler/action_parser.h"
#include "assembler.h"

namespace {
	void ulongToByteArray(size_t value, byte** const output) {
		*output = new byte[8];

		for (long long j = 0, i = 7; i >= 0; i--, j++)
			(*output)[j] = (value >> (i * 8)) & 0xff;
	}
	void doubleToByteArray(double value, byte** const output) {
		*output = new byte[8];
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined (__CYGWIN__)
		memcpy_s(*output, sizeof(double), &value, sizeof(double));
#else
		memcpy(*output, &value, sizeof(double));
#endif
	}
	
	inline void ASMBL_FP_NMATHS(std::vector<byte>& out, const std::map<virtual_actions, byte>& map2ndOPC,
			action& _action, memory* const& mem) {
		out.push_back(map2ndOPC.at(_action.getAction()));
		const auto [vaddr, vsize, vopt] = *std::static_pointer_cast<arg_tuple>(_action.getValuePtr());

		byte* uc_n = new byte[sizeof(size_t)];
		mem->_MG(uc_n, sizeof(size_t), vaddr);

		byte reg_value = (byte)(ATOULL(uc_n) & 0xff);
		delete[] uc_n;

		reg_value = fp_registers_set[(comn_registers)reg_value];
		out.push_back(reg_value);
	}
}

std::vector<byte> assembleAction(action _action, memory* const mem) {
	std::vector<byte> out;
	out.push_back(instructions_set[_action.getAction()]);

	if (opt_arg_ops.find(out[0]) != opt_arg_ops.end())
		out.push_back(std::get<2>(*std::static_pointer_cast<arg_tuple>(_action.getValuePtr())).raw_byte);

	if (_action.getAction() == virtual_actions::set) {
		if (comn_registers_table::is_num_reg((comn_registers)out.back())) {
			const auto [vaddr, vsize, vopt] = *std::static_pointer_cast<arg_tuple>(_action.getValuePtr());

			byte* uc_n = new byte[sizeof(size_t)];
			mem->_MG(uc_n, vsize, vaddr);

			size_t compressed_len = COMPBA(uc_n, sizeof(size_t));

			for (byte i = 0; i < compressed_len; i++)
				out.push_back(uc_n[i]);
			delete[] uc_n;
			return out;
		}
		else if (comn_registers_table::is_fp_reg((comn_registers)out.back())) {
			const auto [vaddr, vsize, vopt] = *std::static_pointer_cast<arg_tuple>(_action.getValuePtr());

			byte* uc_a = new byte[vsize];
			mem->_MG(uc_a, vsize, vaddr);

			for (size_t i = 0; i < vsize; i++)
				out.push_back(uc_a[i]);
			delete[] uc_a;

			return out;
		}
		else if ((comn_registers)out.back() == comn_registers::CR) {
			const auto [vaddr, vsize, vopt] = *std::static_pointer_cast<arg_tuple>(_action.getValuePtr());
			byte* uc_c = new byte[1];

			mem->_MG(uc_c, 1, vaddr);
			out.push_back(uc_c[0]);

			delete[] uc_c;
			return out;
		}
	}

	if (zero_args_opcodes.find(out[0]) != zero_args_opcodes.end()) {
		return out;
	}
	else if (uint64_args_opcodes.find(out[0]) != uint64_args_opcodes.end()) {
		const auto[vaddr, vsize, vopt] = *std::static_pointer_cast<arg_tuple>(_action.getValuePtr());

		byte* uc_n = new byte[sizeof(size_t)];
		mem->_MG(uc_n, vsize, vaddr);

		size_t compressed_len = COMPBA(uc_n, sizeof(size_t));
		
		for (byte i = 0; i < compressed_len; i++)
			out.push_back(uc_n[i]);
		delete[] uc_n;
		return out;
	}
	else if (reg_args_opcodes.find(out[0]) != reg_args_opcodes.end()) {
		const auto[vaddr, vsize, vopt] = *std::static_pointer_cast<arg_tuple>(_action.getValuePtr());

		byte* uc_n = new byte[sizeof(size_t)];
		mem->_MG(uc_n, sizeof(size_t), vaddr);

		byte reg_value = (byte)(ATOULL(uc_n) & 0xff);
		delete[] uc_n;

		out.push_back(reg_value);
		return out;
	}
	else {
		return std::vector<byte>({ 0, 0, 0, 0, 0, 0, 0, 0, });
	}
}

std::vector<byte> as(std::string filename) {
	process_memory* p_mem = new process_memory;
	regs* registers = new regs();
	memory* mem = new memory(registers);

	std::vector<action> built_actions = build_actions_only(filename, p_mem, mem);
	std::vector<byte> main_linked;

	// assemble main
	for (size_t i = 0; i < built_actions.size(); i++) {
		std::vector<byte> temp = assembleAction(built_actions[i], mem);
		for (size_t j = 0; j < temp.size(); j++) {
			main_linked.push_back(temp[j]);
		}
	}

	/* link everything
	* code block format:
	*	0x00 -> 0x07 => Requested memory by the program (set to default memory size if user haven't set new memory size)
	*   0x08 -> 0x0F (8 bytes) => thread id (only used if the block is a thread one, else 8 bytes are set to 0 and ignored)
	*	0x10 -> 0x17 (8 bytes) => code length
	*	0x18 -> (0x18 + [code length - 1]) => executable code
	*/
	std::vector<byte> linked;

	byte* req_mem = nullptr;
	ULLTOA(mem->getMemLen(), &req_mem);

	for (size_t i = 0; i < sizeof(size_t); i++)
		linked.push_back(req_mem[i]);
	delete[] req_mem;

	byte* buffer = nullptr;

	ULLTOA(main_linked.size(), &buffer);
	for (size_t i = 0; i < sizeof(size_t); i++)
		linked.push_back(buffer[i]);
	delete[] buffer;

	for (size_t i = 0; i < main_linked.size(); i++)
		linked.push_back(main_linked[i]);

	delete p_mem;
	delete mem;
	delete registers;

	return linked;
}

void asFinal(std::string filename, const std::vector<byte> linkedBytes) {
	byte* linkedBytesArray = new byte[linkedBytes.size()];
	std::copy(linkedBytes.begin(), linkedBytes.end(), linkedBytesArray);

	std::ofstream file(filename, std::ios::out | std::ios::binary);	
	file.write((const char*)linkedBytesArray, linkedBytes.size());

	file.close();
	delete[] linkedBytesArray;
}

int main(int argc, char* argv[]) {
	if (argc < 3) {
		nstd::ncout << "Usage: vas <inputFile> <outputFile>" << nstd::nendl;
		return 1;
	}

	std::string inputFile = argv[1];
	std::string outputFile = argv[2];

	asFinal(outputFile, as(inputFile));

	nstd::ncout << "Linking of '" << inputFile << "': done." << nstd::nendl;

	return 0;
}