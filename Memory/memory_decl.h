#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <any>

#pragma warning (push)

struct m_container {
public:
	void set(unsigned char data[]) {
		delete[] this->obj;
		obj = new unsigned char[sizeof(unsigned long long)];
		this->obj = data;
	}

	void set(unsigned char data[], unsigned long long length) {
		delete[] this->obj;
		obj = new unsigned char[length];
		this->obj = data;
	}

	unsigned char* get() {
		return this->obj;
	}
private:
	unsigned char *obj = new unsigned char[sizeof(unsigned long long)];
};

struct memory {
public:
	memory() {
		this->init();
	}

	void push(unsigned char data[]) {
		m_container buffer;
		buffer.set(data);
		this->_memory.push_back(buffer);
	}
	void push(unsigned char data[], unsigned long long length) {
		m_container buffer;
		buffer.set(data, length);
		this->_memory.push_back(buffer);
	}

	unsigned char *pop() {
		if (this->_memory.size() < 2) {
			return NULL;
		}
		else {
			m_container buffer = this->_memory[this->_memory.size() - 1];			
			this->_memory.pop_back();
			return buffer.get();
		}
	}

	unsigned char *access(unsigned long long addr) {
		if (this->_memory.size() < 2 || this->_memory.size() <= addr) {
			return NULL;
		}
		else {
			return this->_memory[addr].get();
		}
	}

	void set(unsigned char *data, unsigned long long addr) {
		if (addr >= this->_memory.size() || addr == 0) {
			return;
		}
		else {
			this->_memory[addr].set(data);
			return;
		}
	}
	void set(unsigned char* data, unsigned long long length, unsigned long long addr) {
		if (addr >= this->_memory.size() || addr == 0) {
			return;
		}
		else {
			this->_memory[addr].set(data, length);
		}
	}
private:
	std::vector<m_container> _memory;

	void init() {
		m_container start_block;
		start_block.set((unsigned char*)"NULL_AREA[MEMORY_START]");
		_memory.push_back(start_block);
	}
};

#pragma warning (pop)