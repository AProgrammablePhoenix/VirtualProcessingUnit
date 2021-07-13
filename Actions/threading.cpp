#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "threading.h"
#include "v_engine.h"

engine::engine() {
	this->threading = true;
}
engine::engine(const process& proc) {
	this->processes.push_back(proc);
	this->threading = true;
}
engine::engine(std::vector<process> procs) {
	for (size_t i = 0; i < procs.size(); i++) {
		this->processes.push_back(procs[i]);
	}
	this->threading = true;
}

size_t engine::pushProcess(const process& proc) {
	this->processes.push_back(proc);
	return this->processes.size() - 1;
}
void engine::popProcess() {
	this->processes.pop_back();
}

int engine::getProcessById(size_t id, process* out_proc) {
	if (id >= this->processes.size()) {
		out_proc = NULL;
		return 1;
	}
	else {
		*out_proc = this->processes[id];
		return 0;
	}
}
void engine::deleteProcessById(size_t id) {
	if (id < this->processes.size()) {
		this->processes.erase(this->processes.begin() + id);
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
		for (size_t i = 0; i < this->processes.size(); i++) {
			this->processes[i].start();
		}
	}
}

void engine::destroy() {
	for (size_t i = 0; i < this->processes.size(); i++) {
		this->processes[i].destroy();
	}
}