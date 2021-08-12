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
extern std::map<virtual_actions, byte> map_FPR_mov_2nd_opc;

extern std::map<virtual_actions, byte> map_FPR_mul_2nd_opc;
extern std::map<virtual_actions, byte> map_FPR_div_2nd_opc;
extern std::map<virtual_actions, byte> map_FPR_add_2nd_opc;
extern std::map<virtual_actions, byte> map_FPR_sub_2nd_opc;

extern std::map<virtual_actions, byte>& ops;

extern std::unordered_set<byte> zero_args_opcodes;
extern std::unordered_set<byte> uint64_args_opcodes;
extern std::unordered_set<byte> reg_args_opcodes;
extern std::unordered_set<byte> parted_opcodes;

std::vector<byte> as(std::string filename);