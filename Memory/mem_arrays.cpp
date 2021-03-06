#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "../Registers/regs_decl.h"
#include "memory_decl.h"
#include "mem_arrays.h"

str_mem_array::str_mem_array(regs* _registers, unsigned long long size) {
	if (!this->initialized) {
		if (this->container != NULL) {
			delete[] this->container;
		}
		this->registers = _registers;
		this->container = new std::string[size];
		this->container_size = size;
		this->values_type = "class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >";
	}
}
void str_mem_array::getAt(unsigned long long index) {
	if (this->initialized) {
		if (index < this->container_size) {
			this->registers->sr->set(this->container[index]);
		}
	}
}
void str_mem_array::setAt(unsigned long long index) {
	if (this->initialized) {
		if (index < this->container_size) {
			this->container[index] = this->registers->sr->get();
		}
	}
}

unum_mem_array::unum_mem_array(regs* _registers, unsigned long long size) {
	if (!this->initialized) {
		if (this->container != NULL) {
			delete[] this->container;
		}
		this->registers = _registers;
		this->container = new unsigned long long[size];
		this->container_size = size;
		this->values_type = "unsigned __int64";
	}
}
void unum_mem_array::getAt(unsigned long long index) {
	if (this->initialized) {
		if (index < this->container_size) {
			this->registers->rdx->set(this->container[index]);
		}
	}
}
void unum_mem_array::setAt(unsigned long long index) {
	if (this->initialized) {
		if (index < this->container_size) {
			this->registers->rdx->set(this->container[index]);
		}
	}
}

mem_arrays::mem_arrays(regs* _registers) {
	this->registers = _registers;
}
void mem_arrays::makeArray(std::string name, std::string type,  unsigned long long size) {
	if (!this->arrays_table.count(name)) {
		if (type == "unsigned __int64" || type == "unsigned number") {
			unum_mem_array _array = unum_mem_array(this->registers, size);
			this->unsigned_number_arrays[name] = _array;
			this->arrays_table[name] = &this->unsigned_number_arrays[name];
			this->types_table[name] = "unsigned __int64";
		}
		else if (type == "class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >" || type == "string") {
			str_mem_array _array = str_mem_array(this->registers, size);
			this->string_arrays[name] = _array;
			this->arrays_table[name] = &this->string_arrays[name];
			this->types_table[name] = "class std::basic_string<char, struct std::char_traits<char>, class std::allocator<char> >";
		}
	}
}
void mem_arrays::getArray(std::string arr_name, unsigned long long index) {
	if (this->arrays_table.count(arr_name)) {
		if (types_table[arr_name] == "unsigned __int64") {
			((mem_array_int<unsigned long long>*)arrays_table[arr_name])->getAt(index);
		}
		else {
			((mem_array_int<std::string>*)arrays_table[arr_name])->getAt(index);
		}
	}
}
void mem_arrays::setArray(std::string arr_name, unsigned long long index) {
	if (this->arrays_table.count(arr_name)) {
		if (types_table[arr_name] == "unsigned __int64") {
			((mem_array_int<unsigned long long>*)arrays_table[arr_name])->setAt(index);
		}
		else {
			((mem_array_int<std::string>*)arrays_table[arr_name])->setAt(index);
		}
	}
}