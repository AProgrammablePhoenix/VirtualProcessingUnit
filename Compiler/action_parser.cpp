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

	SETREG(registries_def::RBP, "RBP", uc_r, vars, this->data_ptrs);
	SETREG(registries_def::RSP, "RSP", uc_r, vars, this->data_ptrs);

	SETXREG(extra_registries::SR, "SR", uc_r, vars, this->data_ptrs);
	SETXREG(extra_registries::CR, "CR", uc_r, vars, this->data_ptrs);

	SETXREG(extra_registries::FPR0, "FPR0", uc_r, vars, this->data_ptrs);
	SETXREG(extra_registries::FPR1, "FPR1", uc_r, vars, this->data_ptrs);
	SETXREG(extra_registries::FPR2, "FPR2", uc_r, vars, this->data_ptrs);
	SETXREG(extra_registries::FPR3, "FPR3", uc_r, vars, this->data_ptrs);

	SETXREG(extra_registries::EFPR0, "EFPR0", uc_r, vars, this->data_ptrs);
	SETXREG(extra_registries::EFPR1, "EFPR1", uc_r, vars, this->data_ptrs);
	SETXREG(extra_registries::EFPR2, "EFPR2", uc_r, vars, this->data_ptrs);
	SETXREG(extra_registries::EFPR3, "EFPR3", uc_r, vars, this->data_ptrs);

	SETXREG(extra_registries::RFPR0, "RFPR0", uc_r, vars, this->data_ptrs);
	SETXREG(extra_registries::RFPR1, "RFPR1", uc_r, vars, this->data_ptrs);
	SETXREG(extra_registries::RFPR2, "RFPR2", uc_r, vars, this->data_ptrs);
	SETXREG(extra_registries::RFPR3, "RFPR3", uc_r, vars, this->data_ptrs);
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

	{"setFPR0", virtual_actions::setFPR0},
	{"setFPR1", virtual_actions::setFPR1},
	{"setFPR2", virtual_actions::setFPR2},
	{"setFPR3", virtual_actions::setFPR3},

	{"setEFPR0", virtual_actions::setEFPR0},
	{"setEFPR1", virtual_actions::setEFPR1},
	{"setEFPR2", virtual_actions::setEFPR2},
	{"setEFPR3", virtual_actions::setEFPR3},

	{"setRFPR0", virtual_actions::setRFPR0},
	{"setRFPR1", virtual_actions::setRFPR1},
	{"setRFPR2", virtual_actions::setRFPR2},
	{"setRFPR3", virtual_actions::setRFPR3},

	{"setSR", virtual_actions::setSR},
	{"getSR", virtual_actions::getSR},

	{"setCR", virtual_actions::setCR},
	{"getCR", virtual_actions::getCR},

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
	{"movRBP", virtual_actions::movRBP},
	{"movRSP", virtual_actions::movRSP},

	{"movFPR0", virtual_actions::movFPR0},
	{"movFPR1", virtual_actions::movFPR1},
	{"movFPR2", virtual_actions::movFPR2},
	{"movFPR3", virtual_actions::movFPR3},

	{"movEFPR0", virtual_actions::movEFPR0},
	{"movEFPR1", virtual_actions::movEFPR1},
	{"movEFPR2", virtual_actions::movEFPR2},
	{"movEFPR3", virtual_actions::movEFPR3},

	{"movRFPR0", virtual_actions::movRFPR0},
	{"movRFPR1", virtual_actions::movRFPR1},
	{"movRFPR2", virtual_actions::movRFPR2},
	{"movRFPR3", virtual_actions::movRFPR3},

	{"inc", virtual_actions::inc},
	{"dec", virtual_actions::dec},
	{"incFP", virtual_actions::incFP},
	{"decFP", virtual_actions::decFP},

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

	{"mulFPR0", virtual_actions::mulFPR0},
	{"mulFPR1", virtual_actions::mulFPR1},
	{"mulFPR2", virtual_actions::mulFPR2},
	{"mulFPR3", virtual_actions::mulFPR3},

	{"mulEFPR0", virtual_actions::mulEFPR0},
	{"mulEFPR1", virtual_actions::mulEFPR1},
	{"mulEFPR2", virtual_actions::mulEFPR2},
	{"mulEFPR3", virtual_actions::mulEFPR3},

	{"mulRFPR0", virtual_actions::mulRFPR0},
	{"mulRFPR1", virtual_actions::mulRFPR1},
	{"mulRFPR2", virtual_actions::mulRFPR2},
	{"mulRFPR3", virtual_actions::mulRFPR3},

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

	{"divFPR0", virtual_actions::divFPR0},
	{"divFPR1", virtual_actions::divFPR1},
	{"divFPR2", virtual_actions::divFPR2},
	{"divFPR3", virtual_actions::divFPR3},

	{"divEFPR0", virtual_actions::divEFPR0},
	{"divEFPR1", virtual_actions::divEFPR1},
	{"divEFPR2", virtual_actions::divEFPR2},
	{"divEFPR3", virtual_actions::divEFPR3},

	{"divRFPR0", virtual_actions::divRFPR0},
	{"divRFPR1", virtual_actions::divRFPR1},
	{"divRFPR2", virtual_actions::divRFPR2},
	{"divRFPR3", virtual_actions::divRFPR3},

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
	{"addRBP", virtual_actions::addRBP},
	{"addRSP", virtual_actions::addRSP},

	{"addFPR0", virtual_actions::addFPR0},
	{"addFPR1", virtual_actions::addFPR1},
	{"addFPR2", virtual_actions::addFPR2},
	{"addFPR3", virtual_actions::addFPR3},

	{"addEFPR0", virtual_actions::addEFPR0},
	{"addEFPR1", virtual_actions::addEFPR1},
	{"addEFPR2", virtual_actions::addEFPR2},
	{"addEFPR3", virtual_actions::addEFPR3},

	{"addRFPR0", virtual_actions::addRFPR0},
	{"addRFPR1", virtual_actions::addRFPR1},
	{"addRFPR2", virtual_actions::addRFPR2},
	{"addRFPR3", virtual_actions::addRFPR3},

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
	{"subRBP", virtual_actions::subRBP},
	{"subRSP", virtual_actions::subRSP},

	{"subFPR0", virtual_actions::subFPR0},
	{"subFPR1", virtual_actions::subFPR1},
	{"subFPR2", virtual_actions::subFPR2},
	{"subFPR3", virtual_actions::subFPR3},

	{"subEFPR0", virtual_actions::subEFPR0},
	{"subEFPR1", virtual_actions::subEFPR1},
	{"subEFPR2", virtual_actions::subEFPR2},
	{"subEFPR3", virtual_actions::subEFPR3},

	{"subRFPR0", virtual_actions::subRFPR0},
	{"subRFPR1", virtual_actions::subRFPR1},
	{"subRFPR2", virtual_actions::subRFPR2},
	{"subRFPR3", virtual_actions::subRFPR3},

	{"toString", virtual_actions::toString},
	{"castreg", virtual_actions::castreg},

	{"sdzs", virtual_actions::sdzs},

	{"push", virtual_actions::push},
	{"pop", virtual_actions::pop},

	{"pushSR", virtual_actions::pushSR},
	{"popSR", virtual_actions::popSR},

	{"pushCR", virtual_actions::pushCR},
	{"popCR", virtual_actions::popCR},

	{"pushFP", virtual_actions::pushFP},
	{"popFP", virtual_actions::popFP},

	{"movsm", virtual_actions::movsm},
	{"movgm", virtual_actions::movgm},

	{"movsmSR", virtual_actions::movsmSR},
	{"movgmSR", virtual_actions::movgmSR},

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
	{"cmpstr", virtual_actions::cmpstr},
	{"cmpdbl", virtual_actions::cmpdbl},

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
	{"nselep", virtual_actions::nselep},

	{"crtthread", virtual_actions::pcrtthread},
	{"rstthread", virtual_actions::prstthread},
	{"endthread", virtual_actions::pendthread}
#pragma endregion
};

static std::unordered_set<std::string> included_headers;

static std::unordered_set<std::string> global_toInclude;
static std::vector<std::string> global_include_list;

static bool mem_resizing_req = false;
static bool is_mem_resized = false;

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
std::vector<std::vector<std::string>> parseCodeLines(std::string filename, variables_decl* vars,
		std::vector<std::tuple<std::string, size_t>>& threadsToBuild, memory* const& memptr) {
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
			else if (!line.rfind("[include] ", 0)) {
				std::string included = line.substr(10);
				std::string localpath = std::filesystem::relative(filename).replace_filename(included).string();
				
				if (!included_headers.count(localpath)) {
					toInclude.push_back(localpath);
					global_include_list.push_back(localpath);
				}
				continue;
			}
			else if (!line.rfind("[ldthread] ", 0)) {
				// Has form [ldthread] <thread_file>,<thread_id>
				std::string loadedThread = line.substr(11);
				
				std::istringstream split(loadedThread);
				std::vector<std::string> tokens;
				for (std::string token; std::getline(split, token, ','); tokens.push_back(token));
				if (tokens.size() < 2) {
					std::cout
						<< "Error on thread declaration, missing thread id (should be: [ldthread] <thread_file>,<thread_id>)"
						<< std::endl;
					continue;

				}
				loadedThread = tokens[0];
				size_t threadId = std::stoull(tokens[1]);

				std::string localpath = std::filesystem::relative(filename).replace_filename(loadedThread).string();
				threadsToBuild.push_back(std::make_tuple<std::string&, size_t&>(localpath, threadId));
				continue;
			}
			else if (!line.rfind("[MRES] ", 0) && !is_mem_resized) {
				// Has form of [MRES] <new memory size>, with new size an unsigned number
				std::string s_new_size = line.substr(7);
				size_t new_size = std::stoull(s_new_size);
				memptr->_MRSZ(new_size);
				is_mem_resized = true;
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
				std::vector<std::vector<std::string>> loaded = parseCodeLines(toInclude[i], vars, threadsToBuild, memptr);

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
	std::vector<std::tuple<std::string, size_t>> threadsToBuild;

	variables_decl vars = build_variables_decl_tree(filename, mem);
	std::vector<std::vector<std::string>> parsed = parseCodeLines(filename, &vars, threadsToBuild, mem);
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

	std::vector<std::tuple<std::vector<action>, size_t>> sub_threads;
	for (size_t i = 0; i < threadsToBuild.size(); i++) {
		std::vector<action> thread = build_actions_only(std::get<0>(threadsToBuild[i]), out_mem, mem, sub_threads);
		out_proc->addThread(thread, std::get<1>(threadsToBuild[i]));
	}
	for (size_t i = 0; i < sub_threads.size(); i++) {
		std::vector<action> sthread = std::get<0>(sub_threads[i]);
		out_proc->addThread(sthread, std::get<1>(sub_threads[i]));
	}
	
	if (mem_resizing_req && !is_mem_resized) {
		out_proc->updateStackRegs();
		is_mem_resized = true;
	}
}
std::vector<action> build_actions_only(std::string filename, process_memory* out_mem, memory*& mem,
		std::vector<std::tuple<std::vector<action>, size_t>>& out_threads) {
	std::vector<std::tuple<std::string, size_t>> threadsToBuild;

	variables_decl vars = build_variables_decl_tree(filename, mem);
	std::vector<std::vector<std::string>> parsed = parseCodeLines(filename, &vars, threadsToBuild, mem);
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

	std::vector<std::tuple<std::vector<action>, size_t>> sub_threads;
	for (size_t i = 0; i < threadsToBuild.size(); i++) {
		std::vector<action> thread = build_actions_only(std::get<0>(threadsToBuild[i]), out_mem, mem, sub_threads);
		out_threads.push_back(std::make_tuple<std::vector<action>&, size_t&>(thread, std::get<1>(threadsToBuild[i])));
	}
	for (size_t i = 0; i < sub_threads.size(); i++) {
		out_threads.push_back(std::make_tuple<std::vector<action>&, size_t&>(std::get<0>(sub_threads[i]), std::get<1>(sub_threads[i])));
	}
		 
	return out_actions;
}