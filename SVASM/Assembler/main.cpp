#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>

#if defined(__linux__)
#include <cstring>
#include <stdio.h>
#endif

#include "../../VirtualAssembler/assembler.h"
#include "compiler.h"
#include "parser.h"

// Used to map virtual line to real line number
std::map<uint64_t, uint64_t> vline_rline;
std::string source_file = "";

void test_exit(const int& ret_code) {
	if (ret_code) {
		std::cerr << "SVAS returned with code: " << ret_code << std::endl;
		std::exit(ret_code);
	}
}

int main(int argc, char* argv[]) {
	if (argc < 3) {
		std::cerr << "Usage: svas <inputFile> <outputFile>" << std::endl;
		return 1;
	}

	int exit_code = false;

	std::string inputFile = source_file = argv[1];
	std::string outputFile = argv[2];

	std::vector<codeline> parsed;
	std::unordered_set<std::string> labels;
	exit_code |= main_parse(inputFile, parsed, labels);
	test_exit(exit_code);

	std::vector<tokenized> tokens;
	exit_code |= tokenizer(labels, parsed, tokens);
	test_exit(exit_code);

	std::vector<action> preprocessed;
	exit_code |= preprocTokenized(tokens, preprocessed);
	test_exit(exit_code);

	std::vector<byte> out_bytes;
	exit_code |= compileAll(preprocessed, out_bytes);
	test_exit(exit_code);

	std::vector<byte> linked;
	linked.reserve(0x17 + out_bytes.size());
	linked.resize(linked.size() + sizeof(size_t));

	//setup requested memory
	std::copy(out_bytes.begin(), out_bytes.begin() + 8, linked.begin());

	// setup main header
	linked.emplace_back(0);
	for (byte i = 0; i < 8; i++)
		linked.emplace_back(0);
	byte* temp = nullptr;

	ULLTOA(out_bytes.size() - sizeof(size_t), &temp); // take back bytes of requested memory (first 8 bytes normally)
	for (byte i = 0; i < sizeof(size_t); i++)
		linked.emplace_back(temp[i]);
	delete[] temp;

	for (size_t i = sizeof(size_t); i < out_bytes.size(); i++)
		linked.emplace_back(out_bytes[i]);

	byte* linkedBytes = new byte[linked.size()];
	std::copy(linked.begin(), linked.end(), linkedBytes);

	std::ofstream output(outputFile, std::ios::out | std::ios::binary);

	if (!output) {
		std::cerr << "Error while opening output file for writing" << std::endl;
		std::cerr << "SVAS returned with code: 2" << std::endl;
		std::exit(2);

	}
	output.write((const char*)linkedBytes, linked.size());
	output.close();

	std::cout << "Linking of '" << inputFile << "': done." << std::endl;
	std::cout << "SVAS returned with code: " << exit_code << std::endl;
	return 0;
}