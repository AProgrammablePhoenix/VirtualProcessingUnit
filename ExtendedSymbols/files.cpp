#include <iostream>
#include <fstream>
#include <memory>
#include <string>

#include "../utility.h"
#include "../Registers/registers_symbols.h"
#include "../Memory/memory_symbols.h"
#include "files.h"

/* Registers statuses/values before call:
*	SR: path to file to open
*	RAX: number of bytes to read/write
*	RBX: Address of output/input byte array
*	RCX: Output for status: Successfull(0), File opening error(1), Error while reading/writing bytes(2)
*/
void ex_readFile(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	std::string filename = registers->sr->get();
	size_t nBytes = registers->rax->get();
	size_t addr = registers->rbx->get();

	std::ifstream file(filename);

	if (!nBytes) {
		registers->rcx->set(1);
		return;
	}

	unsigned char* read = new unsigned char[nBytes];

	if (file) {
		file.read((char*)read, nBytes);

		if (!file) {
			registers->rcx->set(2);
			delete[] read;
			return;
		}

		mem->_MS(read, nBytes, addr);
		delete[] read;
		registers->rcx->set(0);
	}
	else {
		registers->rcx->set(1);
		delete[] read;
		return;
	}
}
void ex_writeFile(std::shared_ptr<void> unused_p, regs* registers, memory* mem) {
	std::string filename = registers->sr->get();
	size_t nBytes = registers->rax->get();
	size_t addr = registers->rcx->get();

	std::ofstream file(filename);

	if (!nBytes) {
		registers->rcx->set(1);
		return;
	}

	unsigned char* bytes = new unsigned char[nBytes];
	mem->_MG(bytes, nBytes, addr);

	if (file) {
		file.write((const char*)bytes, nBytes);
		delete[] bytes;

		if (!file) {
			registers->rcx->set(2);
			return;
		}

		registers->rcx->set(0);
	}
	else {
		registers->rcx->set(1);
		delete[] bytes;
		return;
	}
}