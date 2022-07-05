#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

#include "compiler.h"
#include "util.h"

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

	"rbp",
	"rsp",

	"cr"
};
static const std::unordered_set<std::string> fp_regs = {
	"fpr0",
	"fpr1",
	"fpr2",
	"fpr3",
	"efpr0",
	"efpr1",
	"efpr2",
	"efpr3",
	"rfpr0",
	"rfpr1",
	"rfpr2",
	"rfpr3"
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
// check whether s represents a memory address stored in a register or not
static bool isRegAddr(const std::string& s) {
	if (!isAddr(s))
		return false;

	std::string potential_reg = s.substr(1, s.size() - 2);
	if (registers.find(potential_reg) != registers.end()) {
		if (potential_reg != "cr")
			return true;
	}

	return false;
}
// check whether s represents an immediate address or not
static bool isImmAddr(const std::string& s) {
	if (!isAddr(s))
		return false;

	std::string potential_addr = s.substr(1, s.size() - 2);
	tokenTypes tempT;
	if (isNum(potential_addr, tempT)) {
		if (tempT == tokenTypes::hex_n || tempT == tokenTypes::unsigned_n)
			return true;
	}

	return false;
}
// check whether s represents a type quantifier or not
static bool isTQ(const std::string& s) {
	if (s == "QWORD" || s == "qword")
		return true;
	else if (s == "DWORD" || s == "dword")
		return true;
	else if (s == "WORD" || s == "word")
		return true;
	else if (s == "BYTE" || s == "byte")
		return true;
	return false;
}

int tokenizeArgument(const std::unordered_set<std::string> labels, const std::string& arg, token& out_tokenized, uint64_t nline) {
	if (registers.find(arg) != registers.end()) {
		out_tokenized = token(arg, tokenTypes::reg);
		return SVAS_OK;
	}
	else if (fp_regs.find(arg) != fp_regs.end()) {
		out_tokenized = token(arg, tokenTypes::fp_reg);
		return SVAS_OK;
	}
	else {
		tokenTypes tempType;
		if (isNum(arg, tempType)) {
			out_tokenized = token(arg, tempType);
			return SVAS_OK;
		}
		else if (isStr(arg)) {
			out_tokenized = token(arg, tokenTypes::str);
			return SVAS_OK;
		}
		else if (isRegAddr(arg)) {
			out_tokenized = token(arg, tokenTypes::stored_addr_reg);
			return SVAS_OK;
		}
		else if (isImmAddr(arg)) {
			out_tokenized = token(arg, tokenTypes::stored_addr_raw);
			return SVAS_OK;
		}
		else if (isTQ(arg)) {
			out_tokenized = token(arg, tokenTypes::type_quantifier);
			return SVAS_OK;
		}
		else {
			if (labels.find(arg) != labels.end()) {
				out_tokenized = token(arg, tokenTypes::label);
				return SVAS_OK;
			}
		}

		assert_err("Type of variable: " + arg + " cannot be determined", nline, ERROR_TYPE);
	}
}
int tokenizeCodeline(const std::unordered_set<std::string>& labels, const codeline& parsed, tokenized& out_tokenized, uint64_t nline) {
	if (parsed.instruction.back() == ':' && parsed.arguments.size() == 0) {
		out_tokenized.instruction = "[labeldef]";
		out_tokenized.arguments.emplace_back(parsed.instruction.substr(0, parsed.instruction.size() - 1), tokenTypes::label);
		return SVAS_OK;
	}
	else if ((parsed.instruction == "#safe" || parsed.instruction == "#unsafe") && parsed.arguments.size() == 0) {
		out_tokenized.instruction = "[safety_control]";
		out_tokenized.arguments.emplace_back(parsed.instruction.substr(1, parsed.instruction.size() - 1), tokenTypes::safety_directive);
		return SVAS_OK;
	}
	else if ((parsed.instruction == "#resmem") && parsed.arguments.size() == 1) {
		tokenTypes arg_type;
		size_t new_size = 0;

		if (!isNum(parsed.arguments[0], arg_type))
			assert_err("Argument is not a number: " + parsed.arguments[0] + " on: ", "#resmem", parsed.arguments, nline, ARGV_ERROR);
		else if (arg_type != tokenTypes::unsigned_n && arg_type != tokenTypes::hex_n)
			assert_err("Argument is not an unsigned number: " + parsed.arguments[0] + "on: ", "#resmem", parsed.arguments, nline, ARGV_ERROR);
		else if (arg_type == tokenTypes::hex_n)
			new_size = std::stoull(parsed.arguments[0], 0, 0x10);
		else
			new_size = std::stoull(parsed.arguments[0]);

		out_tokenized.instruction = "[memory_control]";
		out_tokenized.arguments.emplace_back(std::to_string(new_size), tokenTypes::memory_directive);
	}

	for (const std::string arg : parsed.arguments) {
		token targ;
		tokenizeArgument(labels, arg, targ, nline);

		if (targ.type == tokenTypes::hex_n) {
			targ.element = std::to_string(std::stoull(targ.element, 0, 0x10));
			targ.type = tokenTypes::unsigned_n;
		}
		else if (targ.type == tokenTypes::stored_addr_raw) {
			tokenTypes tempT;
			targ.element = targ.element.substr(1, targ.element.size() - 2);
			isNum(targ.element, tempT);

			if (tempT == tokenTypes::hex_n)
				targ.element = std::to_string(std::stoull(targ.element, 0, 0x10));
		}

		out_tokenized.arguments.emplace_back(targ);
	}
	out_tokenized.instruction = parsed.instruction;

	return SVAS_OK;
}
int tokenizer(const std::unordered_set<std::string>& labels, const std::vector<codeline>& parsed,
		std::vector<tokenized>& out_tokenized) {
	out_tokenized.reserve(parsed.size());
	
	for (size_t i = 0; i < parsed.size(); i++) {
		tokenized temp;

		tokenizeCodeline(labels, parsed[i], temp, vline_rline[i]);

		out_tokenized.emplace_back(temp);
	}

	return SVAS_OK;
}