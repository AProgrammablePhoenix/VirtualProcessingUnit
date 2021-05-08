#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sstream>
#include <memory>

#include "../Actions/actions_symbols.h"
#include "../Actions/v_engine.h"
#include "../Actions/threading.h"
#include "variables.h"

struct process_memory {
public:
	void set(variables_decl* var);
	void setTags(variables_decl* vars);
	void setTagValue(std::string tagname, unsigned long long value);
	void set(std::string var_name, std::shared_ptr<void> data_ptr);
	void setRegisters();
	bool isTag(std::string tagname);

	std::shared_ptr<void> getVarPtr(std::string var_name);
	std::string getVarType(std::string var_name);
private:
	std::map<std::string, unsigned long long> unsigned_numbers;
	std::map<std::string, long long> signed_numbers;
	std::map<std::string, std::string> stored_strings;
	std::map<std::string, unsigned long long> stored_tags;
	std::map<std::string, std::shared_ptr<void>> data_ptrs;
};

void build_process(std::string filename, process* out_proc, engine* engine, process_memory* out_mem);
std::vector<action> build_actions_only(std::string filename, process_memory* out_mem);