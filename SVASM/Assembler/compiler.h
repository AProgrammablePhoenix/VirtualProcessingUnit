#pragma once

#include <chrono>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../../Actions/action_decl.h"
#include "../../VirtualAssembler/assembler.h"
#include "parser.h"


enum class tokenTypes {
	error_type = -1,
	unsigned_n,
	signed_n,
	hex_n,
	double_n,
	str,
	reg,
	fp_reg,
	stored_addr_reg,
	stored_addr_raw,
	label,
	safety_directive,
	memory_directive
};
struct token {
	token(const std::string& _e, const tokenTypes& _t) {
		this->element.reserve(SAFE_ARGS_CAP);

		this->element = _e;
		this->type = _t;
	}
	token() { this->element.reserve(SAFE_ARGS_CAP); }

	std::string element = "";
	tokenTypes type = tokenTypes::error_type;
};
struct tokenized {
	tokenized() {
		this->instruction.reserve(SAFE_INST_CAP);
		this->arguments.reserve(SAFE_ARGS_CAP);
	}

	std::string instruction = "";
	std::vector<token> arguments;
};

// defined in tokenizer.cpp
extern int tokenizer(const std::unordered_set<std::string>& labels, const std::vector<codeline>& parsed,
	std::vector<tokenized>& out_tokenized);

int preprocTokenized(const std::vector<tokenized> tokens, std::vector<action>& out_actions);
int compileAll(const std::vector<action>& raw_actions, std::vector<byte>& out_bytes);