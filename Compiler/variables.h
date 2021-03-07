#pragma once

#include <iostream>
#include <string>
#include <map>

#include "../Memory/memory_decl.h"

struct code_file_decl_form {
	std::string decl_attr;
	std::string decl_type;
	std::string decl_name;
	std::string decl_value;
};
struct tag_decl_form {
	std::string tagname;
	unsigned long long value = 0;
};

struct variables_decl {
public:
	variables_decl();
	
	void set(std::string var_name, unsigned char value[]);
	void set(std::string var_name, unsigned char value[], unsigned long long length);

	unsigned char* get(std::string var_name);
	void deleteVar(std::string var_name);

	void setVariablesTree(code_file_decl_form branch);
	code_file_decl_form getVariableBranch(std::string var_name);

	std::vector<code_file_decl_form> getVariablesTree();

	void setTag(std::string tagname, unsigned long long value);
	unsigned long long getTag(std::string tagname);
	void setTagsMap(std::map<std::string, unsigned long long> map);
	std::map<std::string, unsigned long long> getTagsMap();

	void setTagsBranch(tag_decl_form branch);
	tag_decl_form getTagBranch(std::string tagname);
	std::vector<tag_decl_form> getTagsTree();
private:
	std::map<std::string, m_container> vars;
	std::map<std::string, code_file_decl_form> variables_tree;
	std::vector<code_file_decl_form> variables_vector;

	std::map<std::string, unsigned long long> tags;
	std::map<std::string, tag_decl_form> tags_tree;
	std::vector<tag_decl_form> tags_vector;
};

variables_decl build_variables_decl_tree(std::string filename);