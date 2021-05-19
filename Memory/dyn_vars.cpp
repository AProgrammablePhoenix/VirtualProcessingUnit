#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "../Registers/regs_decl.h"
#include "memory_decl.h"
#include "dyn_vars.h"

#define CHAR_TYPE "char"
#define DOUBLE_TYPE "double"
#define SNUM_TYPE "__int64"
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

dyn_char_var::dyn_char_var() {
	if (!this->initialized) {
		if (this->content != '\0') {
			this->content = '\0';
		}
	}
}
dyn_char_var::dyn_char_var(regs* _registers) {
	if (!this->initialized) {
		if (!this->content != '\0') {
			this->content = '\0';
		}
		this->registers = _registers;
		this->value_type = "";
		this->initialized = true;
	}
}
void dyn_char_var::dynget() {
	if (this->initialized) {
		this->registers->cr->set(this->content);
	}
}
void dyn_char_var::dynset() {
	if (this->initialized) {
		this->content = this->registers->cr->get();
	}
}

dyn_unum_var::dyn_unum_var() {
	if (!this->initialized) {
		if (this->content) {
			this->content = 0;
		}
	}
}
dyn_unum_var::dyn_unum_var(regs* _registers) {
	if (!this->initialized) {
		if (this->content) {
			this->content = 0;
		}
		this->registers = _registers;
		this->value_type = CHAR_TYPE;
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

dyn_snum_var::dyn_snum_var() {
	if (!this->initialized) {
		if (this->content) {
			this->content = 0;
		}
	}
}
dyn_snum_var::dyn_snum_var(regs* _registers) {
	if (!this->initialized) {
		if (this->content) {
			this->content = 0;
		}
		this->registers = _registers;
		this->value_type = SNUM_TYPE;
		this->initialized = true;
	}
}
void dyn_snum_var::dynget() {
	if (this->initialized) {
		this->registers->rdx->set((unsigned long long)this->content);
	}
}
void dyn_snum_var::dynset() {
	if (this->initialized) {
		this->content = (long long)this->registers->rdx->get();
	}
}

dyn_double_var::dyn_double_var() {
	if (!this->initialized) {
		if (this->content) {
			this->content = 0;
		}
	}
}
dyn_double_var::dyn_double_var(regs* _registers) {
	if (!this->initialized) {
		if (this->content) {
			this->content = 0;
		}
		this->registers = _registers;
		this->value_type = DOUBLE_TYPE;
		this->initialized = true;
	}
}
void dyn_double_var::dynget() {
	if (this->initialized) {
		this->registers->dr->set(this->content);
	}
}
void dyn_double_var::dynset() {
	if (this->initialized) {
		this->content = this->registers->dr->get();
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
			this->variables_table[name] = std::make_shared<dyn_unum_var>(this->dyn_unsigned_number_vars[name]);
			this->types_table[name] = UNUM_TYPE;
		}
		else if (type == SNUM_TYPE) {
			dyn_snum_var _var = dyn_snum_var(this->registers);
			this->dyn_signed_number_vars[name] = _var;
			this->variables_table[name] = std::make_shared<dyn_snum_var>(this->dyn_signed_number_vars[name]);
			this->types_table[name] = SNUM_TYPE;
		}
		else if (type == STR_TYPE) {
			dyn_str_var _var = dyn_str_var(this->registers);
			this->dyn_string_vars[name] = _var;
			this->variables_table[name] = std::make_shared<dyn_str_var>(this->dyn_string_vars[name]);
			this->types_table[name] = STR_TYPE;
		}
		else if (type == CHAR_TYPE) {
			dyn_char_var _var = dyn_char_var(this->registers);
			this->dyn_char_vars[name] = _var;
			this->variables_table[name] = std::make_shared<dyn_char_var>(this->dyn_char_vars[name]);
			this->types_table[name] = CHAR_TYPE;
		}
		else if (type == DOUBLE_TYPE) {
			dyn_double_var _var = dyn_double_var(this->registers);
			this->dyn_double_vars[name] = _var;
			this->variables_table[name] = std::make_shared<dyn_double_var>(this->dyn_double_vars[name]);
			this->types_table[name] = DOUBLE_TYPE;
		}
	}
}
void mem_dyn_vars::dynGetVar(std::string name) {
	if (this->variables_table.count(name)) {
		if (this->types_table[name] == UNUM_TYPE) {
			(std::static_pointer_cast<dyn_var_int<unsigned long long>>(this->variables_table[name]))->dynget();
		}
		else if (this->types_table[name] == SNUM_TYPE) {
			(std::static_pointer_cast<dyn_var_int<long long>>(this->variables_table[name]))->dynget();
		}
		else if (this->types_table[name] == STR_TYPE) {
			(std::static_pointer_cast<dyn_var_int<std::string>>(this->variables_table[name]))->dynget();
		}
		else if (this->types_table[name] == CHAR_TYPE) {
			(std::static_pointer_cast<dyn_var_int<char>>(this->variables_table[name]))->dynget();
		}
		else if (this->types_table[name] == DOUBLE_TYPE) {
			(std::static_pointer_cast<dyn_var_int<double>>(this->variables_table[name]))->dynget();
		}
	}
}
void mem_dyn_vars::dynSetVar(std::string name) {
	if (this->variables_table.count(name)) {
		if (this->types_table[name] == UNUM_TYPE) {
			(std::static_pointer_cast<dyn_var_int<unsigned long long>>(this->variables_table[name]))->dynset();
		}
		else if (this->types_table[name] == SNUM_TYPE) {
			(std::static_pointer_cast<dyn_var_int<long long>>(this->variables_table[name]))->dynset();
		}
		else if (this->types_table[name] == STR_TYPE) {
			(std::static_pointer_cast<dyn_var_int<std::string>>(this->variables_table[name]))->dynset();
		}
		else if (this->types_table[name] == CHAR_TYPE) {
			(std::static_pointer_cast<dyn_var_int<char>>(this->variables_table[name]))->dynset();
		}
		else if (this->types_table[name] == DOUBLE_TYPE) {
			(std::static_pointer_cast<dyn_var_int<double>>(this->variables_table[name]))->dynset();
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