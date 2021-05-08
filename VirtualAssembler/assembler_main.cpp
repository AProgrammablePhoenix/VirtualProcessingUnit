#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>

#if defined(__linux__)
#include <stdio.h>
#include <cstring>
#endif

#include "../Compiler/action_parser.h"
#include "assembler.h"

void ulongToByteArray(unsigned long long value, byte** output) {
	*output = new byte[8];

	for (long long j = 0, i = 7; i >= 0; i--, j++) {
		(*output)[j] = (value >> (i * 8)) & 0xff;
	}
}

std::vector<byte> assembleAction(action _action) {
	std::vector<byte> out;
	out.push_back(instructions_set[_action.getAction()]);

	if (out[0] < 0x0D || out[0] == 0x1A || (out[0] > 0x59 && out[0] < 0x61) || (out[0] > 0x61 && out[0] < 0x64) 
	|| (out[0] > 0x65 && out[0] < 0x74) || (out[0] > 0x7D && out[0] < 0x8A) || out[0] > 0x8B) {
		out.push_back(0);
		return out;
	}
	else if ((out[0] > 0x0C && out[0] < 0x19) || out[0] == 0x8A || out[0] == 0x8B) {
		unsigned long long value = *std::static_pointer_cast<unsigned long long>(_action.getValuePtr());
		byte* converted = new byte[8];
		ulongToByteArray(value, &converted);
		
		for (byte i = 0; i < 8; i++) {
			out.push_back(converted[i]);
		}
		delete[] converted;
		return out;
	}
	else if (out[0] == 0x19) {
		std::string str = *std::static_pointer_cast<std::string>(_action.getValuePtr());
		unsigned long long str_size = str.size();

		byte* b_str_size = new byte[8];
		ulongToByteArray(str_size, &b_str_size);

		for (byte i = 0; i < 8; i++) {
			out.push_back(b_str_size[i]);
		}
		delete[] b_str_size;

		char* b_str = new char[str_size];
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
		strncpy_s(b_str, str_size, str.c_str(), str_size);
#else
		strncpy(b_str, str.c_str(), str_size);
#endif

		for (byte i = 0; i < str_size; i++) {
			out.push_back(b_str[i]);
		}

		delete[] b_str;
		return out;
	}
	else if ((out[0] > 0x1A && out[0] < 0x5A) || out[0] == 0x61 || (out[0] > 0x63 && out[0] < 0x66)
			|| (out[0] > 0x73 && out[0] < 0x7E)) {
		unsigned char reg_value = (*std::static_pointer_cast<unsigned long long>(_action.getValuePtr())) & 0xff;
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
	std::vector<action> built_actions = build_actions_only(filename, p_mem);
	std::vector<byte> linked;

	for (size_t i = 0; i < built_actions.size(); i++) {
		std::vector<byte> temp = assembleAction(built_actions[i]);
		for (size_t j = 0; j < temp.size(); j++) {
			linked.push_back(temp[j]);
		}
	}

	delete p_mem;

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