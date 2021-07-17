#include <iostream>
#include <map>
#include <string>
#include <unordered_map>

#if defined(__linux__)
#include <cstring>
#include <stdio.h>
#endif

#include "compiler.h"
#include "parser.h"

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

	std::string inputFile = argv[1];
	std::string outputFile = argv[2];

	std::vector<codeline> parsed;
	exit_code |= main_parse(inputFile, parsed);
	test_exit(exit_code);

	std::vector<tokenized> tokens;
	exit_code |= tokenizer(parsed, tokens);
	test_exit(exit_code);

	std::vector<action> preprocessed;
	exit_code |= preprocTokenized(tokens, preprocessed);
	test_exit(exit_code);

	std::vector<byte> out_bytes;
	exit_code |= compileAll(preprocessed, out_bytes);
	test_exit(exit_code);

	std::vector<byte> linked;

	// setup main header
	linked.push_back(0);
	for (byte i = 0; i < 8; i++)
		linked.push_back(0);
	byte* temp = nullptr;

	ULLTOA(out_bytes.size(), &temp);
	for (byte i = 0; i < sizeof(size_t); i++)
		linked.push_back(temp[i]);
	delete[] temp;

	for (byte b : out_bytes)
		linked.push_back(b);

	byte* linkedBytes = new byte[linked.size()];
	std::copy(linked.begin(), linked.end(), linkedBytes);	

	std::ofstream output(outputFile, std::ios::out | std::ios::binary);
	output.write((const char*)linkedBytes, linked.size());
	output.close();

	std::cout << "Linking of '" << inputFile << "': done." << std::endl;
	std::cout << "SVAS returned with code: " << exit_code << std::endl;
	return 0;
}