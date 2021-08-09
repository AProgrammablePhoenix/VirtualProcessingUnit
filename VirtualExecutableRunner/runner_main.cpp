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

std::vector<action> decodeByteArray(std::vector<unsigned char>* byteArray, memory*& mem) {
	std::vector<action> actions;

	for (size_t i = 0; i < byteArray->size(); i++) {
		if (zero_args_opcodes.find((*byteArray)[i]) != zero_args_opcodes.end()) {

			byte _op = (*byteArray)[i];

			virtual_actions real_op = findKeyByValue(instructions_set, _op);

			action _action(real_op, nullptr);
			actions.push_back(_action);

			continue;
		}
		else if (uint64_args_opcodes.find((*byteArray)[i]) != uint64_args_opcodes.end()) {
			byte _op = (*byteArray)[i];
			virtual_actions real_op = findKeyByValue(instructions_set, _op);
			i++;

			byte* b_value = new byte[sizeof(size_t) + 1];
			b_value[0] = (*byteArray)[i];
			i++;

			size_t reallen;

			if ((size_t)b_value[0] < sizeof(size_t)) {
				for (size_t j = 0; j < sizeof(size_t) - b_value[0]; j++, i++) {
					b_value[j + 1] = (*byteArray)[i];
				}
				i--;

				reallen = EXTDBA(b_value, sizeof(size_t) - b_value[0] + 1);
			}
			else {
				b_value[1] = (*byteArray)[i];
				reallen = EXTDBA(b_value, 2);
			}

			size_t addr = mem->_SDZTOP();
			mem->_SDZS(b_value, reallen);

			delete[] b_value;

			action _action(real_op, std::make_shared<arg_tuple>(std::make_tuple<size_t&, size_t&>(addr, reallen)));
			actions.push_back(_action);

			continue;
		}
		else if ((*byteArray)[i] == ops[virtual_actions::setSR]) {
			byte _op = (*byteArray)[i];
			virtual_actions real_op = findKeyByValue(instructions_set, _op);
			i++;

			byte* b_str_size = new byte[sizeof(size_t) + 1];
			b_str_size[0] = (*byteArray)[i];
			i++;

			size_t complen;

			if ((size_t)b_str_size[0] < sizeof(size_t)) {
				for (size_t j = 0; j < sizeof(size_t) - b_str_size[0]; j++, i++) {
					b_str_size[j + 1] = (*byteArray)[i];
				}

				complen = EXTDBA(b_str_size, sizeof(size_t) - b_str_size[0] + 1);
			}
			else {
				b_str_size[1] = (*byteArray)[i];
				complen = EXTDBA(b_str_size, 2);
			}

			size_t str_size = ATOULL(b_str_size);
			delete[] b_str_size;

			byte* b_str = new byte[str_size];
			for (size_t j = 0; j < str_size; i++, j++) {
				b_str[j] = (*byteArray)[i];
			}
			i--;

			size_t addr = mem->_SDZTOP();
			size_t& len = str_size;
			mem->_SDZS(b_str, len);

			delete[] b_str;

			action _action(real_op, std::make_shared<arg_tuple>(std::make_tuple<size_t&, size_t&>(addr, len)));

			actions.push_back(_action);

			continue;
		}
		else if ((*byteArray)[i] == ops[virtual_actions::setCR]) {
			byte _op = (*byteArray)[i];
			++i;
			byte _arg = (*byteArray)[i];

			virtual_actions real_op = findKeyByValue(instructions_set, _op);

			byte* uc_c = new byte[1];
			uc_c[0] = _arg;

			size_t addr = mem->_SDZTOP();
			size_t len = 1;
			mem->_SDZS(uc_c, len);

			delete[] uc_c;

			action _action(real_op, std::make_shared<arg_tuple>(std::make_tuple<size_t&, size_t&>(addr, len)));
			actions.push_back(_action);

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

			size_t addr = mem->_SDZTOP();
			size_t len = sizeof(double);
			mem->_SDZS(b_value, len);

			delete[] b_value;

			action _action(real_op, std::make_shared<arg_tuple>(std::make_tuple<size_t&, size_t&>(addr, len)));
			actions.push_back(_action);

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

			size_t addr = mem->_SDZTOP();
			size_t len = sizeof(size_t);
			mem->_SDZS(uc_n, len);

			delete[] uc_n;

			action _action(real_op, std::make_shared<arg_tuple>(std::make_tuple<size_t&, size_t&>(addr, len)));
			actions.push_back(_action);

			continue;
		}
		else if (parted_opcodes.find((*byteArray)[i]) != parted_opcodes.end()) {
			if ((*byteArray)[i] == instructions_set[virtual_actions::setFPR0]) {
				i++;

				byte _fpr_opc = (*byteArray)[i];
				virtual_actions real_op = findKeyByValue(map_FPR_set_2nd_opc, _fpr_opc);
				i++;

				size_t addr = mem->_SDZTOP();
				mem->_SDZS(byteArray->data() + i, sizeof(double));
				i += sizeof(double) - 1;

				action _action(real_op, std::make_shared<arg_tuple>(std::make_tuple<size_t&, size_t&&>(addr, (size_t)sizeof(double))));
				actions.push_back(_action);

				continue;
			}
		}
	}

	return actions;
}

std::vector<byte> decodeFile(const std::vector<byte> *const file_content,
		std::vector<std::tuple<std::vector<byte>, size_t>>& proc_threads, size_t& req_mem) {
	std::vector<std::tuple<std::vector<byte>, size_t>> threads_code;
	std::vector<byte> main_code;

	size_t i = 0;

	byte* b_req_mem = new byte[sizeof(size_t)];
	for (; i < sizeof(size_t); i++)
		b_req_mem[i] = (*file_content)[i];
	req_mem = ATOULL(b_req_mem);
	delete[] b_req_mem;

	bool main_found = false;

	// fetch non main code
	for (i = sizeof(size_t); i < file_content->size(); i++) {
		if ((*file_content)[i] == 0x00) {
			main_found = true;
			i++;
			break;
		}
		else if ((*file_content)[i] == 0x01) {
			byte* buffer = new byte[sizeof(size_t)];
			++i;

			for (size_t j = 0; j < sizeof(size_t); i++,j++)
				buffer[j] = (*file_content)[i];

			size_t thread_id = ATOULL(buffer);
			for (size_t j = 0; j < sizeof(size_t); i++, j++)
				buffer[j] = (*file_content)[i];

			size_t code_length = ATOULL(buffer);
			std::vector<byte> thread_code;

			for (size_t j = 0; j < code_length; i++, j++)
				thread_code.push_back((*file_content)[i]);
			i--;
			delete[] buffer;

			threads_code.push_back(std::make_tuple<std::vector<byte>&, size_t&>(thread_code, thread_id));
		}
	}

	// fetch main code
	if (main_found) {
		byte* buffer = new byte[sizeof(size_t)];
		i += 8;
		
		for (size_t j = 0; j < sizeof(size_t); i++, j++)
			buffer[j] = (*file_content)[i];

		size_t main_len = ATOULL(buffer);
		delete[] buffer;

		for (size_t j = 0; j < main_len; i++, j++)
			main_code.push_back((*file_content)[i]);
	}

	// Set output
	if (!proc_threads.empty()) {
		proc_threads.clear();
	}
	for (auto thread_code : threads_code)
		proc_threads.push_back(thread_code);

	return main_code;
}

void executeFile(const std::vector<byte>& main_code, const std::vector<std::tuple<std::vector<byte>, size_t>>& proc_threads,
		const size_t& req_mem) {
	std::vector<action> main_actions;
	std::vector<std::tuple<std::vector<action>, size_t>> threads_actions;

	regs* registers = new regs;
	memory* mem = new memory(registers);

	for (size_t i = 0; i < proc_threads.size(); i++) {
		auto thread_header = std::get<0>(proc_threads[i]);
		size_t thread_id = std::get<1>(proc_threads[i]);

		std::vector<action> thread_actions = decodeByteArray(&thread_header, mem);
		threads_actions.push_back(std::make_tuple<std::vector<action>&, size_t&>(thread_actions, thread_id));
	}

	std::vector<byte> temp_main_code = main_code;
	main_actions = decodeByteArray(&temp_main_code, mem);

	actions_engine _actions_engine = actions_engine(mem, registers);
	process _proc = process(_actions_engine, main_actions);

	mem->_MRSZ(req_mem);

	for (size_t i = 0; i < threads_actions.size(); i++) {
		_proc.addThread(std::get<0>(threads_actions[i]), std::get<1>(threads_actions[i]));
	}

	_proc.updateStackRegs();
	_proc.start();

	delete registers;

	mem->destroy();
	delete mem;
}

void runFile(std::string filename) {
	std::ifstream input(filename, std::ios::binary);
	std::vector<byte>* exe_bytes = new std::vector<byte>(
		(std::istreambuf_iterator<char>(input)),
		(std::istreambuf_iterator<char>()));
	
	std::vector<std::tuple<std::vector<byte>, size_t>> threads_code;
	size_t req_mem = 0;

	std::vector<byte> main_code = decodeFile(exe_bytes, threads_code, req_mem);
	executeFile(main_code, threads_code, req_mem);

	delete exe_bytes;
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		std::cerr << "Usage: vexe <executableFile>" << std::endl;
		return 1;
	}

	std::string inputfile = argv[1];

	runFile(inputfile);
}