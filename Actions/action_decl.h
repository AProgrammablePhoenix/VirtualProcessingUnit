#pragma once

#include <iostream>
#include <string>

#include "actions_symbols.h"

struct action {
public:
	action(virtual_actions _action_, void* _value_ptr_) {
		this->_action = _action_;
		this->_value_ptr = _value_ptr_;
	}

	virtual_actions getAction() {
		return this->_action;
	}

	void* getValuePtr() {
		return this->_value_ptr;
	}
private:
	virtual_actions _action;
	void* _value_ptr;
};

action makeAction(virtual_actions _action, void* value_ptr);