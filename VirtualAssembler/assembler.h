#pragma once

typedef unsigned char byte;

#include <iostream>
#include <map>
#include <string>
#include <vector>

extern std::map<registries_def, byte> registers_set;
extern std::map<virtual_actions, unsigned char> instructions_set;

std::vector<byte> as(std::string filename);