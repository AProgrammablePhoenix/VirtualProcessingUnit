#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "v_engine.h"

struct engine {
public:
	engine();
	engine(const process& proc);
	engine(std::vector<process> procs);

	size_t pushProcess(const process& proc);
	void popProcess();

	int engine::getProcessById(size_t id, process* out_proc);
	void deleteProcessById(size_t id);
	
	void setThreading(bool state);
	void start();

	void destroy();
private:
	std::vector<process> processes;
	bool threading;
};