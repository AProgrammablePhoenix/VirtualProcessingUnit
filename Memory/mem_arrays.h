#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>

#include "../Registers/regs_decl.h"
#include "memory_decl.h"

template<typename T>
struct mem_array_int {
public:
	virtual void getAt(unsigned long long index) {
		return;
	}
	virtual void setAt(unsigned long long index) {
		return;
	}
	virtual void destroy() {
		if (this->container != NULL) {
			delete[] this->container;
		}
	}
private:
	regs* registers = NULL;
	bool initialized = false;
	T* container = NULL;
	unsigned long long container_size = 0;
	std::string values_type = "";
};
struct str_mem_array : mem_array_int<std::string> {
public:
	str_mem_array();
	str_mem_array(regs* _registers, unsigned long long size);
	void getAt(unsigned long long index);
	void setAt(unsigned long long index);
	void destroy();
private:
	regs* registers = NULL;
	bool initialized = false;
	std::string* container = NULL;
	unsigned long long container_size = 0;
	std::string values_type = "";
};
struct unum_mem_array : mem_array_int<unsigned long long> {
public:
	unum_mem_array();
	unum_mem_array(regs* _registers, unsigned long long size);
	void getAt(unsigned long long index);
	void setAt(unsigned long long index);
	void destroy();
private:
	regs* registers = NULL;
	bool initialized = false;
	unsigned long long* container = NULL;
	unsigned long long container_size = 0;
	std::string values_type = "";
};
struct snum_mem_array : mem_array_int<long long> {
public:
	snum_mem_array();
	snum_mem_array(regs* _registers, unsigned long long size);
	void getAt(unsigned long long index);
	void setAt(unsigned long long index);
	void destroy();
private:
	regs* registers = NULL;
	bool initialized = false;
	long long* container = NULL;
	unsigned long long container_size = 0;
	std::string values_type = "";
};

template<typename T>
struct dyn_array_int {
public:
	virtual void getAt(unsigned long long index) {
		return;
	}
	virtual void setAt(unsigned long long index) {
		return;
	}
	virtual void getSize() {
		return;
	}
private:
	regs* registers = NULL;
	bool initialized = false;
	std::vector<T> container;
	std::string values_type = "";
};
struct dyn_str_array : dyn_array_int<std::string> {
public:
	dyn_str_array();
	dyn_str_array(regs* _registers);
	void getAt(unsigned long long index);
	void setAt(unsigned long long index);
	void getSize();
private:
	regs* registers = NULL;
	bool initialized = false;
	std::vector<std::string> container;
	std::string values_type = "";
};
struct dyn_unum_array : dyn_array_int<unsigned long long> {
public:
	dyn_unum_array();
	dyn_unum_array(regs* _registers);
	void getAt(unsigned long long index);
	void setAt(unsigned long long index);
	void getSize();
private:
	regs* registers = NULL;
	bool initialized = false;
	std::vector<unsigned long long> container;
	std::string values_types = "";
};
struct dyn_snum_array : dyn_array_int<long long> {
public:
	dyn_snum_array();
	dyn_snum_array(regs* _registers);
	void getAt(unsigned long long index);
	void setAt(unsigned long long index);
	void getSize();
private:
	regs* registers = NULL;
	bool initialized = false;
	std::vector<long long> container;
	std::string values_types = "";
};

struct mem_arrays {
public:
	mem_arrays();
	mem_arrays(regs* _registers);
	void makeArray(std::string name, std::string type, unsigned long long size);
	void getArray(std::string arr_name, unsigned long long index);
	void setArray(std::string arr_name, unsigned long long index);
	void getDynSize(std::string arr_name);

	std::string getArrayType(std::string arr_name);

	void destroy();
private:
	regs* registers;

	std::map<std::string, str_mem_array> string_arrays;
	std::map<std::string, unum_mem_array> unsigned_number_arrays;
	std::map<std::string, snum_mem_array> signed_numbers_arrays;

	std::map<std::string, dyn_str_array> dyn_string_arrays;
	std::map<std::string, dyn_unum_array> dyn_unsigned_number_arrays;
	std::map<std::string, dyn_snum_array> dyn_signed_number_arrays;

	std::map<std::string, std::shared_ptr<void>> arrays_table;
	std::map<std::string, std::string> types_table;

	std::vector<std::string> static_arrays;
};