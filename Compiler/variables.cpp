#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_set>
#include <tuple>

#if defined(__linux__)
#include <cstring>
#include <stdio.h>
#endif

#include "../utility.h"
#include "../Memory/memory_decl.h"
#include "variables.h"

std::unordered_set<std::string> regs_names = {
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
	"RSP"

	"SR",
	"CR",

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
	"RFPR3"
};

variables_decl::variables_decl(memory* _mem) {
	this->mem = _mem;
}

void variables_decl::set(std::string var_name, unsigned char* value, size_t length) {
	auto it = this->vars.find(var_name);
	if (it != this->vars.end())
		return;

	size_t addr = this->mem->_SDZTOP();
	this->mem->_SDZS(value, length);

	this->vars[var_name] = std::make_tuple(addr, length, nbyte(0));
}
void variables_decl::get(std::string var_name, unsigned char** out) {
	if (!this->vars.count(var_name)) {
		out = nullptr;
		return;
	}
	const auto [vaddr, vsize, vopt] = this->vars[var_name];

	*out = new unsigned char[vsize];
	this->mem->_MG(*out, vsize, vaddr);
}

void variables_decl::setVariablesTree(code_file_decl_form branch) {
	if (!this->variables_tree.count(branch.decl_name)) {
		this->variables_tree[branch.decl_name] = branch;
		this->variables_vector.push_back(branch);
	}
}
std::vector<code_file_decl_form> variables_decl::getVariablesTree() {
	return this->variables_vector;
}

void variables_decl::setTag(std::string tagname, size_t value) {
	this->tags[tagname] = value;
}
std::map<std::string, size_t> variables_decl::getTagsMap() {
	return this->tags;
}

void variables_decl::setTagsBranch(tag_decl_form branch) {
	if (!this->tags_tree.count(branch.tagname)) {
		this->tags_tree[branch.tagname] = branch;
		this->tags_vector.push_back(branch);
	}
}
std::vector<tag_decl_form> variables_decl::getTagsTree() {
	return this->tags_vector;
}

arg_tuple variables_decl::getVarInfos(std::string var_name) {
	return this->vars[var_name];
}

code_file_decl_form processDeclCodeLine(std::string line) {
	code_file_decl_form decl_data;

	if (line.substr(5, 6) == "string") {
		decl_data.decl_attr = "defined";
		decl_data.decl_type = "string";
		
		std::stringstream ss(line.substr(12));
		
		if (std::getline(ss, decl_data.decl_name, ' ')) {
			if (!std::getline(ss, decl_data.decl_value)) {
				decl_data.decl_value = "<parsing-error>";
			}
		}
		else {
			decl_data.decl_name = "<parsing-error>";
		}

		if (!decl_data.decl_name.rfind(RES_VAR_TAG, 0) || !decl_data.decl_name.rfind(RES_USR_VAR_TAG, 0) ||
				!decl_data.decl_name.rfind(RES_TAG_VAR_TAG, 0) || regs_names.find(decl_data.decl_name) != regs_names.end()) {
			std::cout << "Denied variable name (starts with restricted name): " << decl_data.decl_name << std::endl;
			std::cout << "Variable not compiled" << std::endl;
			decl_data.decl_name = "<parsing-error>";
		}

		return decl_data;
	}
	else if (line.substr(5, 4) == "char") {
		decl_data.decl_attr = "defined";
		decl_data.decl_type = "char";

		std::stringstream ss(line.substr(10));

		if (std::getline(ss, decl_data.decl_name, ' ')) {
			if (!std::getline(ss, decl_data.decl_value)) {
				decl_data.decl_value = "<parsing-error>";
			}
		}
		else {
			decl_data.decl_name = "<parsing-error>";
		}

		if (!decl_data.decl_name.rfind(RES_VAR_TAG, 0) || !decl_data.decl_name.rfind(RES_USR_VAR_TAG, 0) ||
				!decl_data.decl_name.rfind(RES_TAG_VAR_TAG, 0) || regs_names.find(decl_data.decl_name) != regs_names.end()) {
			std::cout << "Denied variable name (starts with restricted name): " << decl_data.decl_name << std::endl;
			std::cout << "Variable not compiled" << std::endl;
			decl_data.decl_name = "<parsing-error>";
		}

		return decl_data;
	}
	else if (line.substr(5, 15) == "unsigned_number") {
		decl_data.decl_attr = "defined";
		decl_data.decl_type = "unsigned number";

		std::stringstream ss(line.substr(21));

		ss >> decl_data.decl_name >> std::ws >> decl_data.decl_value;

		if (!decl_data.decl_name.rfind(RES_VAR_TAG, 0) || !decl_data.decl_name.rfind(RES_USR_VAR_TAG, 0) ||
				!decl_data.decl_name.rfind(RES_TAG_VAR_TAG, 0) || regs_names.find(decl_data.decl_name) != regs_names.end()) {
			std::cout << "Denied variable name (starts with restricted name): " << decl_data.decl_name << std::endl;
			std::cout << "Variable not compiled" << std::endl;
			decl_data.decl_name = "<parsing-error>";
		}
	}
	else if (line.substr(5, 13) == "signed_number") {
		decl_data.decl_attr = "defined";
		decl_data.decl_type = "signed number";

		std::stringstream ss(line.substr(19));

		ss >> decl_data.decl_name >> std::ws >> decl_data.decl_value;

		if (!decl_data.decl_name.rfind(RES_VAR_TAG, 0) || !decl_data.decl_name.rfind(RES_USR_VAR_TAG, 0) ||
				!decl_data.decl_name.rfind(RES_TAG_VAR_TAG, 0) || regs_names.find(decl_data.decl_name) != regs_names.end()) {
			std::cout << "Denied variable name (starts with restricted name): " << decl_data.decl_name << std::endl;
			std::cout << "Variable not compiled" << std::endl;
			decl_data.decl_name = "<parsing-error>";
		}
	}
	else if(line.substr(5, 6) == "double") {
		decl_data.decl_attr = "defined";
		decl_data.decl_type = "double";

		std::stringstream ss(line.substr(12));

		ss >> decl_data.decl_name >> std::ws >> decl_data.decl_value;

		if (!decl_data.decl_name.rfind(RES_VAR_TAG, 0) || !decl_data.decl_name.rfind(RES_USR_VAR_TAG, 0) ||
				!decl_data.decl_name.rfind(RES_TAG_VAR_TAG, 0) || regs_names.find(decl_data.decl_name) != regs_names.end()) {
			std::cout << "Denied variable name (starts with restricted name): " << decl_data.decl_name << std::endl;
			std::cout << "Variable not compiled" << std::endl;
			decl_data.decl_name = "<parsing-error>";
		}
	}
	else {
		decl_data.decl_attr = "undefined";
		decl_data.decl_type = "undefined";
		decl_data.decl_name = "<parsing-error>";
		decl_data.decl_value = "<parsing-error>";
	}
	return decl_data;
}

std::vector<code_file_decl_form> parse_code_file(std::string filename, std::vector<tag_decl_form>* tagsvec) {
	std::ifstream file(filename);
	std::vector<code_file_decl_form> decl_lines;
	std::string line = "";

	if (file) {
		while (!file.eof()) {
			std::getline(file, line);

			if (line.empty() || line[0] == ';') {
				continue;
			}
			else if (!line.rfind("decltag ", 0)) {
				tag_decl_form tag;
				tag.tagname = line.substr(8);
				tag.value = 0;

				if (regs_names.find(tag.tagname) != regs_names.end()) {
					std::cout << "Denied tag name (starts with restricted name): " << tag.tagname << std::endl;
					std::cout << "tag not compiled" << std::endl;
				}
				else {
					tagsvec->push_back(tag);
				}
			}
			else if (!line.rfind("decl ", 0)) {
				decl_lines.push_back(processDeclCodeLine(line));
			}
			continue;
		}
	}
	else {
		std::cout << "Unable to read secified code file" << std::endl;
		return decl_lines;
	}

	return decl_lines;
}

variables_decl build_variables_decl_tree(std::string filename, memory* mem) {
	variables_decl storage = variables_decl(mem);
	std::vector<tag_decl_form> tagsvec;
	std::vector<code_file_decl_form> parsed = parse_code_file(filename, &tagsvec);

	for (size_t i = 0; i < parsed.size(); i++) {
		if (parsed[i].decl_attr == "defined") {
			if (parsed[i].decl_type == "undefined" || parsed[i].decl_name == "<parsing-error>" ||
				parsed[i].decl_value == "<parsing-error>") {
				continue;
			}
			else {
				if (parsed[i].decl_type == "string") {
					unsigned char* uc_s = new unsigned char[parsed[i].decl_value.size() + 1];

#if defined(ISWIN)
					memcpy_s(uc_s, parsed[i].decl_value.size() + 1, parsed[i].decl_value.c_str(), parsed[i].decl_value.size() + 1);
#else
					std::memcpy(uc_s, parsed[i].decl_value.c_str(), parsed[i].decl_value.size() + 1);
#endif
					storage.set(parsed[i].decl_name, uc_s, parsed[i].decl_value.size() + 1);
					storage.setVariablesTree(parsed[i]);

					delete[] uc_s;
				}
				else if (parsed[i].decl_type == "char") {
					if (parsed[i].decl_value.size() < 1) {
						std::cout << "Error at declaration of variable: '" << parsed[i].decl_name 
							<< "' (type: char) :: invalid value length" << std::endl;
						continue;
					}

					char c = parsed[i].decl_value[0];

					unsigned char* uc_c = new unsigned char[1];
					uc_c[0] = c;
					
					storage.set(parsed[i].decl_name, uc_c, 1);
					storage.setVariablesTree(parsed[i]);

					delete[] uc_c;
				}
				else if (parsed[i].decl_type == "unsigned number") {
					size_t n_value = std::stoull(parsed[i].decl_value);
					
					unsigned char* uc_n = nullptr;
					ULLTOA(n_value, &uc_n);

					storage.set(parsed[i].decl_name, uc_n, sizeof(size_t));
					storage.setVariablesTree(parsed[i]);

					delete[] uc_n;
				}
				else if (parsed[i].decl_type == "signed number") {
					long long n_value = std::stoll(parsed[i].decl_value);

					unsigned char* uc_n = nullptr;
					ULLTOA((size_t)n_value, &uc_n);

					storage.set(parsed[i].decl_name, uc_n, sizeof(size_t));
					storage.setVariablesTree(parsed[i]);

					delete[] uc_n;
				}
				else if (parsed[i].decl_type == "double") {
					std::stringstream ss(parsed[i].decl_value);
					long double n_value;
					ss >> std::setprecision(std::numeric_limits<long double>::digits10) >> n_value;

					unsigned char* uc_d = nullptr;
					
					LDTOA(n_value, &uc_d);

					storage.set(parsed[i].decl_name, uc_d, sizeof(long double));
					storage.setVariablesTree(parsed[i]);

					delete[] uc_d;
				}
			}
		}
		else {
			continue;
		}
	}
	for (size_t i = 0; i < tagsvec.size(); i++) {
		if (tagsvec[i].tagname == "undefined") {
			continue;
		}
		storage.setTag(tagsvec[i].tagname, tagsvec[i].value);
	}

	return storage;
}