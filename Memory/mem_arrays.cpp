#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "../Registers/regs_decl.h"
#include "memory_decl.h"
#include "mem_arrays.h"

#define STATIC_CHAR_ARRAY "static char"
#define STATIC_DOUBLE_ARRAY "static double"
#define STATIC_SNUM_ARRAY "static __int64"
#define STATIC_STR_ARRAY "static class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >"
#define STATIC_UNUM_ARRAY "static unsigned __int64"

#define DYNAMIC_CHAR_ARRAY "dynamic char"
#define DYNAMIC_DOUBLE_ARRAY "dynamic double"
#define DYNAMIC_SNUM_ARRAY "dynamic __int64"
#define DYNAMIC_STR_ARRAY "dynamic class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >"
#define DYNAMIC_UNUM_ARRAY "dynamic unsigned __int64"

str_mem_array::str_mem_array() {
	if (!this->initialized) {
		if (this->container != NULL) {
			delete[] this->container;
		}
	}
}
str_mem_array::str_mem_array(regs* _registers, unsigned long long size) {
	if (!this->initialized) {
		if (this->container != NULL) {
			delete[] this->container;
		}
		this->registers = _registers;
		this->container = new std::string[size];
		this->container_size = size;
		this->values_type = "class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >";
		this->initialized = true;
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
void str_mem_array::destroy() {
	if (this->container) {
		delete[] this->container;
	}
}

char_mem_array::char_mem_array() {
	if (!this->initialized) {
		if (this->container != NULL) {
			delete[] this->container;
		}
	}
}
char_mem_array::char_mem_array(regs* _registers, unsigned long long size) {
	if (!this->initialized) {
		if (this->container != NULL) {
			delete[] this->container;
		}
		this->registers = _registers;
		this->container = new char[size];
		this->container_size = size;
		this->values_type = "char";
		this->initialized = true;
	}
}
void char_mem_array::getAt(unsigned long long index) {
	if (this->initialized) {
		if (index < this->container_size) {
			this->registers->cr->set(this->container[index]);
		}
	}
}
void char_mem_array::setAt(unsigned long long index) {
	if (this->initialized) {
		if (index < this->container_size) {
			this->container[index] = this->registers->cr->get();
		}
	}
}
void char_mem_array::destroy() {
	if (this->container) {
		delete[] this->container;
	}
}

unum_mem_array::unum_mem_array() {
	if (!this->initialized) {
		if (this->container != NULL) {
			delete[] this->container;
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
		this->initialized = true;
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
			this->container[index] = this->registers->rdx->get();
			return;
		}
	}
}
void unum_mem_array::destroy() {
	if (this->container) {
		delete[] this->container;
	}
}

snum_mem_array::snum_mem_array() {
	if (!this->initialized) {
		if (this->container != NULL) {
			delete[] this->container;
		}
	}
}
snum_mem_array::snum_mem_array(regs* _registers, unsigned long long size) {
	if (!this->initialized) {
		if (this->container != NULL) {
			delete[] this->container;
		}
		this->registers = _registers;
		this->container = new long long[size];
		this->container_size = size;
		this->values_type = "__int64";
		this->initialized = true;
	}
}
void snum_mem_array::getAt(unsigned long long index) {
	if (this->initialized) {
		if (index < this->container_size) {
			this->registers->rdx->set((unsigned long long)this->container[index]);
		}
	}
}
void snum_mem_array::setAt(unsigned long long index) {
	if (this->initialized) {
		if (index < this->container_size) {
			this->container[index] = (long long)this->registers->rdx->get();
		}
	}
}
void snum_mem_array::destroy() {
	if (this->container) {
		delete[] this->container;
	}
}

double_mem_array::double_mem_array() {
	if (!this->initialized) {
		if (this->container != NULL) {
			delete[] this->container;
		}
	}
}
double_mem_array::double_mem_array(regs* _registers, unsigned long long size) {
	if (!this->initialized) {
		if (this->container != NULL) {
			delete[] this->container;
		}
		this->registers = _registers;
		this->container = new double[size];
		this->container_size = size;
		this->values_type = "double";
		this->initialized = true;
	}
}
void double_mem_array::getAt(unsigned long long index) {
	if (this->initialized) {
		if (index < this->container_size) {
			this->registers->dr->set(this->container[index]);
		}
	}
}
void double_mem_array::setAt(unsigned long long index) {
	if (this->initialized) {
		if (index < this->container_size) {
			this->container[index] = this->registers->dr->get();
		}
	}
}
void double_mem_array::destroy() {
	if (this->container) {
		delete[] this->container;
	}
}

dyn_str_array::dyn_str_array() {
	if (!this->initialized) {
		if (!this->container.empty()) {
			this->container.clear();
		}
	}
}
dyn_str_array::dyn_str_array(regs* _registers) {
	if (!this->initialized) {
		if (!this->container.empty()) {
			this->container.clear();
		}
		this->registers = _registers;
		this->values_type = "class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >";
		this->initialized = true;
	}
}
void dyn_str_array::getAt(unsigned long long index) {
	if (this->initialized) {
		if (index < this->container.size()) {
			this->registers->sr->set(this->container[index]);
		}
	}
}
void dyn_str_array::setAt(unsigned long long index) {
	if (this->initialized) {
		if (index < this->container.size()) {
			this->container[index] = this->registers->sr->get();
		}
		else {
			this->container.push_back(this->registers->sr->get());
		}
	}
}
void dyn_str_array::getSize() {
	this->registers->rdx->set(this->container.size());
}

dyn_char_array::dyn_char_array() {
	if (!this->initialized) {
		if (!this->container.empty()) {
			this->container.clear();
		}
	}
}
dyn_char_array::dyn_char_array(regs* _registers) {
	if (!this->initialized) {
		if (!this->container.empty()) {
			this->container.clear();
		}
		this->registers = _registers;
		this->values_type = "char";
		this->initialized = true;
	}
}
void dyn_char_array::getAt(unsigned long long index) {
	if (this->initialized) {
		if (index < this->container.size()) {
			this->registers->cr->set(this->container[index]);
		}
	}
}
void dyn_char_array::setAt(unsigned long long index) {
	if (this->initialized) {
		if (index < this->container.size()) {
			this->container[index] = this->registers->cr->get();
		}
		else {
			this->container.push_back(this->registers->cr->get());
		}
	}
}
void dyn_char_array::getSize() {
	this->registers->rdx->set(this->container.size());
}

dyn_unum_array::dyn_unum_array() {
	if (!this->initialized) {
		if (!this->container.empty()) {
			this->container.clear();
		}
	}
}
dyn_unum_array::dyn_unum_array(regs* _registers) {
	if (!this->initialized) {
		if (!this->container.empty()) {
			this->container.clear();
		}
		this->registers = _registers;
		this->values_types = "unsigned __int64";
		this->initialized = true;
	}
}
void dyn_unum_array::getAt(unsigned long long index) {
	if (this->initialized) {
		if (index < this->container.size()) {
			this->registers->rdx->set(this->container[index]);
		}
	}
}
void dyn_unum_array::setAt(unsigned long long index) {
	if (this->initialized) {
		if (index < this->container.size()) {
			this->container[index] = this->registers->rdx->get();
		}
		else {
			this->container.push_back(this->registers->rdx->get());
		}
	}
}
void dyn_unum_array::getSize() {
	this->registers->rdx->set(this->container.size());
}

dyn_snum_array::dyn_snum_array() {
	if (!this->initialized) {
		if (!this->container.empty()) {
			this->container.clear();
		}
	}
}
dyn_snum_array::dyn_snum_array(regs* _registers) {
	if (!this->initialized) {
		if (!this->container.empty()) {
			this->container.clear();
		}
		this->registers = _registers;
		this->values_types = "__int64";
		this->initialized = true;
	}
}
void dyn_snum_array::getAt(unsigned long long index) {
	if (this->initialized) {
		if (index < this->container.size()) {
			this->registers->rdx->set((unsigned long long)this->container[index]);
		}
	}
}
void dyn_snum_array::setAt(unsigned long long index) {
	if (this->initialized) {
		if (index < this->container.size()) {
			this->container[index] = (long long)this->registers->rdx->get();
		}
		else {
			this->container.push_back((long long)this->registers->rdx->get());
		}
	}
}
void dyn_snum_array::getSize() {
	this->registers->rdx->set(this->container.size());
}

dyn_double_array::dyn_double_array() {
	if (!this->initialized) {
		if (!this->container.empty()) {
			this->container.clear();
		}
	}
}
dyn_double_array::dyn_double_array(regs* _registers) {
	if (!this->initialized) {
		if (!this->container.empty()) {
			this->container.clear();
		}
		this->registers = _registers;
		this->values_types = "double";
		this->initialized = true;
	}
}
void dyn_double_array::getAt(unsigned long long index) {
	if (this->initialized) {
		if (index < this->container.size()) {
			this->registers->dr->set(this->container[index]);
		}
	}
}
void dyn_double_array::setAt(unsigned long long index) {
	if (this->initialized) {
		if (index < this->container.size()) {
			this->container[index] = this->registers->dr->get();
		}
		else {
			this->container.push_back(this->registers->dr->get());
		}
	}
}
void dyn_double_array::getSize() {
	this->registers->rdx->set(this->container.size());
}

mem_arrays::mem_arrays() {
	this->registers = NULL;
}
mem_arrays::mem_arrays(regs* _registers) {
	this->registers = _registers;
}
void mem_arrays::makeArray(std::string name, std::string type,  unsigned long long size) {
	if (!this->arrays_table.count(name)) {
		if (type == STATIC_UNUM_ARRAY || type == "static unsigned number") {
			unum_mem_array _array = unum_mem_array(this->registers, size);
			this->unsigned_number_arrays[name] = _array;
			this->arrays_table[name] = std::make_shared<unum_mem_array>(this->unsigned_number_arrays[name]);
			this->types_table[name] = STATIC_UNUM_ARRAY;
			this->static_arrays.push_back(name);
		}
		else if (type == STATIC_SNUM_ARRAY || type == "static signed number") {
			snum_mem_array _array = snum_mem_array(this->registers, size);
			this->signed_numbers_arrays[name] = _array;
			this->arrays_table[name] = std::make_shared<snum_mem_array>(this->signed_numbers_arrays[name]);
			this->types_table[name] = STATIC_SNUM_ARRAY;
			this->static_arrays.push_back(name);
		}
		else if (type == STATIC_STR_ARRAY || type == "static string") {
			str_mem_array _array = str_mem_array(this->registers, size);
			this->string_arrays[name] = _array;
			this->arrays_table[name] = std::make_shared<str_mem_array>(this->string_arrays[name]);
			this->types_table[name] = STATIC_STR_ARRAY;
			this->static_arrays.push_back(name);
		}
		else if (type == STATIC_CHAR_ARRAY) {
			char_mem_array _array = char_mem_array(this->registers, size);
			this->char_arrays[name] = _array;
			this->arrays_table[name] = std::make_shared<char_mem_array>(this->char_arrays[name]);
			this->types_table[name] = STATIC_CHAR_ARRAY;
			this->static_arrays.push_back(name);
		}
		else if (type == STATIC_DOUBLE_ARRAY) {
			double_mem_array _array = double_mem_array(this->registers, size);
			this->double_arrays[name] = _array;
			this->arrays_table[name] = std::make_shared<double_mem_array>(this->double_arrays[name]);
			this->types_table[name] = STATIC_DOUBLE_ARRAY;
			this->static_arrays.push_back(name);
		}
		else if (type == DYNAMIC_UNUM_ARRAY || type == "dynamic unsigned number") {
			dyn_unum_array _array = dyn_unum_array(this->registers);
			this->dyn_unsigned_number_arrays[name] = _array;
			this->arrays_table[name] = std::make_shared<dyn_unum_array>(this->dyn_unsigned_number_arrays[name]);
			this->types_table[name] = DYNAMIC_UNUM_ARRAY;
		}
		else if (type == DYNAMIC_SNUM_ARRAY || type == "dynamic signed number") {
			dyn_snum_array _array = dyn_snum_array(this->registers);
			this->dyn_signed_number_arrays[name] = _array;
			this->arrays_table[name] = std::make_shared<dyn_snum_array>(this->dyn_signed_number_arrays[name]);
			this->types_table[name] = DYNAMIC_SNUM_ARRAY;
		}
		else if (type == DYNAMIC_STR_ARRAY || type == "dynamic string") {
			dyn_str_array _array = dyn_str_array(this->registers);
			this->dyn_string_arrays[name] = _array;
			this->arrays_table[name] = std::make_shared<dyn_str_array>(this->dyn_string_arrays[name]);
			this->types_table[name] = DYNAMIC_STR_ARRAY;
		}
		else if (type == DYNAMIC_CHAR_ARRAY) {
			dyn_char_array _array = dyn_char_array(this->registers);
			this->dyn_char_arrays[name] = _array;
			this->arrays_table[name] = std::make_shared<dyn_char_array>(this->dyn_char_arrays[name]);
			this->types_table[name] = DYNAMIC_CHAR_ARRAY;
		}
		else if (type == DYNAMIC_DOUBLE_ARRAY) {
			dyn_double_array _array = dyn_double_array(this->registers);
			this->dyn_double_arrays[name] = _array;
			this->arrays_table[name] = std::make_shared<dyn_double_array>(this->dyn_double_arrays[name]);
			this->types_table[name] = DYNAMIC_DOUBLE_ARRAY;
		}
	}
}
void mem_arrays::getArray(std::string arr_name, unsigned long long index) {
	if (this->arrays_table.count(arr_name)) {
		if (types_table[arr_name] == STATIC_UNUM_ARRAY) {
			(std::static_pointer_cast<mem_array_int<unsigned long long>>(this->arrays_table[arr_name]))->getAt(index);
		}
		else if (types_table[arr_name] == STATIC_SNUM_ARRAY) {
			(std::static_pointer_cast<mem_array_int<long long>>(this->arrays_table[arr_name]))->getAt(index);
		}
		else if (types_table[arr_name] == STATIC_STR_ARRAY) {
			(std::static_pointer_cast<mem_array_int<std::string>>(this->arrays_table[arr_name]))->getAt(index);
		}
		else if (types_table[arr_name] == STATIC_CHAR_ARRAY) {
			(std::static_pointer_cast<mem_array_int<char>>(this->arrays_table[arr_name]))->getAt(index);
		}
		else if (types_table[arr_name] == STATIC_DOUBLE_ARRAY) {
			(std::static_pointer_cast<mem_array_int<double>>(this->arrays_table[arr_name]))->getAt(index);
		}
		else if (types_table[arr_name] == DYNAMIC_UNUM_ARRAY) {
			(std::static_pointer_cast<dyn_array_int<unsigned long long>>(this->arrays_table[arr_name])->getAt(index));
		}
		else if (types_table[arr_name] == DYNAMIC_SNUM_ARRAY) {
			(std::static_pointer_cast<dyn_array_int<long long>>(this->arrays_table[arr_name]))->getAt(index);
		}
		else if (types_table[arr_name] == DYNAMIC_STR_ARRAY) {
			(std::static_pointer_cast<dyn_array_int<std::string>>(this->arrays_table[arr_name]))->getAt(index);
		}
		else if (types_table[arr_name] == DYNAMIC_CHAR_ARRAY) {
			(std::static_pointer_cast<dyn_array_int<char>>(this->arrays_table[arr_name]))->getAt(index);
		}
		else if (types_table[arr_name] == DYNAMIC_DOUBLE_ARRAY) {
			(std::static_pointer_cast<dyn_array_int<double>>(this->arrays_table[arr_name]))->getAt(index);
		}
	}
}
void mem_arrays::setArray(std::string arr_name, unsigned long long index) {
	if (this->arrays_table.count(arr_name)) {
		if (types_table[arr_name] == STATIC_UNUM_ARRAY) {
			(std::static_pointer_cast<mem_array_int<unsigned long long>>(this->arrays_table[arr_name]))->setAt(index);
		}
		else if (types_table[arr_name] == STATIC_SNUM_ARRAY) {
			(std::static_pointer_cast<mem_array_int<long long>>(this->arrays_table[arr_name]))->setAt(index);
		}
		else if (types_table[arr_name] == STATIC_STR_ARRAY) {
			(std::static_pointer_cast<mem_array_int<std::string>>(this->arrays_table[arr_name]))->setAt(index);
		}
		else if (types_table[arr_name] == STATIC_CHAR_ARRAY) {
			(std::static_pointer_cast<mem_array_int<char>>(this->arrays_table[arr_name]))->setAt(index);
		}
		else if (types_table[arr_name] == STATIC_DOUBLE_ARRAY) {
			(std::static_pointer_cast<mem_array_int<double>>(this->arrays_table[arr_name]))->setAt(index);
		}
		else if (types_table[arr_name] == DYNAMIC_UNUM_ARRAY) {
			(std::static_pointer_cast<dyn_array_int<unsigned long long>>(this->arrays_table[arr_name]))->setAt(index);
		}
		else if (types_table[arr_name] == DYNAMIC_SNUM_ARRAY) {
			(std::static_pointer_cast<dyn_array_int<long long>>(this->arrays_table[arr_name]))->setAt(index);
		}
		else if (types_table[arr_name] == DYNAMIC_STR_ARRAY) {
			(std::static_pointer_cast<dyn_array_int<std::string>>(this->arrays_table[arr_name]))->setAt(index);
		}
		else if (types_table[arr_name] == DYNAMIC_CHAR_ARRAY) {
			(std::static_pointer_cast<dyn_array_int<char>>(this->arrays_table[arr_name]))->setAt(index);
		}
		else if (types_table[arr_name] == DYNAMIC_DOUBLE_ARRAY) {
			(std::static_pointer_cast<dyn_array_int<double>>(this->arrays_table[arr_name]))->setAt(index);
		}
	}
}
void mem_arrays::getDynSize(std::string arr_name) {
	if (this->arrays_table.count(arr_name)) {
		if (types_table[arr_name] == DYNAMIC_UNUM_ARRAY) {
			(std::static_pointer_cast<dyn_array_int<unsigned long long>>(this->arrays_table[arr_name]))->getSize();
		}
		else if (types_table[arr_name] == DYNAMIC_SNUM_ARRAY) {
			(std::static_pointer_cast<dyn_array_int<long long>>(this->arrays_table[arr_name]))->getSize();
		}
		else if (types_table[arr_name] == DYNAMIC_STR_ARRAY) {
			(std::static_pointer_cast<dyn_array_int<std::string>>(this->arrays_table[arr_name]))->getSize();
		}
		else if (types_table[arr_name] == DYNAMIC_CHAR_ARRAY) {
			(std::static_pointer_cast<dyn_array_int<char>>(this->arrays_table[arr_name]))->getSize();
		}
		else if(types_table[arr_name] == DYNAMIC_DOUBLE_ARRAY) {
			(std::static_pointer_cast<dyn_array_int<double>>(this->arrays_table[arr_name]))->getSize();
		}
	}
}

std::string mem_arrays::getArrayType(std::string arr_name) {
	if (this->types_table.count(arr_name)) {
		return this->types_table[arr_name];
	}
	else {
		return "UNDEFINED_ARRAY";
	}
}
void mem_arrays::destroy() {
	for (std::string _array : this->static_arrays) {
		if (this->types_table[_array] == STATIC_UNUM_ARRAY) {
			this->unsigned_number_arrays[_array].destroy();
		}
		else if (this->types_table[_array] == STATIC_SNUM_ARRAY) {
			this->signed_numbers_arrays[_array].destroy();
		}
		else if (this->types_table[_array] == STATIC_STR_ARRAY) {
			this->string_arrays[_array].destroy();
		}
		else if (this->types_table[_array] == STATIC_CHAR_ARRAY) {
			this->char_arrays[_array].destroy();
		}
		else if (this->types_table[_array] == STATIC_DOUBLE_ARRAY) {
			this->double_arrays[_array].destroy();
		}
	}
}