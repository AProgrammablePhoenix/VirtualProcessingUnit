#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "../Registers/regs_decl.h"
#include "memory_decl.h"

typedef struct memory memory;

struct __struct__ {
public:
	__struct__();
	__struct__(regs* _registers, memory* _mem);
	
	void declProperty(std::string property_name);
	void get(std::string property_name);
	void set(std::string property_name);
private:
	regs* registers = NULL;
	memory* mem = NULL;
	bool initialized = false;

	std::map<std::string, std::string> string_properties;
	std::map<std::string, unsigned long long> unum_properties;
	std::map<std::string, long long> snum_properties;

	std::map<std::string, std::string> types_table;
};

struct mem_structs {
public:
	mem_structs();
	mem_structs(regs* _registers, memory* _mem);
	void makeStruct(std::string structure_name);
	void selectStruct(std::string structure_name);
	void declStructProperty(std::string property_name);
	void getStructProperty(std::string property_name);
	void setStructProperty(std::string property_name);
private:
	regs* registers;
	memory* mem;

	std::map<std::string, __struct__> structs_container;
};