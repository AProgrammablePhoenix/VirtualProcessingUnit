#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "../Compiler/action_parser.h"
#include "runner.h"

unsigned long long byteArrayToUlong(byte* _array) {
	unsigned long long ret = 0;

	for (long long i = 7, j = 0; i >= 0; i--, j++) {
		ret |= ((unsigned long long)_array[j] << (i * 8));
	}

	return ret;
}

double byteArrayToDouble(byte* _array) {
	double ret = 0;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	memcpy_s(&ret, sizeof(double), _array, sizeof(double));
#else
	memcpy(&ret, _array, sizeof(double));
#endif

	return ret;
}

void executeByteArray(std::vector<unsigned char>* byteArray) {
	std::vector<action>* actions = new std::vector<action>;

	for (size_t i = 0; i < byteArray->size(); i++) {
		if ((*byteArray)[i] < 0x0D || (*byteArray)[i] == 0x1A ||
			((*byteArray)[i] > 0x59 && (*byteArray)[i] < 0x61) || ((*byteArray)[i] > 0x61 && (*byteArray)[i] < 0x64)
			|| ((*byteArray)[i] > 0x65 && (*byteArray)[i] < 0x74) || ((*byteArray)[i] > 0x7D && (*byteArray)[i] < 0x8A)
			|| ((*byteArray)[i] > 0x8B && (*byteArray)[i] != 0x8F && (*byteArray)[i] != 0x95)) {

			unsigned char _op = (*byteArray)[i];
			unsigned char _arg = 0;
			i++;

			virtual_actions real_op = findKeyByValue(instructions_set, _op);

			action _action(real_op, std::make_shared<unsigned char>(_arg));
			actions->push_back(_action);

			continue;
		}
		else if (((*byteArray)[i] > 0x0C && (*byteArray)[i] < 0x19) || (*byteArray)[i] == 0x8A || (*byteArray)[i] == 0x8B) {
			unsigned char _op = (*byteArray)[i];
			virtual_actions real_op = findKeyByValue(instructions_set, _op);
			i++;

			unsigned char* b_value = new unsigned char[8];
			for (size_t j = 0; j < 8; i++, j++) {
				b_value[j] = (*byteArray)[i];
			}
			i--;

			unsigned long long value = byteArrayToUlong(b_value);
			delete[] b_value;

			action _action(real_op, std::make_shared<unsigned long long>(value));
			actions->push_back(_action);

			continue;
		}
		else if ((*byteArray)[i] == 0x19) {
			unsigned char _op = (*byteArray)[i];
			virtual_actions real_op = findKeyByValue(instructions_set, _op);
			i++;

			unsigned char* b_str_size = new unsigned char[8];
			for (size_t j = 0; j < 8; i++, j++) {
				b_str_size[j] = (*byteArray)[i];
			}

			unsigned long long str_size = byteArrayToUlong(b_str_size);
			delete[] b_str_size;

			char* b_str = new char[str_size];
			for (size_t j = 0; j < str_size; i++, j++) {
				b_str[j] = (*byteArray)[i];
			}
			i--;

			std::string str(b_str, b_str + str_size);

			action _action(real_op, std::make_shared<std::string>(str));

			actions->push_back(_action);

			continue;
		}
		else if ((*byteArray)[i] == 0x8F) {
			unsigned char _op = (*byteArray)[i];
			++i;
			char _arg = (char)((*byteArray)[i]);

			virtual_actions real_op = findKeyByValue(instructions_set, _op);

			action _action(real_op, std::make_shared<char>(_arg));
			actions->push_back(_action);

			continue;
		}
		else if ((*byteArray)[i] == 0x95) {
			unsigned char _op = (*byteArray)[i];
			virtual_actions real_op = findKeyByValue(instructions_set, _op);
			i++;

			unsigned char* b_value = new unsigned char[8];
			for (size_t j = 0; j < 8; i++, j++) {
				b_value[j] = (*byteArray)[i];
			}
			i--;

			double value = byteArrayToDouble(b_value);
			delete[] b_value;

			action _action(real_op, std::make_shared<double>(value));
			actions->push_back(_action);

			continue;
		}
		else if (((*byteArray)[i] > 0x1A && ((*byteArray)[i] < 0x5A)) || (*byteArray)[i] == 0x61 || ((*byteArray)[i] > 0x63
			&& (*byteArray)[i] < 0x66) || ((*byteArray)[i] > 0x73 && (*byteArray)[i] < 0x7E)) {
			unsigned char _op = (*byteArray)[i];
			virtual_actions real_op = findKeyByValue(instructions_set, _op);
			i++;

			unsigned char _reg = (*byteArray)[i];
			registries_def real_reg = findKeyByValue(registers_set, _reg);

			action _action(real_op, std::make_shared<registries_def>(real_reg));
			actions->push_back(_action);

			continue;
		}
	}

	regs* registers = new regs;
	memory* mem = new memory(registers);

	actions_engine _actions_engine = actions_engine(mem, registers);
	process _proc = process(&_actions_engine, actions);

	_proc.start();

	delete registers;

	mem->destroy();
	delete mem;

	delete actions;
}

void executeFile(std::string filename) {
	std::ifstream input(filename, std::ios::binary);
	std::vector<byte>* exe_bytes = new std::vector<byte>(
		(std::istreambuf_iterator<char>(input)),
		(std::istreambuf_iterator<char>()));
	
	executeByteArray(exe_bytes);

	delete exe_bytes;
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		std::cerr << "Usage: vexe <executableFile>" << std::endl;
		return 1;
	}

	std::string inputfile = argv[1];

	executeFile(inputfile);
}