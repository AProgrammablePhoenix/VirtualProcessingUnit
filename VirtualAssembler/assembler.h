#pragma once

#include <iostream>
#include <map>
#include <string>
#include <unordered_set>
#include <vector>

#include "../Registers/registers_symbols.h"
#include "../Actions/actions_symbols.h"

typedef unsigned char byte;

extern std::map<comn_registers, byte> registers_set;
extern std::map<comn_registers, byte> fp_registers_set;
extern std::map<virtual_actions, byte> instructions_set;

extern std::map<virtual_actions, byte> map_FPR_mul_2nd_opc;
extern std::map<virtual_actions, byte> map_FPR_div_2nd_opc;
extern std::map<virtual_actions, byte> map_FPR_add_2nd_opc;
extern std::map<virtual_actions, byte> map_FPR_sub_2nd_opc;

extern std::map<virtual_actions, byte>& ops;

extern std::unordered_set<byte> zero_args_opcodes;
extern std::unordered_set<byte> uint64_args_opcodes;
extern std::unordered_set<byte> reg_args_opcodes;

extern std::unordered_set<byte> opt_arg_ops;

std::vector<byte> as(std::string filename);