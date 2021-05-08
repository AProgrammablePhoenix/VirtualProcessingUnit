#pragma once

#include <iostream>
#include <string>
#include <memory>

#include "actions_symbols.h"

struct action {
public:
	action(virtual_actions _action_, std::shared_ptr<void> _value_ptr_) {
		this->_action = _action_;
		this->_value_ptr = _value_ptr_;
	}

	virtual_actions getAction() {
		return this->_action;
	}

	std::shared_ptr<void> getValuePtr() {
		return this->_value_ptr;
	}
private:
	virtual_actions _action;
	std::shared_ptr<void> _value_ptr;
};

action makeAction(virtual_actions _action, std::shared_ptr<void> value_ptr);