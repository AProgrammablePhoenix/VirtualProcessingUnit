#include <iostream>
#include <string>
#include <memory>

#include "v_engine.h"
#include "action_decl.h"
#include "actions_symbols.h"

actions_engine temp_engine = actions_engine();

process::process() {
	this->engine = temp_engine;
	this->actions = std::vector<action>();
	this->step = this->engine.getStepCounterPtr();
	this->terminated = false;
}
process::process(actions_engine* _engine) {
	this->engine = *_engine;
	this->actions = std::vector<action>();
	this->step = this->engine.getStepCounterPtr();
	this->terminated = false;
}
process::process(actions_engine* _engine, std::vector<action>* _actions) {
	this->engine = *_engine;
	this->actions = *_actions;
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

	if (*(this->step) < this->actions.size()) {
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

void process::destroy() {
	this->engine.destroy();
}