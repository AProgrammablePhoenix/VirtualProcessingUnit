#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <typeinfo>

#include "../Registers/regs_decl.h"
#include "memory_decl.h"

template<typename T>
struct dyn_var_int {
public:
	virtual void dynget() {
		return;
	}
	virtual void dynset() {
		return;
	}
private:
	regs* registers = NULL;
	bool initialized = false;
	T content;
	std::string value_type = "";
};
struct dyn_str_var : dyn_var_int<std::string> {
public:
	dyn_str_var();
	dyn_str_var(regs* _registers);
	void dynget();
	void dynset();
private:
	regs* registers = NULL;
	bool initialized = false;
	std::string content;
	std::string value_type = "";
};
struct dyn_unum_var : dyn_var_int<unsigned long long> {
public:
	dyn_unum_var();
	dyn_unum_var(regs* _registers);
	void dynget();
	void dynset();
private:
	regs* registers = NULL;
	bool initialized = false;
	unsigned long long content = 0;
	std::string value_type = "";
};
struct dyn_snum_var : dyn_var_int<long long> {
public:
	dyn_snum_var();
	dyn_snum_var(regs* _registers);
	void dynget();
	void dynset();
private:
	regs* registers = NULL;
	bool initialized = false;
	long long content = 0;
	std::string value_type = "";
};

struct mem_dyn_vars {
public:
	mem_dyn_vars();
	mem_dyn_vars(regs* _registers);
	void makeDynVar(std::string name, std::string type);
	void dynGetVar(std::string name);
	void dynSetVar(std::string name);

	std::string getVarType(std::string name);
private:
	regs* registers;

	std::map<std::string, dyn_str_var> dyn_string_vars;
	std::map<std::string, dyn_unum_var> dyn_unsigned_number_vars;

	std::map<std::string, void*> variables_table;
	std::map<std::string, std::string> types_table;
};