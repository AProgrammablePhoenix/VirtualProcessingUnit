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

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#define ISWIN 1
#endif

#include "../Compiler/action_parser.h"
#include "assembler.h"

void ulongToByteArray(size_t value, byte** output) {
	*output = new byte[8];

	for (long long j = 0, i = 7; i >= 0; i--, j++) {
		(*output)[j] = (value >> (i * 8)) & 0xff;
	}
}
void doubleToByteArray(double value, byte** output) {
	*output = new byte[8];
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined (__CYGWIN8__)
	memcpy_s(*output, sizeof(double), &value, sizeof(double));
#else
	memcpy(*output, &value, sizeof(double));
#endif
}

std::vector<byte> assembleAction(action _action, memory* mem) {
	std::vector<byte> out;
	out.push_back(instructions_set[_action.getAction()]);

	if (zero_args_opcodes.find(out[0]) != zero_args_opcodes.end()) {
		out.push_back(0);
		return out;
	}
	else if (uint64_args_opcodes.find(out[0]) != uint64_args_opcodes.end()) {
		std::tuple<size_t, size_t> varinfos = *std::static_pointer_cast<std::tuple<size_t, size_t>>(_action.getValuePtr());

		byte* uc_n = new byte[sizeof(size_t)];
		mem->_ROZVG(uc_n, std::get<1>(varinfos), std::get<0>(varinfos));		
		
		for (byte i = 0; i < 8; i++) {
			out.push_back(uc_n[i]);
		}
		delete[] uc_n;
		return out;
	}
	else if (out[0] == ops[virtual_actions::setSR]) {
		std::tuple<size_t, size_t> varinfos = *std::static_pointer_cast<std::tuple<size_t, size_t>>(_action.getValuePtr());
		size_t str_size = std::get<1>(varinfos);		

		byte* b_str_size = new byte[8];
		ulongToByteArray(str_size, &b_str_size);

		for (byte i = 0; i < 8; i++) {
			out.push_back(b_str_size[i]);
		}
		delete[] b_str_size;

		byte* b_str = new byte[str_size];
		mem->_ROZVG(b_str, str_size, std::get<0>(varinfos));

		for (byte i = 0; i < str_size; i++) {
			out.push_back(b_str[i]);
		}

		delete[] b_str;
		return out;
	}
	else if (out[0] == ops[virtual_actions::setCR]) {
		std::tuple<size_t, size_t> varinfos = *std::static_pointer_cast<std::tuple<size_t, size_t>>(_action.getValuePtr());
		byte* uc_c = new byte[1];

		mem->_ROZVG(uc_c, 1, std::get<0>(varinfos));
		out.push_back(uc_c[0]);

		delete[] uc_c;
		return out;
	}
	else if (out[0] == ops[virtual_actions::setDR]) {
		std::tuple<size_t, size_t> varinfos = *std::static_pointer_cast<std::tuple<size_t, size_t>>(_action.getValuePtr());

		byte* uc_d = new byte[sizeof(double)];
		mem->_ROZVG(uc_d, sizeof(double), std::get<0>(varinfos));
		
		for (byte i = 0; i < 8; i++) {
			out.push_back(uc_d[i]);
		}
		delete[] uc_d;
		return out;
	}
	else if (reg_args_opcodes.find(out[0]) != reg_args_opcodes.end()) {
		unsigned char reg_value = (*std::static_pointer_cast<size_t>(_action.getValuePtr())) & 0xff;
		reg_value = registers_set[(registries_def)reg_value];

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
	std::vector<byte> linked;

	for (size_t i = 0; i < built_actions.size(); i++) {
		std::vector<byte> temp = assembleAction(built_actions[i], mem);
		for (size_t j = 0; j < temp.size(); j++) {
			linked.push_back(temp[j]);
		}
	}

	delete p_mem;
	delete mem;
	delete registers;

	return linked;
}

void asFinal(std::string filename, std::vector<byte> linkedBytes) {
	byte* linkedBytesArray = new byte[linkedBytes.size()];
	std::copy(linkedBytes.begin(), linkedBytes.end(), linkedBytesArray);

	std::ofstream file(filename, std::ios::out | std::ios::binary);	
	file.write((const char*)linkedBytesArray, linkedBytes.size());

	file.close();
}

int main(int argc, char* argv[]) {
	if (argc < 3) {
		std::cerr << "Usage: vas <inputFile> <outputFile>" << std::endl;
		return 1;
	}

	std::string inputFile = argv[1];
	std::string outputFile = argv[2];

	asFinal(outputFile, as(inputFile));

	std::cout << "Linking of '" << inputFile << "': done." << std::endl;

	return 0;
}