#include <iostream>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>

#include "../utility.h"
#include "../Registers/registers_symbols.h"
#include "../Memory/memory_symbols.h"
#include "files.h"



void ex_rfread(std::shared_ptr<void> args_p, regs* registers, memory* mem) {
	const auto [vaddr, vsize, vopt] = *std::static_pointer_cast<arg_tuple>(args_p);

	std::string filename = registers->sr->get();
	size_t nBytes, oaddr, file_offset;
	
	auto temp = std::make_unique<unsigned char[]>(sizeof(size_t));
	mem->_MG(temp.get(), sizeof(size_t), vaddr);
	nBytes = ATOULL(temp.get());

	if (!nBytes) {
		registers->rax->set(1);
		return;
	}

	mem->pop(temp.get(), sizeof(size_t));
	oaddr = ATOULL(temp.get());

	mem->pop(temp.get(), sizeof(size_t));
	file_offset = ATOULL(temp.get());

	std::ifstream file(filename);

	auto read = std::make_unique<unsigned char[]>(nBytes);

	if (file) {
		if (std::filesystem::file_size(filename) <= file_offset) {
			registers->rax->set(2);
			file.close();
			return;
		}

		file.seekg(file_offset);
		file.read((char*)read.get(), nBytes);

		if (!file) {
			registers->rax->set(3);
			file.close();
			return;
		}

		mem->_MS(read.get(), nBytes, oaddr);
		registers->rax->set(0);
	}
	else {
		registers->rax->set(4);
		return;
	}
}
void ex_rfwrite(std::shared_ptr<void> args_p, regs* registers, memory* mem) {
	const auto [vaddr, vsize, vopt] = *std::static_pointer_cast<arg_tuple>(args_p);

	std::string filename = registers->sr->get();
	size_t nBytes, iaddr, file_offset;

	auto temp = std::make_unique<unsigned char[]>(sizeof(size_t));
	mem->_MG(temp.get(), sizeof(size_t), vaddr);
	nBytes = ATOULL(temp.get());

	if (!nBytes) {
		registers->rcx->set(1);
		return;
	}

	mem->pop(temp.get(), sizeof(size_t));
	iaddr = ATOULL(temp.get());

	mem->pop(temp.get(), sizeof(size_t));
	file_offset = ATOULL(temp.get());

	std::ofstream file(filename);

	auto bytes = std::make_unique<unsigned char[]>(nBytes);
	mem->_MG(bytes.get(), nBytes, iaddr);

	if (file) {
		if (std::filesystem::file_size(filename) <= file_offset) {
			file.close();
			file.clear();
			file.open(filename, std::ios::app);
		}
		else {
			file.seekp(file_offset);
		}

		file.write((const char*)bytes.get(), nBytes);

		if (!file) {
			registers->rcx->set(3);
			return;
		}

		registers->rcx->set(0);
	}
	else {
		registers->rcx->set(4);
		return;
	}
}

void ex_rflen(std::shared_ptr<void> reg, regs* registers, memory* mem) {
	void* ptr = registries_ptr_table(registers).access(ATTOREGID(reg, mem));

	std::error_code ec;
	((reg_int<size_t>*)ptr)->set(std::filesystem::file_size(registers->sr->get(), ec));

	if (ec)
		registers->rax->set(1);
	else
		registers->rax->set(0);
}