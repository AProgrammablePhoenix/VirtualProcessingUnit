#pragma once

#include <iostream>
#include <map>
#include <string>
#include <unordered_set>
#include <vector>

typedef unsigned char byte;

extern std::map<registries_def, byte> registers_set;
extern std::map<extra_registries, byte> fp_registers_set;
extern std::map<virtual_actions, unsigned char> instructions_set;
extern std::map<virtual_actions, byte> map_FPR_set_2nd_opc;

extern std::map<virtual_actions, byte>& ops;

extern std::unordered_set<byte> zero_args_opcodes;
extern std::unordered_set<byte> uint64_args_opcodes;
extern std::unordered_set<byte> reg_args_opcodes;
extern std::unordered_set<byte> parted_opcodes;

template<typename Tk, typename Tv>
Tk findKeyByValue(std::map<Tk, Tv> map, Tv value) {
	for (auto const& pair : map) {
		if (pair.second == value)
			return pair.first;
	}

	return Tk();
}