#include <array>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_set>

#if defined(__linux__)
#include <cstring>
#include <stdio.h>
#endif

#include "../CursesWrapper/wrapper.hpp"

#include "../utility.h"
#include "../Actions/threading.h"
#include "../Actions/v_engine.h"
#include "../Memory/memory_decl.h"
#include "action_parser.h"
#include "variables.h"

extern std::unordered_set<std::string> regs_names;

namespace {
	std::unordered_set<std::string> num_registers = {
		"AX",
		"BX",
		"CX",
		"DX",

		"EAX",
		"EBX",
		"ECX",
		"EDX",

		"RAX",
		"RBX",
		"RCX",
		"RDX",

		"RBP",
		"RSP",
		"RDI",
		"RSI"
	};
	std::unordered_set<std::string> fp_registers = {
		"FPR0",
		"FPR1",
		"FPR2",
		"FPR3",

		"EFPR0",
		"EFPR1",
		"EFPR2",
		"EFPR3",

		"RFPR0",
		"RFPR1",
		"RFPR2",
		"RFPR3",
	};
	std::unordered_map<std::string, comn_registers> str_to_reg_table = {
		{"AX", comn_registers::AX},
		{"BX", comn_registers::BX},
		{"CX", comn_registers::CX},
		{"DX", comn_registers::DX},

		{"EAX", comn_registers::EAX},
		{"EBX", comn_registers::EBX},
		{"ECX", comn_registers::ECX},
		{"EDX", comn_registers::EDX},

		{"RAX", comn_registers::RAX},
		{"RBX", comn_registers::RBX},
		{"RCX", comn_registers::RCX},
		{"RDX", comn_registers::RDX},

		{"RBP", comn_registers::RBP},
		{"RSP", comn_registers::RSP},
		{"RDI", comn_registers::RDI},
		{"RSI", comn_registers::RSI},

		{"FPR0", comn_registers::FPR0},
		{"FPR1", comn_registers::FPR1},
		{"FPR2", comn_registers::FPR2},
		{"FPR3", comn_registers::FPR3},

		{"EFPR0", comn_registers::EFPR0},
		{"EFPR1", comn_registers::EFPR1},
		{"EFPR2", comn_registers::EFPR2},
		{"EFPR3", comn_registers::EFPR3},

		{"RFPR0", comn_registers::RFPR0},
		{"RFPR1", comn_registers::RFPR1},
		{"RFPR2", comn_registers::RFPR2},
		{"RFPR3", comn_registers::RFPR3}
	};
	std::array<std::string, 6> multi_args_ops = {
		"set",
		"mov",

		"add",
		"sub",
		"mul",
		"div"
	};

	inline void SETREG(comn_registers reg, std::string str_reg, unsigned char*& buffer,
		variables_decl*& vars, std::map<std::string, std::shared_ptr<void>>& ptr_table) {
		ULLTOA((size_t)reg, &buffer);
		vars->set(str_reg, buffer, sizeof(size_t));
		delete[] buffer;
		ptr_table[str_reg] = std::make_shared<arg_tuple>(vars->getVarInfos(str_reg));
	}
	comn_registers STOREGID(const std::string& str) {
		if (num_registers.find(str) == num_registers.end() && fp_registers.find(str) == fp_registers.end()) {
			nstd::ncout << "Can't use special register on an operation that requires a real register" << nstd::nendl;
			std::exit(1);
			return (comn_registers)0; // Disables warnings
		}

		return str_to_reg_table[str];
	}

	inline void std_margs_operations(const std::vector<std::string>& line, const virtual_actions& vaction,
			std::vector<std::tuple<virtual_actions, uint8_t>>& converted) {
		std::stringstream ss(line[0].substr(3));
		std::string dest, src;
		ss >> dest >> std::ws >> src;

		if (regs_names.find(dest) == regs_names.end()) {
			nstd::ncout << "(" << line[1] << ") unknown operands : " << dest << ", " << line[1] << nstd::nendl;
			std::exit(1);
			return;
		}

		converted.emplace_back(vaction, (uint8_t)STOREGID(dest));
	}
	
	bool is_margs_op(std::string op) {
		for (std::string s : multi_args_ops) {
			if (op.starts_with(s))
				return true;
		}
		return false;
	}
}

void process_memory::set(variables_decl* var) {
	std::vector<code_file_decl_form> headers = var->getVariablesTree();

	for (size_t i = 0; i < headers.size(); i++)
		this->data_ptrs[headers[i].decl_name] = std::make_shared<arg_tuple>(var->getVarInfos(headers[i].decl_name));
}
void process_memory::setTags(variables_decl* vars) {
	std::vector<tag_decl_form> tags_headers = vars->getTagsTree();
	std::stringstream ss;

	for (size_t i = 0; i < tags_headers.size(); i++) {
		unsigned char* uc_t = nullptr;
		ULLTOA(tags_headers[i].value, &uc_t);

		ss << std::hex << vars->tag_vars_count;
		std::string tag_name = RES_TAG_VAR_TAG + ss.str();
		vars->set(tag_name, uc_t, sizeof(size_t));

		this->data_ptrs[tags_headers[i].tagname] = std::make_shared<arg_tuple>(vars->getVarInfos(tag_name));
		std::stringstream().swap(ss);

		vars->tag_vars_count += 1;
		delete[] uc_t;
	}
}
void process_memory::setRegisters(variables_decl* vars) {
	unsigned char* uc_r = nullptr;

	SETREG(comn_registers::AX, "AX", uc_r, vars, this->data_ptrs);
	SETREG(comn_registers::BX, "BX", uc_r, vars, this->data_ptrs);
	SETREG(comn_registers::CX, "CX", uc_r, vars, this->data_ptrs);
	SETREG(comn_registers::DX, "DX", uc_r, vars, this->data_ptrs);

	SETREG(comn_registers::EAX, "EAX", uc_r, vars, this->data_ptrs);
	SETREG(comn_registers::EBX, "EBX", uc_r, vars, this->data_ptrs);
	SETREG(comn_registers::ECX, "ECX", uc_r, vars, this->data_ptrs);
	SETREG(comn_registers::EDX, "EDX", uc_r, vars, this->data_ptrs);

	SETREG(comn_registers::RAX, "RAX", uc_r, vars, this->data_ptrs);
	SETREG(comn_registers::RBX, "RBX", uc_r, vars, this->data_ptrs);
	SETREG(comn_registers::RCX, "RCX", uc_r, vars, this->data_ptrs);
	SETREG(comn_registers::RDX, "RDX", uc_r, vars, this->data_ptrs);

	SETREG(comn_registers::RBP, "RBP", uc_r, vars, this->data_ptrs);
	SETREG(comn_registers::RSP, "RSP", uc_r, vars, this->data_ptrs);

	SETREG(comn_registers::RDI, "RDI", uc_r, vars, this->data_ptrs);
	SETREG(comn_registers::RSI, "RSI", uc_r, vars, this->data_ptrs);

	SETREG(comn_registers::CR, "CR", uc_r, vars, this->data_ptrs);

	SETREG(comn_registers::FPR0, "FPR0", uc_r, vars, this->data_ptrs);
	SETREG(comn_registers::FPR1, "FPR1", uc_r, vars, this->data_ptrs);
	SETREG(comn_registers::FPR2, "FPR2", uc_r, vars, this->data_ptrs);
	SETREG(comn_registers::FPR3, "FPR3", uc_r, vars, this->data_ptrs);

	SETREG(comn_registers::EFPR0, "EFPR0", uc_r, vars, this->data_ptrs);
	SETREG(comn_registers::EFPR1, "EFPR1", uc_r, vars, this->data_ptrs);
	SETREG(comn_registers::EFPR2, "EFPR2", uc_r, vars, this->data_ptrs);
	SETREG(comn_registers::EFPR3, "EFPR3", uc_r, vars, this->data_ptrs);

	SETREG(comn_registers::RFPR0, "RFPR0", uc_r, vars, this->data_ptrs);
	SETREG(comn_registers::RFPR1, "RFPR1", uc_r, vars, this->data_ptrs);
	SETREG(comn_registers::RFPR2, "RFPR2", uc_r, vars, this->data_ptrs);
	SETREG(comn_registers::RFPR3, "RFPR3", uc_r, vars, this->data_ptrs);
}

std::shared_ptr<void> process_memory::getVarPtr(const std::string& var_name) {
	if (this->data_ptrs.count(var_name))
		return this->data_ptrs[var_name];
	else {
		nstd::ncout << "Warning: Symbol '" << var_name << "' unrecognized, replaced by NULL statement" << nstd::nendl;
		return std::make_shared<size_t>(0);
	}
}
void process_memory::setVarPtr(const std::string& var_name, const std::shared_ptr<arg_tuple>& new_ptr) {
	this->data_ptrs[var_name] = new_ptr;
}

std::map<std::string, virtual_actions> symbols_converter =
{
#pragma region map_decl
	{"int", virtual_actions::_int},

	{"inc", virtual_actions::inc},
	{"dec", virtual_actions::dec},

	{"toString", virtual_actions::toString},
	{"castreg", virtual_actions::castreg},

	{"sdzs", virtual_actions::sdzs},

	{"push", virtual_actions::push},
	{"pop", virtual_actions::pop},

	{"pushCR", virtual_actions::pushCR},
	{"popCR", virtual_actions::popCR},

	{"pushFP", virtual_actions::pushFP},
	{"popFP", virtual_actions::popFP},

	{"movsm", virtual_actions::movsm},
	{"movgm", virtual_actions::movgm},

	{"movsmCR", virtual_actions::movsmCR},
	{"movgmCR", virtual_actions::movgmCR},

	{"movsmFP", virtual_actions::movsmFP},
	{"movgmFP", virtual_actions::movgmFP},

	{"not", virtual_actions::_not},
	{"and", virtual_actions::_and},
	{"or",  virtual_actions::_or },
	{"xor", virtual_actions::_xor},
	{"shl", virtual_actions::_shl},
	{"shr", virtual_actions::_shr},

	{"log", virtual_actions::_log},
	{"log2", virtual_actions::_log2},
	{"log10", virtual_actions::_log10},
	{"pow", virtual_actions::_pow},

	{"dlog", virtual_actions::_dlog},
	{"dlog2", virtual_actions::_dlog2},
	{"dlog10", virtual_actions::_dlog10},
	{"dpow", virtual_actions::_dpow},

	{"jmp", virtual_actions::jmp},
	{"cmp", virtual_actions::cmp},
	{"je", virtual_actions::je},
	{"jne", virtual_actions::jne},
	{"jl", virtual_actions::jl},
	{"jg", virtual_actions::jg},
	{"jle", virtual_actions::jle},
	{"jge", virtual_actions::jge},
	{"cmpdbl", virtual_actions::cmpdbl},

	{"gca", virtual_actions::gca},
	{"cli", virtual_actions::cli},
	{"sti", virtual_actions::sti},
	{"hlt", virtual_actions::hlt},

	{"call", virtual_actions::call},
	{"lcall", virtual_actions::lcall},
	{"ret", virtual_actions::ret},
	{"svcall", virtual_actions::svcall},
	{"rscall", virtual_actions::rscall},

	{"nopen", virtual_actions::nopen},
	{"nclose", virtual_actions::nclose},
	{"nget", virtual_actions::nget},
	{"nsend", virtual_actions::nsend},
	{"nhrecv", virtual_actions::nhrecv},
	{"ncrtep", virtual_actions::ncrtep},
	{"nselep", virtual_actions::nselep},

	{"rfread", virtual_actions::rfread},
	{"rfwrite", virtual_actions::rfwrite},
	{"rflen", virtual_actions::rflen}
#pragma endregion
};

static std::unordered_set<std::string> included_headers;

static std::unordered_set<std::string> global_toInclude;
static std::vector<std::string> global_include_list;

static bool mem_resizing_req = false;
static bool is_mem_resized = false;

std::string processCompiletimeArg(std::string argument, variables_decl* vars, memory* mem) {
	std::string content = argument.substr(3, argument.size() - 5);
	std::string prefix = std::string(1, content[0]);
	if (prefix == "_") {
		if (content.size() > 2) {
			prefix += content[1];
		}
	}

	try {
		if (prefix == "N") { // Unsigned number
			std::string s_val = content.substr(1);

			size_t value;
			std::stringstream ss(s_val);
			ss >> value;
			std::stringstream().swap(ss);
			
			ss << std::hex << vars->sys_vars_count;
			std::string var_name = RES_VAR_TAG + ss.str();
			std::stringstream().swap(ss);

			code_file_decl_form decl_form;
			decl_form.decl_attr = "defined";
			decl_form.decl_name = var_name;
			decl_form.decl_type = "unsigned number";
			ss << value;
			ss >> decl_form.decl_value;
			std::stringstream().swap(ss);

			unsigned char* uc_n = nullptr;
			ULLTOA(value, &uc_n);

			vars->set(var_name, uc_n, sizeof(size_t));
			vars->setVariablesTree(decl_form);
			vars->sys_vars_count += 1;

			delete[] uc_n;

			return var_name;
		}
		else if (prefix == "_N") { // Signed number
			std::string s_val = content.substr(2);

			long long value;
			std::stringstream ss(s_val);
			ss >> value;
			std::stringstream().swap(ss);

			ss << std::hex << vars->sys_vars_count;
			std::string var_name = RES_VAR_TAG + ss.str();
			std::stringstream().swap(ss);

			code_file_decl_form decl_form;
			decl_form.decl_attr = "defined";
			decl_form.decl_name = var_name;
			decl_form.decl_type = "signed number";
			ss << value;
			ss >> decl_form.decl_value;
			std::stringstream().swap(ss);

			unsigned char* uc_n = nullptr;
			ULLTOA(value, &uc_n);

			vars->set(var_name, uc_n, sizeof(size_t));
			vars->setVariablesTree(decl_form);
			vars->sys_vars_count += 1;

			delete[] uc_n;

			return var_name;
		}
		else if (prefix == "D") {
			std::string s_val = content.substr(1);

			long double value;
			std::stringstream ss(s_val);
			ss >> std::setprecision(std::numeric_limits<long double>::digits10) >> value;
			std::stringstream().swap(ss);

			ss << std::hex << vars->sys_vars_count;
			std::string var_name = RES_VAR_TAG + ss.str();
			std::stringstream().swap(ss);

			code_file_decl_form decl_form;
			decl_form.decl_attr = "defined";
			decl_form.decl_name = var_name;
			decl_form.decl_type = "double";
			ss << value;
			ss >> decl_form.decl_value;
			std::stringstream().swap(ss);

			unsigned char* uc_d = nullptr;
			LDTOA(value, &uc_d);

			vars->set(var_name, uc_d, sizeof(long double));
			vars->setVariablesTree(decl_form);
			vars->sys_vars_count += 1;

			delete[] uc_d;

			return var_name;
		}
		else if (prefix == "C") {
			std::string s_val = content.substr(2, content.size() - 3); // Char compiletime decl has a form of ${{C'<your char>'}}.
			
			char value;
			std::stringstream ss(s_val);
			ss.get(value);
			std::stringstream().swap(ss);

			ss << std::hex << vars->sys_vars_count;
			std::string var_name = RES_VAR_TAG + ss.str();
			std::stringstream().swap(ss);

			code_file_decl_form decl_form;
			decl_form.decl_attr = "defined";
			decl_form.decl_name = var_name;
			decl_form.decl_type = "char";
			decl_form.decl_value = std::string(1, value);
			std::stringstream().swap(ss);

			unsigned char* uc_c = new unsigned char[1];
			uc_c[0] = value;

			vars->set(var_name, uc_c, 1);
			vars->setVariablesTree(decl_form);
			vars->sys_vars_count += 1;

			delete[] uc_c;

			return var_name;
		}
		else if (prefix == "S") {
			std::string value = content.substr(2, content.size() - 3); // String compiletime decl has a form of ${{S"<your string>"}}.
			std::stringstream ss;

			ss << std::hex << vars->sys_vars_count;
			std::string var_name = RES_VAR_TAG + ss.str();
			std::stringstream().swap(ss);

			code_file_decl_form decl_form;
			decl_form.decl_attr = "defined";
			decl_form.decl_name = var_name;
			decl_form.decl_type = "string";
			decl_form.decl_value = value;
			std::stringstream().swap(ss);

			size_t str_size = value.size() + 1;

			auto buffer = std::make_unique<uint8_t[]>(str_size);
#if defined(ISWIN)
			memcpy_s(buffer.get(), str_size, value.c_str(), str_size);
#else
			std::memcpy(buffer.get(), value.c_str(), str_size);
#endif

			size_t addr = mem->_SDZTOP();
			mem->_SDZS(buffer.get(), str_size);

			unsigned char* buffer_2 = nullptr;
			ULLTOA(addr, &buffer_2);

			vars->set(var_name, buffer_2, sizeof(size_t));
			vars->setVariablesTree(decl_form);
			vars->sys_vars_count += 1;

			delete[] buffer_2;

			return var_name;
		}
		else {
			return "NULL";
		}
	}
	catch (const std::out_of_range&) {
		nstd::ncout << "Invalid compiletime value declaration: " << content << nstd::nendl;
		nstd::ncout << "Replaced by NULL" << nstd::nendl;
		return "NULL";
	}
}
std::vector<std::vector<std::string>> parseCodeLines(std::string filename, variables_decl* vars, memory* const& memptr) {
	std::vector<std::string> toInclude;

	if (!included_headers.count(filename))
		included_headers.insert(filename);
	
	std::ifstream file(filename);
	std::vector<std::vector<std::string>> parsed;
	std::string line, prev_line;

	if (file) {
		while (!file.eof()) {
			std::getline(file, line);

			if (line.empty() || line[0] == ';')
				continue;

			if (!line.rfind("decltag ", 0)) {
				std::string tagname = line.substr(8);
				parsed.push_back(std::vector<std::string>({ "decltag", tagname, "0"}));
				continue;
			}
			else if (!line.rfind("[include] ", 0)) {
				std::string included = line.substr(10);
				std::string localpath = std::filesystem::relative(filename).replace_filename(included).string();

				if (!included_headers.count(localpath)) {
					toInclude.push_back(localpath);
					global_include_list.push_back(localpath);
				}
				continue;
			}
			else if (!line.rfind("[MRES] ", 0) && !is_mem_resized) {
				// Has form of [MRES] <new memory size>, with new size an unsigned number
				std::string s_new_size = line.substr(7);
				size_t new_size = std::stoull(s_new_size);
				memptr->_MRSZ(new_size);
				is_mem_resized = true;
			}
			else if (line.starts_with("[QWORD]") || line.starts_with("[DWORD]") || line.starts_with("[WORD]") || line.starts_with("[BYTE]")) {
				prev_line = line;
				continue;
			}

			std::stringstream ss(line);
			std::string action, argument;
			ss >> action >> std::ws >> argument;
			argument = line.substr(action.size() + 1);

			if (!argument.find("${{", 0) && argument.find("}}", argument.size() - 2) > 2
				&& argument.find("}}", argument.size() - 2 < argument.size())) {
				argument = processCompiletimeArg(argument, vars, memptr);
			}

			uint8_t opt_flag = 0b000;
			if (prev_line.starts_with("[QWORD]"))
				opt_flag = 0b100;
			else if (prev_line.starts_with("[DWORD]"))
				opt_flag = 0b011;
			else if (prev_line.starts_with("[WORD]"))
				opt_flag = 0b010;
			else if (prev_line.starts_with("[BYTE]"))
				opt_flag = 0b001;

			parsed.push_back(std::vector<std::string>({action, argument, std::to_string(opt_flag)}));
			prev_line = line;
		}

		for (size_t i = 0; i < toInclude.size(); i++) {
			if (!global_toInclude.count(toInclude[i])) {
				std::vector<std::vector<std::string>> loaded = parseCodeLines(toInclude[i], vars, memptr);

				for (size_t j = 0; j < loaded.size(); j++) {
					parsed.push_back(loaded[j]);
				}
				global_toInclude.insert(toInclude[i]);
			}
		}

		return parsed;
	}
	else {
		nstd::ncout << "Unable to read secified code file" << nstd::nendl;
		return parsed;
	}
}

std::vector<std::tuple<virtual_actions, uint8_t>> convertSymbols(std::vector<std::vector<std::string>> parsed) {
	std::vector<std::tuple<virtual_actions, uint8_t>> converted;
	converted.reserve(parsed.size());

	for (size_t i = 0; i < parsed.size(); i++) {
		if (parsed[i][0].starts_with("set")) {
			std::stringstream ss(parsed[i][0].substr(3));
			std::string dest, src;
			ss >> dest >> std::ws >> src;

			if (regs_names.find(dest) == regs_names.end()) {
				nstd::ncout << "set: unknown operands: " << dest << ", " << parsed[i][1] << nstd::nendl;
				std::exit(1);
				return converted; // won't be executed, used to disable warnings on some compilers
			}

			uint8_t sec_opr;
			if (dest == "CR")
				sec_opr = (uint8_t)comn_registers::CR;
			else
				sec_opr = (uint8_t)STOREGID(dest);
			converted.emplace_back(virtual_actions::set, sec_opr);
		}
		else if (parsed[i][0].starts_with("mov") && !parsed[i][0].starts_with("movsm") && !parsed[i][0].starts_with("movgm"))
			std_margs_operations(parsed[i], virtual_actions::mov, converted);
		else if (parsed[i][0].starts_with("mul"))
			std_margs_operations(parsed[i], virtual_actions::mul, converted);
		else if (parsed[i][0].starts_with("div"))
			std_margs_operations(parsed[i], virtual_actions::div, converted);
		else if (parsed[i][0].starts_with("add"))
			std_margs_operations(parsed[i], virtual_actions::add, converted);
		else if (parsed[i][0].starts_with("sub"))
			std_margs_operations(parsed[i], virtual_actions::sub, converted);
		else
			converted.emplace_back(symbols_converter[parsed[i][0]], (uint8_t)std::stoul(parsed[i][2]));		
	}

	return converted;
}
void purgeParsed(std::vector<std::tuple<virtual_actions, uint8_t>> *converted, std::vector<std::vector<std::string>> *parsed) {
	for (size_t i = 0; i < converted->size(); i++) {
		const auto [vaction, vopt] = (*converted)[i];

		if ((size_t)(vaction) == 0 || vaction < virtual_actions::_int) {
			parsed->erase(parsed->begin() + i);
			converted->erase(converted->begin() + i);
			i--;
		}
	}
}

std::vector<std::vector<std::string>> makeCleanedParsed(std::string filename, bool main = true) {
	std::ifstream file(filename);
	std::vector<std::vector<std::string>> parsed;
	std::string line;

	if (file) {
		while (!file.eof()) {
			std::getline(file, line);

			if (line.empty() || line[0] == ';')
				continue;

			if (!line.rfind("decltag ", 0)) {
				std::string tagname = line.substr(8);
				parsed.push_back(std::vector<std::string>({"decltag", tagname}));
				continue;
			}
			else if (!line.rfind("decl ", 0))
				continue;

			std::stringstream ss(line);
			std::string action, argument;
			ss >> action >> std::ws >> argument;

			parsed.push_back(std::vector<std::string>({action, argument}));
		}

		if (main) {
			for (size_t i = 0; i < global_include_list.size(); i++) {
				std::vector<std::vector<std::string>> loaded = makeCleanedParsed(global_include_list[i], false);

				for (size_t j = 0; j < loaded.size(); j++) {
					parsed.push_back(loaded[j]);
				}
			}
		}

		return parsed;
	}
	else {
		nstd::ncout << "Unable to read secified code file" << nstd::nendl;
		return parsed;
	}
}

void finalizeTags(std::vector<std::vector<std::string>> cleaned_parsed, variables_decl* vars) {
	for (size_t i = 0; i < cleaned_parsed.size(); i++) {
		if (cleaned_parsed[i][0] == "decltag") {
			vars->setTag(cleaned_parsed[i][1], i - 1);

			tag_decl_form decl_form;
			decl_form.tagname = cleaned_parsed[i][1];
			decl_form.value = i - 1;

			vars->setTagsBranch(decl_form);
			cleaned_parsed.erase(cleaned_parsed.begin() + i);
			i--;

			if (i >= cleaned_parsed.size()) {
				break;
			}
		}
		if (((size_t)(symbols_converter[cleaned_parsed[i][0]]) == 0 || symbols_converter[cleaned_parsed[i][0]]  < virtual_actions::_int) &&
				!is_margs_op(cleaned_parsed[i][0])) {
			cleaned_parsed.erase(cleaned_parsed.begin() + i);
			i--;
		}
	}
}
std::vector<std::shared_ptr<void>> convertVariables(std::vector<std::vector<std::string>> cleaned_parsed, 
		const std::vector<std::tuple<virtual_actions, uint8_t>>& conv_actions, process_memory* p_mem) {
	std::vector<std::shared_ptr<void>> arguments;

	for (size_t i = 0; i < cleaned_parsed.size(); i++) {
		const uint8_t& vopt = std::get<1>(conv_actions[i]);
		if (cleaned_parsed[i][1] == "0" || cleaned_parsed[i][1] == "NULL")
			arguments.push_back(NULL);
		else {
			const auto [vaddr, vsize, fvopt] = *std::static_pointer_cast<arg_tuple>(p_mem->getVarPtr(cleaned_parsed[i][1]));
			p_mem->setVarPtr(cleaned_parsed[i][1], std::make_shared<arg_tuple>(vaddr, vsize, vopt));
			arguments.push_back(p_mem->getVarPtr(cleaned_parsed[i][1]));
		}
	}

	return arguments;
}

void build_process(std::string filename, process* out_proc, engine* engine, process_memory* out_mem, memory*& mem) {
	variables_decl vars = build_variables_decl_tree(filename, mem);
	std::vector<std::vector<std::string>> parsed = parseCodeLines(filename, &vars, mem);
	std::vector<std::tuple<virtual_actions, uint8_t>> converted_actions = convertSymbols(parsed);

	purgeParsed(&converted_actions, &parsed);
	finalizeTags(makeCleanedParsed(filename), &vars);

	out_mem->set(&vars);
	out_mem->setRegisters(&vars);
	out_mem->setTags(&vars);

	std::vector<std::shared_ptr<void>> converted_arguments = convertVariables(parsed, converted_actions, out_mem);

	for (size_t i = 0; i < converted_actions.size(); i++)
		out_proc->addAction(std::get<0>(converted_actions[i]), converted_arguments[i]);
	
	if (mem_resizing_req && !is_mem_resized)
		is_mem_resized = true;
}
std::vector<action> build_actions_only(std::string filename, process_memory* out_mem, memory*& mem) {
	variables_decl vars = build_variables_decl_tree(filename, mem);
	std::vector<std::vector<std::string>> parsed = parseCodeLines(filename, &vars, mem);
	std::vector<std::tuple<virtual_actions, uint8_t>> converted_actions = convertSymbols(parsed);

	purgeParsed(&converted_actions, &parsed);
	finalizeTags(makeCleanedParsed(filename), &vars);

	out_mem->set(&vars);
	out_mem->setRegisters(&vars);
	out_mem->setTags(&vars);

	std::vector<std::shared_ptr<void>> converted_arguments = convertVariables(parsed, converted_actions, out_mem);
	std::vector<action> out_actions;

	for (size_t i = 0; i < converted_actions.size(); i++)
		out_actions.push_back(action(std::get<0>(converted_actions[i]), converted_arguments[i]));
		 
	return out_actions;
}