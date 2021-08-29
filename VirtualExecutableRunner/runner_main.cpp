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
#include "../VirtualAssembler/assembler.h"

namespace {
	inline void DCODE_FP_NMATHS(std::vector<action>& actions, const std::map<virtual_actions, byte>& map2ndOPC, 
			const std::vector<byte>& exe_code, size_t& iterator, const bool& has_op_arg, const nbyte& vopt, memory* const& mem) {
		size_t& i = iterator;
		++i;

		if (has_op_arg)
			++i;

		byte _fpr_opc = exe_code[i];
		virtual_actions real_op = findKeyByValue(map2ndOPC, _fpr_opc);
		++i;

		byte _reg = exe_code[i];
		size_t real_reg = (size_t)(findKeyByValue(fp_registers_set, _reg));

		byte* uc_n = nullptr;
		ULLTOA(real_reg, &uc_n);

		size_t addr = mem->_SDZTOP();
		size_t len = sizeof(size_t);
		mem->_SDZS(uc_n, len);

		delete[] uc_n;

		action _action{ real_op, std::make_shared<arg_tuple>(std::make_tuple(addr, len, vopt)) };
		actions.push_back(_action);
	}
}

std::vector<action> decodeByteArray(std::vector<uint8_t>* byteArray, memory*& mem) {
	std::vector<action> actions;
	const std::vector<uint8_t>& data = *byteArray;

	for (size_t i = 0; i < byteArray->size(); ++i) {
		nbyte opt_arg;
		bool has_opt_arg = true;

		if (opt_arg_ops.find(data[i]) != opt_arg_ops.end())
			opt_arg.raw_byte = data[i + 1];
		else
			has_opt_arg = false;

		if (data[i] == ops[virtual_actions::set]) {
			i += 2; // Skip optional arg byte and instruction byte
			action _action(virtual_actions::set, nullptr);

			if (comn_registers_table::is_num_reg((comn_registers)opt_arg.raw_byte)) {
				byte* b_value = new byte[sizeof(size_t) + 1];
				b_value[0] = data[i];
				++i;

				size_t reallen;

				if ((size_t)b_value[0] < sizeof(size_t)) {
					for (size_t j = 0; j < sizeof(size_t) - b_value[0]; ++j, ++i)
						b_value[j + 1] = data[i];
					--i;

					reallen = EXTDBA(b_value, sizeof(size_t) - b_value[0] + 1);
				}
				else {
					b_value[1] = data[i];
					reallen = EXTDBA(b_value, 2);
				}

				size_t addr = mem->_SDZTOP();
				mem->_SDZS(b_value, reallen);

				delete[] b_value;

				_action = action(virtual_actions::set, std::make_shared<arg_tuple>(std::make_tuple(addr, reallen, opt_arg)));
			}
			else if (comn_registers_table::is_fp_reg((comn_registers)opt_arg.raw_byte)) {
				size_t addr = mem->_SDZTOP();
				mem->_SDZS(byteArray->data() + i, sizeof(long double));
				i += sizeof(long double) - 1;

				_action = action(virtual_actions::set, std::make_shared<arg_tuple>(std::make_tuple(addr, sizeof(long double), opt_arg)));
			}
			else if ((comn_registers)opt_arg.raw_byte == comn_registers::CR) {
				byte _arg = data[i];

				byte* uc_c = new byte[1];
				uc_c[0] = _arg;

				size_t addr = mem->_SDZTOP();
				size_t len = 1;
				mem->_SDZS(uc_c, len);

				delete[] uc_c;

				_action = action(virtual_actions::set, std::make_shared<arg_tuple>(std::make_tuple(addr, len, opt_arg)));
			}
			else if ((comn_registers)opt_arg.raw_byte == comn_registers::SR) {
				byte* b_str_size = new byte[sizeof(size_t) + 1];
				b_str_size[0] = data[i];
				++i;

				size_t complen;

				if ((size_t)b_str_size[0] < sizeof(size_t)) {
					for (size_t j = 0; j < sizeof(size_t) - b_str_size[0]; ++j, ++i)
						b_str_size[j + 1] = data[i];

					complen = EXTDBA(b_str_size, sizeof(size_t) - b_str_size[0] + 1);
				}
				else {
					b_str_size[1] = data[i];
					complen = EXTDBA(b_str_size, 2);
				}

				size_t str_size = ATOULL(b_str_size);
				delete[] b_str_size;

				byte* b_str = new byte[str_size];
				for (size_t j = 0; j < str_size; ++i, ++j)
					b_str[j] = data[i];
				--i;

				size_t addr = mem->_SDZTOP();
				size_t& len = str_size;
				mem->_SDZS(b_str, len);

				delete[] b_str;

				_action = action(virtual_actions::set, std::make_shared<arg_tuple>(std::make_tuple(addr, len, opt_arg)));
			}
			else
				throw std::runtime_error("Specified an unknown operand to set operand\n");
			
			actions.push_back(_action);

			continue;
		}

		if (zero_args_opcodes.find(data[i]) != zero_args_opcodes.end()) {
			byte _op = data[i];
			if (has_opt_arg)
				++i;

			virtual_actions real_op = findKeyByValue(instructions_set, _op);

			action _action(real_op, nullptr);
			actions.push_back(_action);

			continue;
		}
		else if (uint64_args_opcodes.find(data[i]) != uint64_args_opcodes.end()) {
			byte _op = data[i];
			virtual_actions real_op = findKeyByValue(instructions_set, _op);
			++i;

			if (has_opt_arg)
				++i;

			byte* b_value = new byte[sizeof(size_t) + 1];
			b_value[0] = data[i];
			++i;;

			size_t reallen;

			if ((size_t)b_value[0] < sizeof(size_t)) {
				for (size_t j = 0; j < sizeof(size_t) - b_value[0]; ++j, ++i)
					b_value[j + 1] = data[i];
				--i;

				reallen = EXTDBA(b_value, sizeof(size_t) - b_value[0] + 1);
			}
			else {
				b_value[1] = data[i];
				reallen = EXTDBA(b_value, 2);
			}

			size_t addr = mem->_SDZTOP();
			mem->_SDZS(b_value, reallen);

			delete[] b_value;

			action _action(real_op, std::make_shared<arg_tuple>(std::make_tuple(addr, reallen, opt_arg)));
			actions.push_back(_action);

			continue;
		}
		else if (reg_args_opcodes.find((*byteArray)[i]) != reg_args_opcodes.end()) {
			byte _op = data[i];
			virtual_actions real_op = findKeyByValue(instructions_set, _op);
			++i;

			if (has_opt_arg)
				++i;

			size_t _reg = (size_t)data[i];

			byte* uc_n = nullptr;
			ULLTOA(_reg, &uc_n);

			size_t addr = mem->_SDZTOP();
			size_t len = sizeof(size_t);
			mem->_SDZS(uc_n, len);

			delete[] uc_n;

			action _action(real_op, std::make_shared<arg_tuple>(std::make_tuple(addr, len, opt_arg)));
			actions.push_back(_action);

			continue;
		}
	}

	return actions;
}

std::vector<byte> decodeFile(const std::vector<byte> *const file_content, size_t& req_mem) {
	std::vector<byte> main_code;

	size_t i = 0;

	byte* b_req_mem = new byte[sizeof(size_t)];
	for (; i < sizeof(size_t); ++i)
		b_req_mem[i] = (*file_content)[i];
	req_mem = ATOULL(b_req_mem);
	delete[] b_req_mem;

	byte* buffer = new byte[sizeof(size_t)];
		
	for (size_t j = 0; j < sizeof(size_t); ++i, ++j)
		buffer[j] = (*file_content)[i];

	size_t main_len = ATOULL(buffer);
	delete[] buffer;

	for (size_t j = 0; j < main_len; ++i, ++j)
		main_code.push_back((*file_content)[i]);

	return main_code;
}

void executeFile(const std::vector<byte>& main_code, const size_t& req_mem) {
	std::vector<action> main_actions;

	regs* registers = new regs;
	memory* mem = new memory(registers);

	std::vector<byte> temp_main_code = main_code;
	main_actions = decodeByteArray(&temp_main_code, mem);

	actions_engine _actions_engine = actions_engine(mem, registers);
	process _proc = process(_actions_engine, main_actions);

	mem->_MRSZ(req_mem);

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
	
	size_t req_mem = 0;

	std::vector<byte> main_code = decodeFile(exe_bytes, req_mem);
	executeFile(main_code, req_mem);

	delete exe_bytes;
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		nstd::ncout << "Usage: vexe <executableFile>" << nstd::nendl;
		return 1;
	}

	std::string inputfile = argv[1];

	runFile(inputfile);
}