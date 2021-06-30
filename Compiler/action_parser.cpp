#include <filesystem>
#include <fstream>
#include <iostream>
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

#include "../utility.h"
#include "../Actions/threading.h"
#include "../Actions/v_engine.h"
#include "../Memory/memory_decl.h"
#include "action_parser.h"
#include "variables.h"

inline void SETREG(registries_def reg, std::string str_reg, unsigned char*& buffer, 
		variables_decl*& vars, std::map<std::string, std::shared_ptr<void>>& ptr_table) {
	ULLTOA((size_t)reg, &buffer);
	vars->set(str_reg, buffer, sizeof(size_t));
	delete[] buffer;
	ptr_table[str_reg] = std::make_shared<std::tuple<size_t, size_t>>(vars->getVarInfos(str_reg));
}

inline void SETXREG(extra_registries xreg, std::string str_xreg, unsigned char*& buffer,
		variables_decl*& vars, std::map<std::string, std::shared_ptr<void>>& ptr_table) {
	ULLTOA((size_t)xreg, &buffer);
	vars->set(str_xreg, buffer, sizeof(size_t));
	delete[] buffer;
	ptr_table[str_xreg] = std::make_shared<std::tuple<size_t, size_t>>(vars->getVarInfos(str_xreg));
}

void process_memory::set(variables_decl* var) {
	std::vector<code_file_decl_form> headers = var->getVariablesTree();

	for (size_t i = 0; i < headers.size(); i++) {
		this->data_ptrs[headers[i].decl_name] = std::make_shared<std::tuple<size_t, size_t>>(var->getVarInfos(headers[i].decl_name));
	}
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

		this->data_ptrs[tags_headers[i].tagname] = std::make_shared<std::tuple<size_t, size_t>>(vars->getVarInfos(tag_name));
		std::stringstream().swap(ss);

		vars->tag_vars_count += 1;
		delete[] uc_t;
	}
}
void process_memory::setRegisters(variables_decl* vars) {
	unsigned char* uc_r = nullptr;

	SETREG(registries_def::AX, "AX", uc_r, vars, this->data_ptrs);
	SETREG(registries_def::BX, "BX", uc_r, vars, this->data_ptrs);
	SETREG(registries_def::CX, "CX", uc_r, vars, this->data_ptrs);
	SETREG(registries_def::DX, "DX", uc_r, vars, this->data_ptrs);

	SETREG(registries_def::EAX, "EAX", uc_r, vars, this->data_ptrs);
	SETREG(registries_def::EBX, "EBX", uc_r, vars, this->data_ptrs);
	SETREG(registries_def::ECX, "ECX", uc_r, vars, this->data_ptrs);
	SETREG(registries_def::EDX, "EDX", uc_r, vars, this->data_ptrs);

	SETREG(registries_def::RAX, "RAX", uc_r, vars, this->data_ptrs);
	SETREG(registries_def::RBX, "RBX", uc_r, vars, this->data_ptrs);
	SETREG(registries_def::RCX, "RCX", uc_r, vars, this->data_ptrs);
	SETREG(registries_def::RDX, "RDX", uc_r, vars, this->data_ptrs);

	SETXREG(extra_registries::SR, "SR", uc_r, vars, this->data_ptrs);
	SETXREG(extra_registries::CR, "CR", uc_r, vars, this->data_ptrs);
	SETXREG(extra_registries::DR, "DR", uc_r, vars, this->data_ptrs);
}

std::shared_ptr<void> process_memory::getVarPtr(std::string var_name) {
	if (this->data_ptrs.count(var_name)) {
		return this->data_ptrs[var_name];
	}
	else {
		std::cout << "Warning: Symbol '" << var_name << "' unrecognized, replaced by NULL statement" << std::endl;
		return std::make_shared<size_t>(0);
	}
}

std::map<std::string, virtual_actions> symbols_converter =
{
#pragma region map_decl
	{"int", virtual_actions::_int},

	{"getAX", virtual_actions::getAX},
	{"getBX", virtual_actions::getBX},
	{"getCX", virtual_actions::getCX},
	{"getDX", virtual_actions::getDX},

	{"setAX", virtual_actions::setAX},
	{"setBX", virtual_actions::setBX},
	{"setCX", virtual_actions::setCX},
	{"setDX", virtual_actions::setDX},

	{"getEAX", virtual_actions::getEAX},
	{"getEBX", virtual_actions::getEBX},
	{"getECX", virtual_actions::getECX},
	{"getEDX", virtual_actions::getEDX},

	{"setEAX", virtual_actions::setEAX},
	{"setEBX", virtual_actions::setEBX},
	{"setECX", virtual_actions::setECX},
	{"setEDX", virtual_actions::setEDX},

	{"getRAX", virtual_actions::getRAX},
	{"getRBX", virtual_actions::getRBX},
	{"getRCX", virtual_actions::getRCX},
	{"getRDX", virtual_actions::getRDX},

	{"setRAX", virtual_actions::setRAX},
	{"setRBX", virtual_actions::setRBX},
	{"setRCX", virtual_actions::setRCX},
	{"setRDX", virtual_actions::setRDX},

	{"setSR", virtual_actions::setSR},
	{"getSR", virtual_actions::getSR},

	{"setCR", virtual_actions::setCR},
	{"getCR", virtual_actions::getCR},

	{"setDR", virtual_actions::setDR},
	{"getDR", virtual_actions::getDR},

	{"movAX", virtual_actions::movAX},
	{"movBX", virtual_actions::movBX},
	{"movCX", virtual_actions::movCX},
	{"movDX", virtual_actions::movDX},

	{"movEAX", virtual_actions::movEAX},
	{"movEBX", virtual_actions::movEBX},
	{"movECX", virtual_actions::movECX},
	{"movEDX", virtual_actions::movEDX},

	{"movRAX", virtual_actions::movRAX},
	{"movRBX", virtual_actions::movRBX},
	{"movRCX", virtual_actions::movRCX},
	{"movRDX", virtual_actions::movRDX},

	{"inc", virtual_actions::inc},
	{"dec", virtual_actions::dec},
	{"incDR", virtual_actions::incDR},
	{"decDR", virtual_actions::decDR},

	{"mulAX", virtual_actions::mulAX},
	{"mulBX", virtual_actions::mulBX},
	{"mulCX", virtual_actions::mulCX},
	{"mulDX", virtual_actions::mulDX},

	{"mulEAX", virtual_actions::mulEAX},
	{"mulEBX", virtual_actions::mulEBX},
	{"mulECX", virtual_actions::mulECX},
	{"mulEDX", virtual_actions::mulEDX},

	{"mulRAX", virtual_actions::mulRAX},
	{"mulRBX", virtual_actions::mulRBX},
	{"mulRCX", virtual_actions::mulRCX},
	{"mulRDX", virtual_actions::mulRDX},

	{"divAX", virtual_actions::divAX},
	{"divBX", virtual_actions::divBX},
	{"divCX", virtual_actions::divCX},
	{"divDX", virtual_actions::divDX},

	{"divEAX", virtual_actions::divEAX},
	{"divEBX", virtual_actions::divEBX},
	{"divECX", virtual_actions::divECX},
	{"divEDX", virtual_actions::divEDX},

	{"divRAX", virtual_actions::divRAX},
	{"divRBX", virtual_actions::divRBX},
	{"divRCX", virtual_actions::divRCX},
	{"divRDX", virtual_actions::divRDX},

	{"addAX", virtual_actions::addAX},
	{"addBX", virtual_actions::addBX},
	{"addCX", virtual_actions::addCX},
	{"addDX", virtual_actions::addDX},

	{"addEAX", virtual_actions::addEAX},
	{"addEBX", virtual_actions::addEBX},
	{"addECX", virtual_actions::addECX},
	{"addEDX", virtual_actions::addEDX},

	{"addRAX", virtual_actions::addRAX},
	{"addRBX", virtual_actions::addRBX},
	{"addRCX", virtual_actions::addRCX},
	{"addRDX", virtual_actions::addRDX},

	{"subAX", virtual_actions::subAX},
	{"subBX", virtual_actions::subBX},
	{"subCX", virtual_actions::subCX},
	{"subDX", virtual_actions::subDX},

	{"subEAX", virtual_actions::subEAX},
	{"subEBX", virtual_actions::subEBX},
	{"subECX", virtual_actions::subECX},
	{"subEDX", virtual_actions::subEDX},

	{"subRAX", virtual_actions::subRAX},
	{"subRBX", virtual_actions::subRBX},
	{"subRCX", virtual_actions::subRCX},
	{"subRDX", virtual_actions::subRDX},

	{"toString", virtual_actions::toString},
	{"castreg", virtual_actions::castreg},

	{"push", virtual_actions::push},
	{"pop", virtual_actions::pop},

	{"pushSR", virtual_actions::pushSR},
	{"popSR", virtual_actions::popSR},

	{"pushCR", virtual_actions::pushCR},
	{"popCR", virtual_actions::popCR},

	{"pushDR", virtual_actions::pushDR},
	{"popDR", virtual_actions::popDR},

	{"nsms", virtual_actions::nsms},

	{"movsm", virtual_actions::movsm},
	{"movgm", virtual_actions::movgm},

	{"movsmSR", virtual_actions::movsmSR},
	{"movgmSR", virtual_actions::movgmSR},

	{"movsmCR", virtual_actions::movsmCR},
	{"movgmCR", virtual_actions::movgmCR},

	{"movsmDR", virtual_actions::movsmDR},
	{"movgmDR", virtual_actions::movgmDR},

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

	{"ijmp", virtual_actions::ijmp},
	{"jmp", virtual_actions::jmp},
	{"cmp", virtual_actions::cmp},
	{"je", virtual_actions::je},
	{"jne", virtual_actions::jne},
	{"jl", virtual_actions::jl},
	{"jg", virtual_actions::jg},
	{"jle", virtual_actions::jle},
	{"jge", virtual_actions::jge},
	{"cmpstr", virtual_actions::cmpstr},

	{"gca", virtual_actions::gca},
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
	{"nselep", virtual_actions::nselep}
#pragma endregion
};

std::unordered_set<std::string> included_headers;

std::unordered_set<std::string> global_toInclude;
std::vector<std::string> global_include_list;

std::string processCompiletimeArg(std::string argument, variables_decl* vars) {
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

			double value;
			std::stringstream ss(s_val);
			ss >> value;
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
			DTOA(value, &uc_d);

			vars->set(var_name, uc_d, sizeof(double));
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

			unsigned char* uc_s = new unsigned char[str_size];
#if defined(ISWIN)
			memcpy_s(uc_s, str_size, value.c_str(), str_size);
#else
			std::memcpy(uc_s, value.c_str(), str_size);
#endif
			vars->set(var_name, uc_s, str_size);
			vars->setVariablesTree(decl_form);
			vars->sys_vars_count += 1;

			delete[] uc_s;

			return var_name;
		}
		else {
			return "NULL";
		}
	}
	catch (const std::out_of_range&) {
		std::cout << "Invalid compiletime value declaration: " << content << std::endl;
		std::cout << "Replaced by NULL" << std::endl;
		return "NULL";
	}
}
std::vector<std::vector<std::string>> parseCodeLines(std::string filename, variables_decl* vars) {
	std::vector<std::string> toInclude;

	if (!included_headers.count(filename)) {
		included_headers.insert(filename);
	}
	
	std::ifstream file(filename);
	std::vector<std::vector<std::string>> parsed;
	std::string line;

	if (file) {
		while (!file.eof()) {
			std::getline(file, line);

			if (line.empty() || line[0] == ';') {
				continue;
			}

			if (!line.rfind("decltag ", 0)) {
				std::string tagname = line.substr(8);
				parsed.push_back(std::vector<std::string>({"decltag", tagname}));
				continue;
			}

			if (!line.rfind("[include] ", 0)) {
				std::string included = line.substr(10);
				std::string localpath = std::filesystem::relative(filename).replace_filename(included).string();

				if (!included_headers.count(localpath)) {
					toInclude.push_back(localpath);
					global_include_list.push_back(localpath);
				}
			}

			std::stringstream ss(line);
			std::string action, argument;
			ss >> action >> std::ws >> argument;
			argument =  line.substr(action.size() + 1);

			if (!argument.find("${{", 0) && argument.find("}}", argument.size() - 2) > 2
				&& argument.find("}}", argument.size() - 2 < argument.size())) {
				argument = processCompiletimeArg(argument, vars);
			}

			parsed.push_back(std::vector<std::string>({action, argument}));
		}

		for (size_t i = 0; i < toInclude.size(); i++) {
			if (!global_toInclude.count(toInclude[i])) {
				std::vector<std::vector<std::string>> loaded = parseCodeLines(toInclude[i], vars);

				for (size_t j = 0; j < loaded.size(); j++) {
					parsed.push_back(loaded[j]);
				}
				global_toInclude.insert(toInclude[i]);
			}
		}

		return parsed;
	}
	else {
		std::cout << "Unable to read secified code file" << std::endl;
		return parsed;
	}
}
std::vector<virtual_actions> convertSymbols(std::vector<std::vector<std::string>> parsed) {
	std::vector<virtual_actions> converted;

	for (size_t i = 0; i < parsed.size(); i++) {
		converted.push_back(symbols_converter[parsed[i][0]]);
	}

	return converted;
}
void purgeParsed(std::vector<virtual_actions> *converted, std::vector<std::vector<std::string>> *parsed) {
	for (size_t i = 0; i < converted->size(); i++) {
		if ((size_t)((*converted)[i]) == 0 || (*converted)[i] < virtual_actions::_int) {
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

			if (line.empty() || line[0] == ';') {
				continue;
			}

			if (!line.rfind("decltag ", 0)) {
				std::string tagname = line.substr(8);
				parsed.push_back(std::vector<std::string>({"decltag", tagname}));
				continue;
			}
			if (!line.rfind("decl ", 0)) {
				continue;
			}

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
		std::cout << "Unable to read secified code file" << std::endl;
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
		if ((size_t)(symbols_converter[cleaned_parsed[i][0]]) == 0 ||
				symbols_converter[cleaned_parsed[i][0]]  < virtual_actions::_int) {
			cleaned_parsed.erase(cleaned_parsed.begin() + i);
			i--;
		}
	}
}
std::vector<std::shared_ptr<void>> convertVariables(std::vector<std::vector<std::string>> cleaned_parsed, process_memory* p_mem) {
	std::vector<std::shared_ptr<void>> arguments;

	for (size_t i = 0; i < cleaned_parsed.size(); i++) {
		if (cleaned_parsed[i][1] == "0" || cleaned_parsed[i][1] == "NULL") {
			arguments.push_back(NULL);
		}
		else {
			arguments.push_back(p_mem->getVarPtr(cleaned_parsed[i][1]));
		}
	}

	return arguments;
}

void build_process(std::string filename, process* out_proc, engine* engine, process_memory* out_mem, memory*& mem) {
	variables_decl vars = build_variables_decl_tree(filename, mem);
	std::vector<std::vector<std::string>> parsed = parseCodeLines(filename, &vars);
	std::vector<virtual_actions> converted_actions = convertSymbols(parsed);

	purgeParsed(&converted_actions, &parsed);
	finalizeTags(makeCleanedParsed(filename), &vars);

	out_mem->set(&vars);
	out_mem->setRegisters(&vars);
	out_mem->setTags(&vars);

	std::vector<std::shared_ptr<void>> converted_arguments = convertVariables(parsed, out_mem);

	for (size_t i = 0; i < converted_actions.size(); i++) {
		out_proc->addAction(converted_actions[i], converted_arguments[i]);
	}
}
std::vector<action> build_actions_only(std::string filename, process_memory* out_mem, memory*& mem) {
	variables_decl vars = build_variables_decl_tree(filename, mem);
	std::vector<std::vector<std::string>> parsed = parseCodeLines(filename, &vars);
	std::vector<virtual_actions> converted_actions = convertSymbols(parsed);

	purgeParsed(&converted_actions, &parsed);
	finalizeTags(makeCleanedParsed(filename), &vars);

	out_mem->set(&vars);
	out_mem->setRegisters(&vars);
	out_mem->setTags(&vars);

	std::vector<std::shared_ptr<void>> converted_arguments = convertVariables(parsed, out_mem);
	std::vector<action> out_actions;

	for (size_t i = 0; i < converted_actions.size(); i++) {
		out_actions.push_back(action(converted_actions[i], converted_arguments[i]));
	}

	return out_actions;
}