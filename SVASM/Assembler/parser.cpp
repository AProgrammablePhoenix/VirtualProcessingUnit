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

int fetch_file(const std::string& filename, std::string& out_read) {
	std::ifstream file(filename);

	if (file) {
		const auto filesize = std::filesystem::file_size(filename);

		out_read = std::string(filesize, 0);
		file.read(out_read.data(), filesize);

		return OK;
	}

	return FILE_ERROR;
}
int fetch_lines(const std::string& content, std::vector<std::string>& lines) {
	std::stringstream ss(content);
	std::string line;

	if (content.empty()) {
		return EMPTY_FILE;
	}

	while (std::getline(ss, line)) {
		if (line.empty() || line.size() < 1)
			continue;

		lines.push_back(line);
	}

	return OK;
}
int parse_line(const std::string& line, codeline& out_parsed) {
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
			return OK;
		}

		temp.push_back(c);
	}

	if (!found) {
		out_parsed.instruction = temp;
		return OK;
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
				return EXTRACOMMA;
			else if (line[i + 1] == ' ')
				for (size_t j = i + 1; j < line.size() && line[j] == ' '; i++, j++);

			continue;
		}

		temp.push_back(line[i]);
	}
	if (!temp.empty())
		out_parsed.arguments.emplace_back(temp);

	return OK;
}

int main_parse(const std::string& filename, std::vector<codeline>& out_parsed, std::unordered_set<std::string>& labels) {
	std::string file_content;

	if (fetch_file(filename, file_content))
		return FILE_ERROR;
	std::vector<std::string> lines;

	if (fetch_lines(file_content, lines))
		return EMPTY_FILE;
	
	out_parsed.reserve(lines.size());
	labels.reserve(lines.size() / 2);

	for (size_t i = 0; i < lines.size(); i++) {
		codeline parsed;
		if (parse_line(lines[i], parsed))
			return EXTRACOMMA;

		if (parsed.instruction.back() == ':' && parsed.arguments.size() == 0) {
			labels.insert(parsed.instruction.substr(0, parsed.instruction.size() - 1));
		}

		out_parsed.emplace_back(parsed);
	}

	return OK;
}