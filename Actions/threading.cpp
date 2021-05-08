#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "v_engine.h"
#include "threading.h"

engine::engine() {
	this->threading = true;
}
engine::engine(process *proc) {
	this->processes.push_back(*proc);
	this->threading = true;
}
engine::engine(std::vector<process> procs) {
	for (unsigned long long i = 0; i < procs.size(); i++) {
		this->processes.push_back(procs[i]);
	}
	this->threading = true;
}

unsigned long long engine::pushProcess(process* proc) {
	this->processes.push_back(*proc);
	return this->processes.size() - 1;
}
void engine::popProcess() {
	this->processes.pop_back();
}

process engine::getProcessById(unsigned long long id) {
	if (id >= this->processes.size()) {
		return NULL;
	}
	else {
		return this->processes[id];
	}
}
void engine::deleteProcessById(unsigned long long id) {
	if (id >= this->processes.size()) {
		return;
	}
	else {
		this->processes.erase(this->processes.begin() + id);
		return;
	}
}

void engine::setThreading(bool state) {
	this->threading = state;
}
void engine::start() {
	if (threading) {
		bool allTerminated = false;

		while (!allTerminated) {
			if (this->processes.empty() || this->processes.size() < 1) {
				return;
			}

			for (int i = 0; i < this->processes.size(); i++) {
				this->processes[i].execute1();
				if (this->processes[i].isTerminated()) {
					this->processes.erase(this->processes.begin() + i);
					continue;
				}
				allTerminated = this->processes[i].isTerminated();
			}
		}
	}
	else {
		for (unsigned long long i = 0; i < this->processes.size(); i++) {
			this->processes[i].start();
		}
	}
}

void engine::destroy() {
	for (unsigned long long i = 0; i < this->processes.size(); i++) {
		this->processes[i].destroy();
	}
}