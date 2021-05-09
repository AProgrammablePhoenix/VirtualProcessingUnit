#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_set>

#if defined(__linux__)
#include <cstring>
#include <stdio.h>
#endif

#include "../Actions/threading.h"
#include "../Actions/v_engine.h"
#include "../Memory/memory_decl.h"
#include "action_parser.h"
#include "variables.h"

void process_memory::set(variables_decl* var) {
	std::vector<code_file_decl_form> headers = var->getVariablesTree();

	for (unsigned long long i = 0; i < headers.size(); i++) {
		if (headers[i].decl_type == "string") {
			this->stored_strings[headers[i].decl_name] = headers[i].decl_value;
			this->data_ptrs[headers[i].decl_name] = std::make_shared<std::string>(this->stored_strings[headers[i].decl_name]);
		}
		else if (headers[i].decl_type == "unsigned number") {
			unsigned long long value;
			std::stringstream ss(headers[i].decl_value);
			ss >> value;
			this->unsigned_numbers[headers[i].decl_name] = value;
			this->data_ptrs[headers[i].decl_name] = std::make_shared<unsigned long long>(this->unsigned_numbers[headers[i].decl_name]);
		}
		else if (headers[i].decl_type == "signed number") {
			long long value;
			std::stringstream ss(headers[i].decl_value);
			ss >> value;
			this->signed_numbers[headers[i].decl_name] = value;
			this->data_ptrs[headers[i].decl_name] = std::make_shared<long long>(this->signed_numbers[headers[i].decl_name]);
		}
	}
}
void process_memory::setTags(variables_decl* vars) {
	std::vector<tag_decl_form> tags_headers = vars->getTagsTree();

	for (unsigned long long i = 0; i < tags_headers.size(); i++) {
		this->stored_tags[tags_headers[i].tagname] = tags_headers[i].value;
		this->data_ptrs[tags_headers[i].tagname] = std::make_shared<unsigned long long>(this->stored_tags[tags_headers[i].tagname]);
	}
}
void process_memory::setTagValue(std::string tagname, unsigned long long value) {
	this->stored_tags[tagname] = value;
}
void process_memory::set(std::string var_name, std::shared_ptr<void> data_ptr) {
	if (!this->data_ptrs.count(var_name)) {
		this->data_ptrs[var_name] = data_ptr;
	}
}
void process_memory::setRegisters() {
	this->data_ptrs["AX"] = std::make_shared<registries_def>(registries_def::AX);
	this->data_ptrs["BX"] = std::make_shared<registries_def>(registries_def::BX);
	this->data_ptrs["CX"] = std::make_shared<registries_def>(registries_def::CX);
	this->data_ptrs["DX"] = std::make_shared<registries_def>(registries_def::DX);

	this->data_ptrs["EAX"] = std::make_shared<registries_def>(registries_def::EAX);
	this->data_ptrs["EBX"] = std::make_shared<registries_def>(registries_def::EBX);
	this->data_ptrs["ECX"] = std::make_shared<registries_def>(registries_def::ECX);
	this->data_ptrs["EDX"] = std::make_shared<registries_def>(registries_def::EDX);

	this->data_ptrs["RAX"] = std::make_shared<registries_def>(registries_def::RAX);
	this->data_ptrs["RBX"] = std::make_shared<registries_def>(registries_def::RBX);
	this->data_ptrs["RCX"] = std::make_shared<registries_def>(registries_def::RCX);
	this->data_ptrs["RDX"] = std::make_shared<registries_def>(registries_def::RDX);

	this->data_ptrs["SR"] = std::make_shared<extra_registries>(extra_registries::SR);
}
bool process_memory::isTag(std::string tagname) {
	if (this->stored_tags.count(tagname)) {
		return true;
	}
	else {
		return false;
	}
}

std::shared_ptr<void> process_memory::getVarPtr(std::string var_name) {
	if (this->data_ptrs.count(var_name)) {
		return this->data_ptrs[var_name];
	}
	else {
		std::cout << "Warning: Symbol '" << var_name << "' unrecognized, replaced by NULL statement" << std::endl;
		return std::make_shared<unsigned int>(0);
	}
}
std::string process_memory::getVarType(std::string var_name) {
	if (this->unsigned_numbers.count(var_name)) {
		return "UNUM";
	}
	else if (this->signed_numbers.count(var_name)) {
		return "SNUM";
	}
	else if (this->stored_strings.count(var_name)) {
		return "STR";
	}
	else if (this->stored_tags.count(var_name)) {
		return "TAG";
	}
	else {
		return "UNDEFINED";
	}
}

std::map<std::string, virtual_actions> symbols_converter =
{
#pragma region map_decl
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
	{"mergeString", virtual_actions::mergeString},
	{"substring", virtual_actions::substring},
	{"strlen", virtual_actions::_strlen},
	{"rdstdin", virtual_actions::rdstdin},
	{"print", virtual_actions::print},
	{"println", virtual_actions::println},
	{"printEOL", virtual_actions::printEOL},

	{"castreg", virtual_actions::castreg},
	{"recast", virtual_actions::recast},
	{"fromString", virtual_actions::fromString},

	{"push", virtual_actions::push},
	{"pop", virtual_actions::pop},

	{"pushSR", virtual_actions::pushSR},
	{"popSR", virtual_actions::popSR},

	{"declArray", virtual_actions::declArray},
	{"setAt", virtual_actions::setAt},
	{"getAt", virtual_actions::getAt},
	{"getDynSize", virtual_actions::getDynSize},

	{"dyndecl", virtual_actions::dyndecl},
	{"dynset", virtual_actions::dynset},
	{"dynget", virtual_actions::dynget},

	{"struct",virtual_actions::_struct},
	{"struct.declprop", virtual_actions::_struct_declprop},
	{"struct.select", virtual_actions::_struct_select},
	{"struct.get", virtual_actions::_struct_get},
	{"struct.set", virtual_actions::_struct_set},

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
	{"rscall", virtual_actions::rscall}
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

			unsigned long long value;
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

			vars->set(var_name, (unsigned char*)value);
			vars->setVariablesTree(decl_form);
			vars->sys_vars_count += 1;
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

			vars->set(var_name, (unsigned char*)value);
			vars->setVariablesTree(decl_form);
			vars->sys_vars_count += 1;
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

			unsigned char* uc_s = new unsigned char[value.size() + 1];
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
			memcpy_s(uc_s, value.size() + 1, value.c_str(), value.size() + 1);
#else
			std::memcpy(uc_s, value.c_str(), value.size() + 1);
#endif
			vars->set(var_name, uc_s, value.size() + 1);
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

		for (unsigned long long i = 0; i < toInclude.size(); i++) {
			if (!global_toInclude.count(toInclude[i])) {
				std::vector<std::vector<std::string>> loaded = parseCodeLines(toInclude[i], vars);

				for (unsigned long long j = 0; j < loaded.size(); j++) {
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

	for (unsigned long long i = 0; i < parsed.size(); i++) {
		converted.push_back(symbols_converter[parsed[i][0]]);
	}

	return converted;
}
void purgeParsed(std::vector<virtual_actions> *converted, std::vector<std::vector<std::string>> *parsed) {
	for (unsigned long long i = 0; i < converted->size(); i++) {
		if ((unsigned long long)((*converted)[i]) == 0 || (*converted)[i] < virtual_actions::setAX) {
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
			for (unsigned long long i = 0; i < global_include_list.size(); i++) {
				std::vector<std::vector<std::string>> loaded = makeCleanedParsed(global_include_list[i], false);

				for (unsigned long long j = 0; j < loaded.size(); j++) {
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
	for (unsigned long long i = 0; i < cleaned_parsed.size(); i++) {
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
		if ((unsigned long long)(symbols_converter[cleaned_parsed[i][0]]) == 0 ||
				symbols_converter[cleaned_parsed[i][0]]  < virtual_actions::setAX) {
			cleaned_parsed.erase(cleaned_parsed.begin() + i);
			i--;
		}
	}
}
std::vector<std::shared_ptr<void>> convertVariables(std::vector<std::vector<std::string>> cleaned_parsed, process_memory* p_mem) {
	std::vector<std::shared_ptr<void>> arguments;

	for (unsigned long long i = 0; i < cleaned_parsed.size(); i++) {
		if (cleaned_parsed[i][1] == "0" || cleaned_parsed[i][1] == "NULL") {
			arguments.push_back(NULL);
		}
		else {
			arguments.push_back(p_mem->getVarPtr(cleaned_parsed[i][1]));
		}
	}

	return arguments;
}

void build_process(std::string filename, process* out_proc, engine* engine, process_memory* out_mem) {
	variables_decl vars = build_variables_decl_tree(filename);
	std::vector<std::vector<std::string>> parsed = parseCodeLines(filename, &vars);
	std::vector<virtual_actions> converted_actions = convertSymbols(parsed);

	purgeParsed(&converted_actions, &parsed);
	finalizeTags(makeCleanedParsed(filename), &vars);

	out_mem->set(&vars);
	out_mem->setRegisters();
	out_mem->setTags(&vars);

	std::vector<std::shared_ptr<void>> converted_arguments = convertVariables(parsed, out_mem);

	for (unsigned long long i = 0; i < converted_actions.size(); i++) {
		out_proc->addAction(converted_actions[i], converted_arguments[i]);
	}
}
std::vector<action> build_actions_only(std::string filename, process_memory* out_mem) {
	variables_decl vars = build_variables_decl_tree(filename);
	std::vector<std::vector<std::string>> parsed = parseCodeLines(filename, &vars);
	std::vector<virtual_actions> converted_actions = convertSymbols(parsed);

	purgeParsed(&converted_actions, &parsed);
	finalizeTags(makeCleanedParsed(filename), &vars);

	out_mem->set(&vars);
	out_mem->setRegisters();
	out_mem->setTags(&vars);

	std::vector<std::shared_ptr<void>> converted_arguments = convertVariables(parsed, out_mem);
	std::vector<action> out_actions;

	for (unsigned long long i = 0; i < converted_actions.size(); i++) {
		out_actions.push_back(action(converted_actions[i], converted_arguments[i]));
	}

	return out_actions;
}