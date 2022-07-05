#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_set>

#if defined(__linux__)
#include <cstring>
#include <stdio.h>
#endif

#include "parser.h"
#include "util.h"

namespace {
	uint64_t n_emlines = 1; // line counter in a file starts at 1
}

int fetch_file(const std::string& filename, std::string& out_read) {
	std::ifstream file(filename);

	if (file) {
		const auto filesize = std::filesystem::file_size(filename);

		out_read = std::string(filesize, 0);
		file.read(out_read.data(), filesize);

		return SVAS_OK;
	}

	assert_err("Error while opening file", 0, FILE_ERROR);
}
int fetch_lines(const std::string& content, std::vector<std::string>& lines) {
	std::stringstream ss(content);
	std::string line;

	if (content.empty())
		assert_err("Trying to assemble an empty file", 0, EMPTY_FILE);

	while (std::getline(ss, line)) {
		if (line.empty() || line.size() < 1 || line[0] == ';') {
			++n_emlines;
			continue;
		}

		vline_rline[lines.size()] = lines.size() + n_emlines;
		lines.push_back(line);
	}

	return SVAS_OK;
}
int parse_line(const std::string& line, codeline& out_parsed, uint64_t nline) {
	bool found = false;
	std::string temp = "";

	temp.reserve(SAFE_INST_CAP);

	out_parsed.arguments.clear();

	for (const char c : line) {
		if (c == ' ') {
			out_parsed.instruction = temp;
			found = true;
			break;
		}
		else if (c == ':' && !found) {
			out_parsed.instruction = temp + c;
			return SVAS_OK;
		}

		temp.push_back(c);
	}

	if (!found) {
		out_parsed.instruction = temp;
		return SVAS_OK;
	}

	bool isstring = false;

	temp = "";
	for (size_t i = out_parsed.instruction.size() + 1; i < line.size() && line[i]; i++) {
		if (line[i] == '"') {
			if (isstring && line[i - 1] != '\\')
				isstring = false;
			else if (!isstring)
				isstring = true;
		}
		else if (line[i] == ',') {
			if (isstring) {
				temp.push_back(line[i]);
				continue;
			}

			out_parsed.arguments.emplace_back(temp);
			temp = "";

			if (i + 1 >= line.size())
				assert_err("Found an extra comma", nline, EXTRACOMMA);
			else if (line[i + 1] == ' ')
				for (size_t j = i + 1; j < line.size() && line[j] == ' '; i++, j++);

			continue;
		}

		temp.push_back(line[i]);
	}
	if (!temp.empty())
		out_parsed.arguments.emplace_back(temp);

	return SVAS_OK;
}

int main_parse(const std::string& filename, std::vector<codeline>& out_parsed, std::unordered_set<std::string>& labels) {
	std::string file_content;

	fetch_file(filename, file_content);
	std::vector<std::string> lines;

	fetch_lines(file_content, lines);
	
	out_parsed.reserve(lines.size());
	labels.reserve(lines.size() / 2);

	for (size_t i = 0; i < lines.size(); i++) {
		codeline parsed;
		parse_line(lines[i], parsed, vline_rline[i]);

		if (parsed.instruction.back() == ':' && parsed.arguments.size() == 0)
			labels.insert(parsed.instruction.substr(0, parsed.instruction.size() - 1));

		out_parsed.emplace_back(parsed);
	}

	return SVAS_OK;
}