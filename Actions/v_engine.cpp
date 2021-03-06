#include <iostream>
#include <memory>
#include <string>

#include "action_decl.h"
#include "actions_symbols.h"
#include "v_engine.h"

#define ENGMEM this->getMemoryPtr()
#define ENGRGS this->engine.getRegsPtr()

#define SetMainStack() \
	*ENGRGS->rbp = *ENGRGS->rsp = ENGMEM->getMemLen() - 1; \
	*ENGRGS->rsend = ENGMEM->getMemLen() - 1 - ENGMEM->stacksize; \
	*ENGRGS->rsbgn = *ENGRGS->rbp + 1

actions_engine temp_engine = actions_engine();

process::process() {
	this->engine = temp_engine;

	SetMainStack();

	this->actions = std::vector<action>();
	this->step = this->engine.getStepCounterPtr();
	this->terminated = false;
}
process::process(const actions_engine& _engine) {
	this->engine = _engine;

	SetMainStack();

	this->actions = std::vector<action>();
	this->step = this->engine.getStepCounterPtr();
	this->terminated = false;
}
process::process(const actions_engine& _engine, const std::vector<action>& _actions) {
	this->engine = _engine;

	SetMainStack();

	this->actions = _actions;
	this->step = this->engine.getStepCounterPtr();
	this->terminated = false;
}

void process::addAction(action _action) {
	this->actions.push_back(_action);
}
void process::addAction(virtual_actions _action, std::shared_ptr<void> value_ptr) {
	this->actions.push_back(action(_action, value_ptr));
}

void process::execute() {
	while (*(this->step) < this->actions.size() && !this->engine.toStop()) {
		this->engine.execute(this->actions[*(this->step)].getAction(), this->actions[*(this->step)].getValuePtr());
		(*(this->step))++;
	}
	terminated = true;
}
void process::start() {
	this->execute();
}
void process::execute1() {
	if (this->engine.toStop()) {
		this->terminated = true;
		return;
	}
	else if (*(this->step) < this->actions.size()) {
		this->engine.execute(this->actions[*(this->step)].getAction(), this->actions[*(this->step)].getValuePtr());
		(*(this->step))++;
	}
	else {
		this->terminated = true;
	}
}

bool process::isTerminated() {
	return this->terminated;
}

memory* process::getMemoryPtr() {
	return this->engine.getMemoryPtr();
}
void process::destroy() {
	this->engine.destroy();
}