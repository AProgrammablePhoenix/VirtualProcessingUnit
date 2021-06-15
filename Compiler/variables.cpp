#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <tuple>

#if defined(__linux__)
#include <cstring>
#include <stdio.h>
#endif

#include "../utility.h"
#include "../Memory/memory_decl.h"
#include "variables.h"

variables_decl::variables_decl(memory* _mem) {
	this->mem = _mem;
	this->vars = std::map<std::string, m_container>();
	this->newvars = std::map<std::string, std::tuple<size_t, size_t>>();
}

void variables_decl::set(std::string var_name, unsigned char value[]) {
	std::map<std::string, m_container>::iterator it;
	it = this->vars.find(var_name);
	if (it != this->vars.end()) {
		return;
	}

	m_container var = m_container();
	var.set(value);
	this->vars[var_name] = var;
}
void variables_decl::set(std::string var_name, unsigned char value[], size_t length) {
	std::map<std::string, m_container>::iterator it;
	it = this->vars.find(var_name);
	if (it != this->vars.end()) {
		return;
	}

	m_container var = m_container();
	var.set(value, length);
	this->vars[var_name] = var;
}

void variables_decl::newset(std::string var_name, unsigned char* value, size_t length) {
	std::map<std::string, std::tuple<size_t, size_t>>::iterator it;
	it = this->newvars.find(var_name);
	if (it != this->newvars.end()) {
		return;
	}

	size_t addr = this->mem->_ROZGST();
	this->mem->_ROZVS(value, length);
	this->newvars[var_name] = std::make_tuple<size_t&, size_t&>(addr, length);
}

unsigned char* variables_decl::get(std::string var_name) {
	if (!this->vars.count(var_name)) {
		return nullptr;
	}
	return this->vars[var_name].get();
}
void variables_decl::newget(std::string var_name, unsigned char** out) {
	if (!this->newvars.count(var_name)) {
		out = nullptr;
		return;
	}
	size_t addr = std::get<0>(this->newvars[var_name]);
	size_t length = std::get<1>(this->newvars[var_name]);

	*out = new unsigned char[length];
	this->mem->_ROZVG(*out, length, addr);
}

void variables_decl::deleteVar(std::string var_name) {
	std::map<std::string, m_container>::iterator it;
	it = this->vars.find(var_name);
	if (it != this->vars.end()) {
		this->vars.erase(it);
	}
}

void variables_decl::setVariablesTree(code_file_decl_form branch) {
	if (!this->variables_tree.count(branch.decl_name)) {
		this->variables_tree[branch.decl_name] = branch;
		this->variables_vector.push_back(branch);
	}
}
code_file_decl_form variables_decl::getVariableBranch(std::string var_name) {
	if (this->variables_tree.count(var_name)) {
		return this->variables_tree[var_name];
	}
	else {
		code_file_decl_form err;
		err.decl_attr = "undefined";
		err.decl_type = "undefined";
		err.decl_name = "<parsing-error>";
		err.decl_value = "<parsing-error>";
		return err;
	}
}
std::vector<code_file_decl_form> variables_decl::getVariablesTree() {
	return this->variables_vector;
}

void variables_decl::setTag(std::string tagname, size_t value) {
	this->tags[tagname] = value;
}
size_t variables_decl::getTag(std::string tagname) {
	if (this->tags.count(tagname)) {
		return this->tags[tagname];
	}
	return 0;
}
void variables_decl::setTagsMap(std::map<std::string, size_t> map) {
	this->tags = map;
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
tag_decl_form variables_decl::getTagBranch(std::string tagname) {
	if (this->tags_tree.count(tagname)) {
		return this->tags_tree[tagname];
	}
	else {
		tag_decl_form err;
		err.tagname = "undefined";
		err.value = 0;
		return err;
	}
}
std::vector<tag_decl_form> variables_decl::getTagsTree() {
	return this->tags_vector;
}

std::tuple<size_t, size_t> variables_decl::newgetVarInfos(std::string var_name) {
	return this->newvars[var_name];
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
				!decl_data.decl_name.rfind(RES_TAG_VAR_TAG, 0)) {
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
				!decl_data.decl_name.rfind(RES_TAG_VAR_TAG, 0)) {
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
				!decl_data.decl_name.rfind(RES_TAG_VAR_TAG, 0)) {
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
				!decl_data.decl_name.rfind(RES_TAG_VAR_TAG, 0)) {
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
				!decl_data.decl_name.rfind(RES_TAG_VAR_TAG, 0)) {
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
				tagsvec->push_back(tag);
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
					storage.newset(parsed[i].decl_name, uc_s, parsed[i].decl_value.size() + 1);
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
					
					storage.newset(parsed[i].decl_name, uc_c, 1);
					storage.setVariablesTree(parsed[i]);

					delete[] uc_c;
				}
				else if (parsed[i].decl_type == "unsigned number") {
					size_t n_value = std::stoull(parsed[i].decl_value);
					
					unsigned char* uc_n = nullptr;
					ULLTOA(n_value, &uc_n);

					storage.newset(parsed[i].decl_name, uc_n, sizeof(size_t));
					storage.setVariablesTree(parsed[i]);

					delete[] uc_n;
				}
				else if (parsed[i].decl_type == "signed number") {
					long long n_value = std::stoll(parsed[i].decl_value);

					unsigned char* uc_n = nullptr;
					ULLTOA((size_t)n_value, &uc_n);

					storage.newset(parsed[i].decl_name, uc_n, sizeof(size_t));
					storage.setVariablesTree(parsed[i]);

					delete[] uc_n;
				}
				else if (parsed[i].decl_type == "double") {
					std::stringstream ss(parsed[i].decl_value);
					double n_value;
					ss >> n_value;

					unsigned char* uc_d = nullptr;
					
					DTOA(n_value, &uc_d);

					storage.newset(parsed[i].decl_name, uc_d, sizeof(double));
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