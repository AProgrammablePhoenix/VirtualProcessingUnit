#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>

typedef unsigned char byte;

extern std::map<registries_def, byte> registers_set;
extern std::map<virtual_actions, unsigned char> instructions_set;

extern std::map<virtual_actions, byte>& ops;
extern std::unordered_set<byte> zero_args_opcodes;
extern std::unordered_set<byte> uint64_args_opcodes;
extern std::unordered_set<byte> reg_args_opcodes;

std::vector<byte> as(std::string filename);