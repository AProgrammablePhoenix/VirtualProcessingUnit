#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <tuple>

#include "../Memory/memory_decl.h"

// a user variable can't start with a variable name like that
#define RES_VAR_TAG "__sys_"
#define RES_USR_VAR_TAG "__usr_"
#define RES_TAG_VAR_TAG "__tag_"

struct code_file_decl_form {
	std::string decl_attr;
	std::string decl_type;
	std::string decl_name;
	std::string decl_value;
};
struct tag_decl_form {
	std::string tagname;
	size_t value = 0;
};

struct variables_decl {
public:
	variables_decl(memory* _mem);

	void set(std::string var_name, unsigned char* value, size_t length);;
	void get(std::string var_name, unsigned char** out);

	void setVariablesTree(code_file_decl_form branch);

	std::vector<code_file_decl_form> getVariablesTree();

	void setTag(std::string tagname, size_t value);
	std::map<std::string, size_t> getTagsMap();

	void setTagsBranch(tag_decl_form branch);
	std::vector<tag_decl_form> getTagsTree();

	std::tuple<size_t, size_t> getVarInfos(std::string var_name);

	size_t sys_vars_count = 0;
	size_t tag_vars_count = 0;
private:
	memory* mem;

	std::map<std::string, std::tuple<size_t, size_t>> vars;
	
	std::map<std::string, code_file_decl_form> variables_tree;
	std::vector<code_file_decl_form> variables_vector;

	std::map<std::string, size_t> tags;
	std::map<std::string, tag_decl_form> tags_tree;
	std::vector<tag_decl_form> tags_vector;
};

variables_decl build_variables_decl_tree(std::string filename, memory* mem);