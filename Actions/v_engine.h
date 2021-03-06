#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "action_decl.h"
#include "actions_symbols.h"

struct process {
public:
	process();
	process(const actions_engine& _engine);
	process(const actions_engine& _engine, const std::vector<action>& _actions);

	void addAction(action _action);
	void addAction(virtual_actions _action, std::shared_ptr<void> value_ptr);

	void start();
	void execute1();

	bool isTerminated();

	memory* getMemoryPtr();
	void destroy();
private:
	actions_engine engine;
	std::vector<action> actions;
	size_t *step;
	bool terminated;

	void execute();
};