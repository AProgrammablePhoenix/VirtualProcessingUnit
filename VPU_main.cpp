﻿#include <iostream>
#include <string>

#include "./Registers/regs_decl.h"
#include "./Registers/registers_symbols.h"
#include "./Memory/memory_decl.h"
#include "./Memory/memory_symbols.h"
#include "./Actions/actions_symbols.h"
#include "./Actions/threading.h"
#include "./Actions/v_engine.h"
#include "./Compiler/action_parser.h"
#include "./Compiler/variables.h"

std::vector<regs*> registers = std::vector<regs*>();
std::vector<memory*> memory_table = std::vector<memory*>();
std::vector<actions_engine*> a_engines = std::vector<actions_engine*>();
std::vector<process*> processes = std::vector<process*>();
std::vector<process_memory*> processes_mem = std::vector<process_memory*>();

std::vector<std::string> scripts = std::vector<std::string>();

namespace {
	void allocateNewProcessMem() {
		regs* regs_ptr = new regs;
		memory* mem_ptr = new memory(regs_ptr);
		process_memory* proc_mem = new process_memory;

		if (!regs_ptr || !mem_ptr || !proc_mem) {
			std::cout << "Error while allocating new process memory" << std::endl;
			delete regs_ptr;
			delete mem_ptr;
			delete proc_mem;
		}

		registers.push_back(regs_ptr);
		memory_table.push_back(mem_ptr);
		processes_mem.push_back(proc_mem);
	}
	void allocateActionsEngine(memory* mem, regs* p_regs) {
		actions_engine* engine_ptr = new actions_engine(mem, p_regs);

		if (!engine_ptr) {
			std::cout << "Error while allocating new process engine memory" << std::endl;
			delete engine_ptr;
		}

		a_engines.push_back(engine_ptr);
	}
	void allocateProcess(const actions_engine& engine) {
		process* new_proc = new process(engine);
		if (!new_proc) {
			std::cout << "Error while allocating new process instance memory" << std::endl;
			delete new_proc;
		}

		processes.push_back(new_proc);
	}
	void clearMemory(engine& threadin_engine) {
		for (size_t i = 0; i < registers.size(); i++) {
			if (registers[i])
				delete registers[i];

			if (memory_table[i]) {
				memory_table[i]->destroy();
				delete memory_table[i];
			}

			actions_engine::has_been_destoryed();

			if (processes[i])
				delete processes_mem[i];
		}
		for (size_t i = 0; i < a_engines.size(); i++) {
			if (a_engines[i])
				delete a_engines[i];
		}
		for (size_t i = 0; i < processes.size(); i++) {
			if (processes[i])
				delete processes[i];
		}

		threadin_engine.destroy();
	}
	int processInput(const std::string& input, engine& threading_engine, int& status) {
		if (!input.rfind("load", 0)) {
			if (!status) {
				std::stringstream ss(input);
				std::string filename, buffer;
				ss >> buffer >> std::ws >> filename;

				scripts.push_back(filename);
				allocateNewProcessMem();

				return 0;
			}

			std::cout << "Operation not allowed: loaded scripts have been built" << std::endl;
			return -1;
		}
		else if (input == "build") {
			if (!status) {
				for (unsigned long long i = 0; i < scripts.size(); i++) {
					allocateActionsEngine(memory_table[i], registers[i]);
					allocateProcess(*a_engines[i]);
					build_process(scripts[i], processes[i], &threading_engine, processes_mem[i], memory_table[i]);
				}

				return 2;
			}

			std::cout << "Operation not allowed: loaded scripts have already been built" << std::endl;
			return -1;
		}
		else if (input == "start") {
			if (status == 1) {
				threading_engine = engine(*processes[0]);

				for (size_t i = 1; i < processes.size(); i++)
					threading_engine.pushProcess(*processes[i]);

				threading_engine.setThreading(true);
				threading_engine.start();

				return 3;
			}

			std::cout << "Operation not allowed: you haven't built any script" << std::endl;
			return -1;
		}
		else if (input == "exit") {
			clearMemory(threading_engine);
			return 1;
		}
		else {
			std::cout << "Unknown command" << std::endl << std::endl;
			return status;
		}
	}

	inline void display_help() {
		std::cout << "usage: vpu -sf <source file> [-h|--help]" << std::endl;
	}
}

int main(int argc, char* argv[])
{
	engine threading_engine = engine();

	if (argc < 2) {
		std::cout << " ____________________________________ " << std::endl;
		std::cout << "|  ________________________________  |" << std::endl;
		std::cout << "| |     Virtual Processing Unit    | |" << std::endl;
		std::cout << "| |________________________________| |" << std::endl;
		std::cout << "|____________________________________|" << std::endl;

		std::cout << std::endl << "Engine usage: " << std::endl << std::endl;

		std::cout << "Load script: " << std::endl;
		std::cout << "\tload <filename>" << std::endl << std::endl;
		std::cout << "Build loaded scripts: " << std::endl;
		std::cout << "\tbuild" << std::endl << std::endl;
		std::cout << "Start built scripts: " << std::endl;
		std::cout << "\tstart" << std::endl << std::endl;
		std::cout << "Termminate program: " << std::endl;
		std::cout << "\texit" << std::endl << std::endl << std::endl;


		int status = 0;
		std::string input = "";
		std::string prompt = "Command to execute: ";
		int ret_code = 0;

		while (true) {
			std::cout << prompt;
			std::getline(std::cin, input);

			ret_code = processInput(input, threading_engine, status);
			if (ret_code == 0)
				continue;
			else if (ret_code == 1)
				break;
			else if (ret_code == 2) {
				status = 1;
				continue;
			}
			else if (ret_code == 3) {
				status = 2;
				continue;
			}
			else {
				continue;
			}
		}
	}
	else {
		std::string source_file;

		bool next_imm = false;
		bool next_sf = false;

		for (size_t i = 1; i < argc; i++) {
			if (next_imm && argv[i][0] == '-') {
				std::cerr << "Unable to parse immediate argument from a command line option: " << argv[i] << std::endl;
				return 1;
			}
			else if (next_imm && next_sf) {
				source_file = argv[i];
				next_imm ^= next_imm;
				next_sf ^= next_sf;
				continue;
			}

			if (argv[i] == std::string("-sf")) {
				next_imm = true;
				next_sf = true;
				continue;
			}
			else if (argv[i] == std::string("-h") || argv[i] == std::string("--help")) {
				display_help();
				return 0;
			}
			else {
				std::cout << "Non recognized option: " << argv[i] << std::endl;
				display_help();
				return 0;
			}
		}

		std::cout << std::endl;

		if (!source_file.empty()) {
			allocateNewProcessMem();
			allocateActionsEngine(memory_table[0], registers[0]);
			allocateProcess(*a_engines[0]);
			build_process(source_file, processes[0], &threading_engine, processes_mem[0], memory_table[0]);

			threading_engine = engine(*processes[0]);
			threading_engine.setThreading(false);
			threading_engine.start();

			clearMemory(threading_engine);
		}
		else {
			std::cerr << "No input file specified" << std::endl;
			return 1;
		}
	}

	return 0;
}