#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "v_engine.h"

struct engine {
public:
	engine();
	engine(process *proc);
	engine(std::vector<process> procs);

	size_t pushProcess(process* proc);
	void popProcess();

	process getProcessById(size_t id);
	void deleteProcessById(size_t id);
	
	void setThreading(bool state);
	void start();

	void destroy();
private:
	std::vector<process> processes;
	bool threading;
};