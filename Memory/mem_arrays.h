#include <iostream>
#include <string>
#include <vector>
#include <map>

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
private:
	regs* registers = NULL;
	bool initialized = false;
	T* container = NULL;
	unsigned long long container_size = 0;
	std::string values_type = "";
};
struct str_mem_array : mem_array_int<std::string> {
public:
	str_mem_array(regs* _registers, unsigned long long size);
	void getAt(unsigned long long index);
	void setAt(unsigned long long index);
private:
	regs* registers = NULL;
	bool initialized = false;
	std::string* container = NULL;
	unsigned long long container_size = 0;
	std::string values_type = "";
};
struct unum_mem_array : mem_array_int<unsigned long long> {
public:
	unum_mem_array(regs* _registers, unsigned long long size);
	void getAt(unsigned long long index);
	void setAt(unsigned long long index);
private:
	regs* registers = NULL;
	bool initialized = false;
	unsigned long long* container = NULL;
	unsigned long long container_size = 0;
	std::string values_type = "";
};

struct mem_arrays {
public:
	mem_arrays(regs* _registers);
	void makeArray(std::string name, std::string type, unsigned long long size);
	void getArray(std::string arr_name, unsigned long long index);
	void setArray(std::string arr_name, unsigned long long index);
private:
	regs* registers;

	std::map<std::string, str_mem_array> string_arrays;
	std::map<std::string, unum_mem_array> unsigned_number_arrays;
	std::map<std::string, void*> arrays_table;
	std::map<std::string, std::string> types_table;
};