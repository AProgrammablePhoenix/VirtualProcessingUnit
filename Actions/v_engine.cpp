#include <iostream>
#include <memory>
#include <string>

#include "action_decl.h"
#include "actions_symbols.h"
#include "v_engine.h"

actions_engine temp_engine = actions_engine();

bool process::allThreadsTerminated() {
	if (nThreads) {
		for (size_t i = 0; i < this->threads.size(); i++) {
			if ((*this->threadsStatuses)[this->threadIdsMap[i]] > THREAD_STOPPED)
				return false;
		}
		return true;
	}
	return true;
}

process::process() {
	this->engine = temp_engine;
	this->engine.setThreadsMap(this->threadsStatuses);

	this->actions = std::vector<action>();
	this->step = this->engine.getStepCounterPtr();
	this->terminated = false;
}
process::process(actions_engine* _engine) {
	this->engine = *_engine;
	this->engine.setThreadsMap(this->threadsStatuses);

	this->actions = std::vector<action>();
	this->step = this->engine.getStepCounterPtr();
	this->terminated = false;
}
process::process(actions_engine* _engine, std::vector<action>* _actions) {
	this->engine = *_engine;
	this->engine.setThreadsMap(this->threadsStatuses);

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

void process::addThread(std::vector<action>* _actions, size_t threadId) {
	this->nThreads++;
	this->threadsActions.push_back(*_actions);

	regs* tempR = new regs;
	tempR->threadsStatuses = this->threadsStatuses;

	memory* tempM = this->getMemoryPtr();

	this->threadsRegisters.push_back(tempR);
	this->threads.push_back(new actions_engine(tempM, tempR));
	(*this->threadsStatuses)[threadId] = false;
	this->threadIdsMap[this->threads.size() - 1] = threadId;
}

void process::execute() {
	if (!nThreads) {
		while (*(this->step) < this->actions.size() && !this->engine.toStop()) {
			this->engine.execute(this->actions[*(this->step)].getAction(), this->actions[*(this->step)].getValuePtr());
			(*(this->step))++;
		}
		terminated = true;
	}
	else {
		while (!this->isTerminated()) {
			this->execute1();
		}
	}
}
void process::start() {
	this->execute();
}
void process::execute1() {
	if (!this->nThreads) {
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
	else {
		if (!this->engineId) {
			if (this->engine.toStop()) {
				this->terminated = true;
				++(this->engineId);
				return;
			}

			if (*(this->step) < this->actions.size()) {
				this->engine.execute(this->actions[*(this->step)].getAction(), this->actions[*(this->step)].getValuePtr());
				(*(this->step))++;
			}
			else {
				this->terminated = true;
			}
			++(this->engineId);
		}
		else {
			size_t i = engineId - 1;
			if ((*this->threadsStatuses)[this->threadIdsMap[i]] <= THREAD_STOPPED) {
				++(this->engineId);
				if (this->engineId - 1 == this->threads.size())
					this->engineId = 0;
				return;
			}

			if (this->threads[i]->toStop() || (*this->threadsStatuses)[this->threadIdsMap[i]] == THREAD_SIGTERM) {
				*this->threads[i]->getStepCounterPtr() = this->threadsActions[i].size();
				(*this->threadsStatuses)[this->threadIdsMap[i]] = THREAD_DEAD;

				++(this->engineId);
				if (this->engineId - 1 == this->threads.size())
					this->engineId = 0;
				return;
			}

			size_t step = *this->threads[i]->getStepCounterPtr();
			if (step < this->threadsActions[i].size()) {
				this->threads[i]->execute(this->threadsActions[i][step].getAction(), this->threadsActions[i][step].getValuePtr());
				(*this->threads[i]->getStepCounterPtr())++;
			}

			++(this->engineId);
			if (this->engineId - 1 == this->threads.size())
				this->engineId = 0;
			return;
		}
	}
}

bool process::isTerminated() {
	if (!nThreads)
		return this->terminated;
	else
		return this->terminated && this->allThreadsTerminated();
}

memory* process::getMemoryPtr() {
	return this->engine.getMemoryPtr();
}
void process::destroy() {
	this->engine.destroy();

	for (size_t i = 0; i < this->threadsRegisters.size(); i++)
		delete this->threadsRegisters[i];
	for (size_t i = 0; i < this->threads.size(); i++) {
		this->threads[i]->destroy();
		delete this->threads[i];
	}

	delete this->threadsStatuses;
}