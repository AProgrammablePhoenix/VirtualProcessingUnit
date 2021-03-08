#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "../Registers/regs_decl.h"
#include "memory_decl.h"
#include "dyn_vars.h"

#define STR_TYPE "class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >"
#define UNUM_TYPE "unsigned __int64"

dyn_str_var::dyn_str_var() {
	if (!this->initialized) {
		if (!this->content.empty()) {
			this->content.clear();
		}
	}
}
dyn_str_var::dyn_str_var(regs* _registers) {
	if (!this->initialized) {
		if (!this->content.empty()) {
			this->content.clear();
		}
		this->registers = _registers;
		this->value_type = STR_TYPE;
		this->initialized = true;
	}
}
void dyn_str_var::dynget() {
	if (this->initialized) {
		this->registers->sr->set(this->content);
	}
}
void dyn_str_var::dynset() {
	if (this->initialized) {
		this->content = this->registers->sr->get();
	}
}

dyn_unum_var::dyn_unum_var() {
	if (!this->initialized) {
		if (this->content) {
			this->content = NULL;
		}
	}
}
dyn_unum_var::dyn_unum_var(regs* _registers) {
	if (!this->initialized) {
		if (this->content) {
			this->content = NULL;
		}
		this->registers = _registers;
		this->value_type = UNUM_TYPE;
		this->initialized = true;
	}
}
void dyn_unum_var::dynget() {
	if (this->initialized) {
		this->registers->rdx->set(this->content);
	}
}
void dyn_unum_var::dynset() {
	if (this->initialized) {
		this->content = this->registers->rdx->get();
	}
}

mem_dyn_vars::mem_dyn_vars() {
	this->registers = NULL;
}
mem_dyn_vars::mem_dyn_vars(regs* _registers) {
	this->registers = _registers;
}
void mem_dyn_vars::makeDynVar(std::string name, std::string type) {
	if (!this->variables_table.count(name)) {
		if (type == UNUM_TYPE) {
			dyn_unum_var _var = dyn_unum_var(this->registers);
			this->dyn_unsigned_number_vars[name] = _var;
			this->variables_table[name] = &this->dyn_unsigned_number_vars[name];
			this->types_table[name] = UNUM_TYPE;
		}
		else if (type == STR_TYPE) {
			dyn_str_var _var = dyn_str_var(this->registers);
			this->dyn_string_vars[name] = _var;
			this->variables_table[name] = &this->dyn_string_vars[name];
			this->types_table[name] = STR_TYPE;
		}
	}
}
void mem_dyn_vars::dynGetVar(std::string name) {
	if (this->variables_table.count(name)) {
		if (this->types_table[name] == UNUM_TYPE) {
			((dyn_var_int<unsigned long long>*)this->variables_table[name])->dynget();
		}
		else if (this->types_table[name] == STR_TYPE) {
			((dyn_var_int<std::string>*)this->variables_table[name])->dynget();
		}
	}
}
void mem_dyn_vars::dynSetVar(std::string name) {
	if (this->variables_table.count(name)) {
		if (this->types_table[name] == UNUM_TYPE) {
			((dyn_var_int<unsigned long long>*)this->variables_table[name])->dynset();
		}
		else if (this->types_table[name] == UNUM_TYPE) {
			((dyn_var_int<std::string>*)this->variables_table[name])->dynset();
		}
	}
}
std::string mem_dyn_vars::getVarType(std::string name) {
	if (this->types_table.count(name)) {
		return this->types_table[name];
	}
	else {
		return "UNDEFINED_VARIABLE";
	}
}