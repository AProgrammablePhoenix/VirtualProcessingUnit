#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_set>

#include "../Actions/actions_symbols.h"
#include "../Actions/threading.h"
#include "../Actions/v_engine.h"
#include "variables.h"

struct process_memory {
public:
	void set(variables_decl* var);
	void setTags(variables_decl* vars);
	void setRegisters(variables_decl* vars);

	std::shared_ptr<void> getVarPtr(const std::string& var_name);
	void setVarPtr(const std::string& var_name, const std::shared_ptr<arg_tuple>& new_ptr);
private:
	std::map<std::string, std::shared_ptr<void>> data_ptrs;
};

void build_process(std::string filename, process* out_proc, engine* engine, process_memory* out_mem, memory*& mem);
std::vector<action> build_actions_only(std::string filename, process_memory* out_mem, memory*& mem,
	std::vector<std::tuple<std::vector<action>, size_t>>& out_threads);