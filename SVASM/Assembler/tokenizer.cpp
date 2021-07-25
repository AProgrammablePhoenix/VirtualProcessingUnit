#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

#include "compiler.h"

static const std::unordered_set<std::string> registers = {
	"ax",
	"bx",
	"cx",
	"dx",
	"eax",
	"ebx",
	"ecx",
	"edx",
	"rax",
	"rbx",
	"rcx",
	"rdx",

	"sr",
	"dr",
	"cr"
};

// checks whether s represent a number or not, ignore out_type if returns false
static bool isNum(const std::string& s, tokenTypes& out_type) {
	if (s.empty())
		return false;

	if (s.size() > 2) {
		if (s.substr(0, 2) == "0x") {
			out_type = tokenTypes::hex_n;
			std::string hex = s.substr(2);

			for (const char c : hex) {
				if (!std::isxdigit(c))
					return false;
			}

			return true;
		}
		else {
			size_t nPoints = 0;

			for (const char c : s) {
				if (!std::isdigit(c)) {
					if (c == '.')
						nPoints++;
					else {
						if (c != '-')
							return false;
					}

					if (nPoints > 1)
						return false;
				}
			}
			if (nPoints) {
				out_type = tokenTypes::double_n;
				return true;
			}
		}
	}

	if (s[0] == '-') {
		if (s.size() < 2)
			return false;

		out_type = tokenTypes::signed_n;
		for (size_t i = 1; i < s.size(); i++) {
			if (!std::isdigit(s[i]))
				return false;
		}
		return true;
	}
	else {
		out_type = tokenTypes::unsigned_n;
		for (const char c : s) {
			if (!std::isdigit(c))
				return false;
		}
		return true;
	}
}
// check whether s represents a string literal or not
static bool isStr(const std::string& s) {
	if (s.empty() || s.size() < 2)
		return false;

	if (s[0] != '"' || s.back() != '"')
		return false;

	return true;
}
// check whether s represents a memory address or not
static bool isAddr(const std::string& s) {
	// s size has to be >= 3, because an unsigned number (the address) has to be at least present between the braces
	if (s.empty() || s.size() < 3)
		return false;

	if (s[0] != '[' || s.back() != ']')
		return false;

	return true;
}
// check whether s represents a memory address stores in a register or not
static bool isRegAddr(const std::string& s) {
	if (!isAddr(s))
		return false;

	std::string potential_reg = s.substr(1, s.size() - 2);
	if (registers.find(potential_reg) != registers.end()) {
		if (potential_reg != "sr" && potential_reg != "cr" && potential_reg != "dr")
			return true;
	}

	return false;
}


int tokenizeArgument(const std::unordered_set<std::string> labels, const std::string& arg, token& out_tokenized) {
	if (registers.find(arg) != registers.end()) {
		out_tokenized = token(arg, tokenTypes::reg);
		return OK;
	}
	else {
		tokenTypes tempType;
		if (isNum(arg, tempType)) {
			out_tokenized = token(arg, tempType);
			return OK;
		}
		else if (isStr(arg)) {
			out_tokenized = token(arg, tokenTypes::str);
			return OK;
		}
		else if (isRegAddr(arg)) {
			out_tokenized = token(arg, tokenTypes::stored_addr_reg);
			return OK;
		}
		else {
			if (labels.find(arg) != labels.end()) {
				out_tokenized = token(arg, tokenTypes::label);
				return OK;
			}
		}

		return ERROR_TYPE;
	}
}
int tokenizeCodeline(const std::unordered_set<std::string>& labels, const codeline& parsed, tokenized& out_tokenized) {
	if (parsed.instruction.back() == ':' && parsed.arguments.size() == 0) {
		out_tokenized.instruction = "[labeldef]";
		out_tokenized.arguments.emplace_back(parsed.instruction.substr(0, parsed.instruction.size() -1), tokenTypes::label);
		return OK;
	}
	for (const std::string arg : parsed.arguments) {
		token targ;
		if (tokenizeArgument(labels, arg, targ))
			return ERROR_TYPE;

		if (targ.type == tokenTypes::hex_n) {
			targ.element = std::to_string(std::stoull(targ.element, 0, 0x10));
			targ.type = tokenTypes::unsigned_n;
		}

		out_tokenized.arguments.emplace_back(targ);
	}
	out_tokenized.instruction = parsed.instruction;

	return OK;
}
int tokenizer(const std::unordered_set<std::string>& labels, const std::vector<codeline>& parsed,
		std::vector<tokenized>& out_tokenized) {
	out_tokenized.reserve(parsed.size());
	
	for (size_t i = 0; i < parsed.size(); i++) {
		tokenized temp;

		if (tokenizeCodeline(labels, parsed[i], temp))
			return ERROR_TYPE;

		out_tokenized.emplace_back(temp);
	}

	return OK;
}