#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "../Registers/regs_decl.h"
#include "memory_decl.h"
#include "mem_structs.h"
#include "memory_symbols.h"

#define CHAR_PROPERTY "char"
#define DOUBLE_PROPERTY "double"
#define SNUM_PROPERTY "__int64"
#define STR_PROPERTY "class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >"
#define UNUM_PROPERTY "unsigned __int64"

__struct__::__struct__() {
	if (!this->initialized) {
		if (!this->types_table.empty()) {
			this->types_table.clear();
			this->string_properties.clear();
			this->unum_properties.clear();
			this->snum_properties.clear();
		}
	}
}
__struct__::__struct__(regs* _registers, memory* _mem) {
	if (!this->initialized) {
		if (!this->types_table.empty()) {
			this->types_table.clear();
			this->string_properties.clear();
			this->unum_properties.clear();
			this->snum_properties.clear();
		}

		this->registers = _registers;
		this->mem = _mem;
		this->initialized = true;
	}
}
void __struct__::declProperty(std::string property_name) {
	if (this->initialized) {
		if (!this->types_table.count(property_name)) {
			std::string saved_sr = this->registers->sr->get();

			popMemSR(NULL, this->registers, this->mem);
			std::string property_type = this->registers->sr->get();

			this->registers->sr->set(saved_sr);

			if (property_type == STR_PROPERTY || property_type == "string") {
				this->types_table[property_name] = STR_PROPERTY;
				this->string_properties[property_name] = "";
			}
			else if (property_type == CHAR_PROPERTY) {
				this->types_table[property_name] = CHAR_PROPERTY;
				this->char_properties[property_name] = '\0';
			}
			else if (property_type == UNUM_PROPERTY || property_type == "unsigned number") {
				this->types_table[property_name] = UNUM_PROPERTY;
				this->unum_properties[property_name] = 0;
			}
			else if (property_type == SNUM_PROPERTY || property_type == "signed number") {
				this->types_table[property_name] = SNUM_PROPERTY;
				this->snum_properties[property_name] = 0;
			}
			else if (property_type == DOUBLE_PROPERTY) {
				this->types_table[property_name] = DOUBLE_PROPERTY;
				this->double_properties[property_name] = 0;
			}
		}
	}
}
void __struct__::get(std::string property_name) {
	if (this->initialized) {
		if (this->types_table.count(property_name)) {
			if (this->types_table[property_name] == STR_PROPERTY) {
				std::string saved_sr = this->registers->sr->get();

				this->registers->sr->set(this->string_properties[property_name]);
				pushMemSR(nullptr, this->registers, this->mem);

				this->registers->sr->set(saved_sr);
			}
			else if (this->types_table[property_name] == CHAR_PROPERTY) {
				char saved_cr = this->registers->cr->get();

				this->registers->cr->set(this->char_properties[property_name]);
				pushMemCR(nullptr, this->registers, this->mem);

				this->registers->cr->set(saved_cr);
			}
			else if (this->types_table[property_name] == UNUM_PROPERTY) {
				size_t value = this->unum_properties[property_name];
				unsigned char* temp = new unsigned char[sizeof(size_t)];
				mp_memcpy(&value, temp);

				mem->push(temp, sizeof(size_t));
				delete[] temp;
			}
			else if (this->types_table[property_name] == SNUM_PROPERTY) {
				size_t value = (size_t)(this->snum_properties[property_name]);
				unsigned char* temp = new unsigned char[sizeof(size_t)];
				mp_memcpy(&value, temp);

				mem->push(temp, sizeof(size_t));
				delete[] temp;
			}
			else {
				double saved_dr = this->registers->dr->get();

				this->registers->dr->set(this->double_properties[property_name]);
				pushMemDR(nullptr, this->registers, this->mem);

				this->registers->dr->set(saved_dr);
			}
		}
	}
}
void __struct__::set(std::string property_name) {
	if (this->initialized) {
		if (this->types_table.count(property_name)) {
			if (this->types_table[property_name] == STR_PROPERTY) {
				std::string saved_sr = this->registers->sr->get();

				popMemSR(nullptr, this->registers, this->mem);
				this->string_properties[property_name] = this->registers->sr->get();

				this->registers->sr->set(saved_sr);
			}
			else if (this->types_table[property_name] == CHAR_PROPERTY) {
				char saved_cr = this->registers->cr->get();

				popMemCR(nullptr, this->registers, this->mem);
				this->char_properties[property_name] = this->registers->cr->get();

				this->registers->cr->set(saved_cr);
			}
			else if (this->types_table[property_name] == UNUM_PROPERTY) {
				unsigned char* temp = new unsigned char[sizeof(size_t)];
				mem->pop(temp, sizeof(size_t));
				size_t value = 0;

				mp_memcpy(temp, &value);
				delete[] temp;

				this->unum_properties[property_name] = value;
			}
			else if (this->types_table[property_name] == SNUM_PROPERTY) {
				unsigned char* temp = new unsigned char[sizeof(size_t)];
				mem->pop(temp, sizeof(size_t));
				size_t value = 0;

				mp_memcpy(temp, &value);
				delete[] temp;

				this->snum_properties[property_name] = (long long)value;
			}
			else {
				double saved_dr = this->registers->dr->get();

				popMemDR(nullptr, this->registers, this->mem);
				this->double_properties[property_name] = this->registers->dr->get();

				this->registers->dr->set(saved_dr);
			}
		}
	}
}

mem_structs::mem_structs() {
	this->registers = nullptr;
	this->mem = nullptr;
}
mem_structs::mem_structs(regs* _registers, memory* _mem) {
	this->registers = _registers;
	this->mem = _mem;
}
void mem_structs::makeStruct(std::string structure_name) {
	if (structure_name == "<unexisting_structure>") {
		return;
	}
	else if (!this->structs_container.count(structure_name)) {
		this->structs_container[structure_name] = __struct__(this->registers, this->mem);
	}
}
void mem_structs::selectStruct(std::string structure_name) {
	if (this->structs_container.count(structure_name)) {
		this->registers->structPtr->set(structure_name);
	}
	else {
		this->registers->structPtr->set("<unexisting_structure>");
	}
}
void mem_structs::declStructProperty(std::string property_name) {
	std::string selected = this->registers->structPtr->get();

	if (selected != "<unexisting_structure>" && this->structs_container.count(selected)) {
		this->structs_container[selected].declProperty(property_name);
	}
}
void mem_structs::getStructProperty(std::string property_name) {
	std::string selected = this->registers->structPtr->get();

	if (selected != "<unexisting_structure>" && this->structs_container.count(selected)) {
		this->structs_container[selected].get(property_name);
	}
}
void mem_structs::setStructProperty(std::string property_name) {
	std::string selected = this->registers->structPtr->get();

	if (selected != "<unexisting_structure>" && this->structs_container.count(selected)) {
		this->structs_container[selected].set(property_name);
	}
}