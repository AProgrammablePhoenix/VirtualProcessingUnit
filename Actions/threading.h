#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "v_engine.h"

struct engine {
public:
	engine();
	engine(process *proc);
	engine(std::vector<process> procs);

	unsigned long long pushProcess(process* proc);
	void popProcess();

	process getProcessById(unsigned long long id);
	void deleteProcessById(unsigned long long id);
	
	void setThreading(bool state);
	void start();

	void destroy();
private:
	std::vector<process> processes;
	bool threading;
};