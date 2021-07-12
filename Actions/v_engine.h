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
	process(actions_engine* _engine);
	process(actions_engine* _engine, std::vector<action>* _actions);

	void addAction(action _action);
	void addAction(virtual_actions _action, std::shared_ptr<void> value_ptr);

	void addThread(std::vector<action>* _actions, size_t threadId);

	void start();
	void execute1();

	bool isTerminated();

	memory* getMemoryPtr();
	void destroy();
private:
	bool allThreadsTerminated();

	actions_engine engine;
	std::vector<action> actions;
	size_t *step;
	bool terminated;

	std::vector<actions_engine*> threads;
	std::vector<std::vector<action>> threadsActions;
	
	std::map<size_t, size_t> threadIdsMap;
	std::map<size_t, int>* threadsStatuses = new std::map<size_t, int>();

	std::vector<regs*> threadsRegisters;

	size_t nThreads = 0;
	size_t engineId = 0;
	bool threadsTerminated = false;

	void execute();
};