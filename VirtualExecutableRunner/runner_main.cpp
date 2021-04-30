#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "../Compiler/action_parser.h"
#include "runner.h"

std::vector<void*> runtime_vars;

unsigned long long byteArrayToUlong(byte* _array) {
	unsigned long long ret = 0;

	for (long long i = 7, j = 0; i >= 0; i--, j++) {
		ret |= ((unsigned long long)_array[j] << (i * 8));
	}

	return ret;
}

void executeByteArray(std::vector<unsigned char>* byteArray) {
	std::vector<action>* actions = new std::vector<action>;

	for (size_t i = 0; i < byteArray->size(); i++) {
		if ((*byteArray)[i] < 0x0D || (*byteArray)[i] == 0x1A ||
			((*byteArray)[i] > 0x59 && (*byteArray)[i] < 0x61) || ((*byteArray)[i] > 0x61 && (*byteArray)[i] < 0x64)
			|| ((*byteArray)[i] > 0x65 && (*byteArray)[i] < 0x84) || ((*byteArray)[i] > 0x8D && (*byteArray)[i] < 0x9A)
			|| (*byteArray)[i] > 0x9B) {

			unsigned char _op = (*byteArray)[i];
			unsigned char _arg = 0;
			i++;

			virtual_actions real_op = findKeyByValue(instructions_set, _op);

			//unsigned char* p_arg = new unsigned char;
			//*p_arg = _arg;
			runtime_vars.push_back(new unsigned char(_arg));

			action _action(real_op, runtime_vars[runtime_vars.size() - 1]);
			actions->push_back(_action);

			continue;
		}
		else if (((*byteArray)[i] > 0x0C && (*byteArray)[i] < 0x19) || (*byteArray)[i] == 0x9A || (*byteArray)[i] == 0x9B) {
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

			//unsigned long long* p_arg = new unsigned long long;
			//*p_arg = value;
			runtime_vars.push_back(new unsigned long long(value));

			action _action(real_op, runtime_vars[runtime_vars.size() - 1]);
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
			//std::string* p_arg = new std::string;
			//*p_arg = str;
			runtime_vars.push_back(new std::string(str));

			action _action(real_op, runtime_vars[runtime_vars.size() - 1]);

			actions->push_back(_action);

			continue;
		}
		else if (((*byteArray)[i] > 0x1A && ((*byteArray)[i] < 0x5A)) || (*byteArray)[i] == 0x61 || ((*byteArray)[i] > 0x63
			&& (*byteArray)[i] < 0x66) || ((*byteArray)[i] > 0x83 && (*byteArray)[i] < 0x8E)) {
			unsigned char _op = (*byteArray)[i];
			virtual_actions real_op = findKeyByValue(instructions_set, _op);
			i++;

			unsigned char _reg = (*byteArray)[i];
			registries_def real_reg = findKeyByValue(registers_set, _reg);

			action _action(real_op, (void*)real_reg);
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

	for (size_t i = 0; i < runtime_vars.size(); i++) {
		delete runtime_vars[i];
	}
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