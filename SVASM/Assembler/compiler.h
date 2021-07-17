#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../../Actions/action_decl.h"
#include "parser.h"

typedef unsigned char byte;

extern std::map<registries_def, byte> registers_set;
extern std::map<virtual_actions, byte> instructions_set;

extern std::unordered_set<byte> zero_args_opcodes;
extern std::unordered_set<byte> uint64_args_opcodes;
extern std::unordered_set<byte> reg_args_opcodes;


enum class tokenTypes {
	error_type = -1,
	unsigned_n = 0,
	signed_n = 1,
	hex_n = 2,
	double_n = 3,
	str = 4,
	reg = 5,
};
struct token {
	token(const std::string& _e, const tokenTypes& _t) {
		this->element = _e;
		this->type = _t;
	}
	token() { };

	std::string element = "";
	tokenTypes type = tokenTypes::error_type;
};
struct tokenized {
	std::string instruction = "";
	std::vector<token> arguments;
};

// defined in tokenizer.cpp
extern int tokenizer(const std::vector<codeline>& parsed, std::vector<tokenized>& out_tokenized);

int preprocTokenized(const std::vector<tokenized> tokens, std::vector<action>& out_actions);
int compileAll(const std::vector<action>& raw_actions, std::vector<byte>& out_bytes);