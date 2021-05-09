#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>

typedef unsigned char byte;

extern std::map<registries_def, byte> registers_set;
extern std::map<virtual_actions, unsigned char> instructions_set;

template<typename Tk, typename Tv>
Tk findKeyByValue(std::map<Tk, Tv> map, Tv value) {
	for (auto const& pair : map) {
		if (pair.second == value)
			return pair.first;
	}

	return Tk();
}