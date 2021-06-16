#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>

#include "../Actions/actions_symbols.h"
#include "../Actions/threading.h"
#include "../Actions/v_engine.h"
#include "variables.h"

struct process_memory {
public:
	void set(variables_decl* var);
	void setTags(variables_decl* vars);
	void setRegisters();

	std::shared_ptr<void> getVarPtr(std::string var_name);
private:
	std::map<std::string, std::shared_ptr<void>> data_ptrs;
};

void build_process(std::string filename, process* out_proc, engine* engine, process_memory* out_mem, memory*& mem);
std::vector<action> build_actions_only(std::string filename, process_memory* out_mem, memory*& mem);