#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "action_decl.h"
#include "actions_symbols.h"

struct process {
public:
	process();
	process(actions_engine* _engine);
	process(actions_engine* _engine, std::vector<action>* _actions);

	void addAction(action _action);
	void addAction(virtual_actions _action, void* value_ptr);

	void start();
	void execute1();

	bool isTerminated();

	void destroy();
private:
	actions_engine engine;
	std::vector<action> actions;
	unsigned long long *step;
	bool terminated;

	void execute();
};