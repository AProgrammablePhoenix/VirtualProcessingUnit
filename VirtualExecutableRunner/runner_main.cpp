#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

#if defined(__linux__)
#include <cstring>
#include <stdio.h>
#endif

#include "../utility.h"
#include "../Compiler/action_parser.h"
#include "runner.h"

void executeByteArray(std::vector<unsigned char>* byteArray) {
	std::vector<action>* actions = new std::vector<action>;

	regs* registers = new regs;
	memory* mem = new memory(registers);

	for (size_t i = 0; i < byteArray->size(); i++) {
		if (zero_args_opcodes.find((*byteArray)[i]) != zero_args_opcodes.end()) {

			byte _op = (*byteArray)[i];
			i++;

			virtual_actions real_op = findKeyByValue(instructions_set, _op);

			action _action(real_op, nullptr);
			actions->push_back(_action);

			continue;
		}
		else if (uint64_args_opcodes.find((*byteArray)[i]) != uint64_args_opcodes.end()) {
			byte _op = (*byteArray)[i];
			virtual_actions real_op = findKeyByValue(instructions_set, _op);
			i++;

			byte* b_value = new byte[sizeof(size_t)];
			for (size_t j = 0; j < sizeof(size_t); i++, j++) {
				b_value[j] = (*byteArray)[i];
			}
			i--;

			size_t addr = mem->_ROZGST();
			size_t len = sizeof(size_t);
			mem->_ROZVS(b_value, len);

			delete[] b_value;

			action _action(real_op, std::make_shared<arg_tuple>(std::make_tuple<size_t&, size_t&>(addr, len)));
			actions->push_back(_action);

			continue;
		}
		else if ((*byteArray)[i] == ops[virtual_actions::setSR]) {
			byte _op = (*byteArray)[i];
			virtual_actions real_op = findKeyByValue(instructions_set, _op);
			i++;

			byte* b_str_size = new byte[8];
			for (size_t j = 0; j < 8; i++, j++) {
				b_str_size[j] = (*byteArray)[i];
			}

			size_t str_size = ATOULL(b_str_size);
			delete[] b_str_size;

			byte* b_str = new byte[str_size];
			for (size_t j = 0; j < str_size; i++, j++) {
				b_str[j] = (*byteArray)[i];
			}
			i--;

			size_t addr = mem->_ROZGST();
			size_t& len = str_size;
			mem->_ROZVS(b_str, len);

			delete[] b_str;

			action _action(real_op, std::make_shared<arg_tuple>(std::make_tuple<size_t&, size_t&>(addr, len)));

			actions->push_back(_action);

			continue;
		}
		else if ((*byteArray)[i] == ops[virtual_actions::setCR]) {
			byte _op = (*byteArray)[i];
			++i;
			byte _arg = (*byteArray)[i];

			virtual_actions real_op = findKeyByValue(instructions_set, _op);

			byte* uc_c = new byte[1];
			uc_c[0] = _arg;

			size_t addr = mem->_ROZGST();
			size_t len = 1;
			mem->_ROZVS(uc_c, len);

			delete[] uc_c;

			action _action(real_op, std::make_shared<arg_tuple>(std::make_tuple<size_t&, size_t&>(addr, len)));
			actions->push_back(_action);

			continue;
		}
		else if ((*byteArray)[i] == ops[virtual_actions::setDR]) {
			byte _op = (*byteArray)[i];
			virtual_actions real_op = findKeyByValue(instructions_set, _op);
			i++;

			byte* b_value = new byte[sizeof(double)];
			for (size_t j = 0; j < sizeof(double); i++, j++) {
				b_value[j] = (*byteArray)[i];
			}
			i--;

			size_t addr = mem->_ROZGST();
			size_t len = sizeof(double);
			mem->_ROZVS(b_value, len);

			delete[] b_value;

			action _action(real_op, std::make_shared<arg_tuple>(std::make_tuple<size_t&, size_t&>(addr, len)));
			actions->push_back(_action);

			continue;
		}
		else if (reg_args_opcodes.find((*byteArray)[i]) != reg_args_opcodes.end()) {
			byte _op = (*byteArray)[i];
			virtual_actions real_op = findKeyByValue(instructions_set, _op);
			i++;

			byte _reg = (*byteArray)[i];
			size_t real_reg = (size_t)(findKeyByValue(registers_set, _reg));

			byte* uc_n = nullptr;
			ULLTOA(real_reg, &uc_n);

			size_t addr = mem->_ROZGST();
			size_t len = sizeof(size_t);
			mem->_ROZVS(uc_n, len);

			delete[] uc_n;

			action _action(real_op, std::make_shared<arg_tuple>(std::make_tuple<size_t&, size_t&>(addr, len)));
			actions->push_back(_action);

			continue;
		}
	}

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