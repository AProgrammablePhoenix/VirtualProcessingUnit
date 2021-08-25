#include <iostream>
#include <memory>
#include <string>

#include "action_decl.h"
#include "actions_symbols.h"

void (*a_db[(size_t)virtual_actions::rflen + 1])(std::shared_ptr<void>, regs*, memory*);

action makeAction(virtual_actions _action, std::shared_ptr<void> value_ptr) {
	return action(_action, value_ptr);
}