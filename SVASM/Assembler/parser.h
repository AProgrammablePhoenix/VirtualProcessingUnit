#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>

constexpr size_t SAFE_INST_CAP = 5;
constexpr size_t SAFE_ARGS_CAP = 2;

constexpr int SVAS_OK = 0;
constexpr int FILE_ERROR = -1; // input file is probable doesn't exist
constexpr int EMPTY_FILE = -2; // input file is empty
constexpr int INST_ERROR = -3; // error on instruction (probable doesn't exist or syntax error)
constexpr int EXTRACOMMA = -4; // Unexpected comma after last instruction argument
constexpr int ERROR_TYPE = -5; // Value type can't be determined
constexpr int WRONGNARGS = -6; // Wrong number of arguments associated with specific instruction
constexpr int ARGV_ERROR = -7; // Argument value is not correct/valid/allowed for specific instruction

struct codeline {
	codeline() {
		this->instruction.reserve(SAFE_INST_CAP);
		this->arguments.reserve(SAFE_ARGS_CAP);
	}

	std::string instruction = "";
	std::vector<std::string> arguments;
};

int main_parse(const std::string& filename, std::vector<codeline>& out_parsed, std::unordered_set<std::string>& labels);