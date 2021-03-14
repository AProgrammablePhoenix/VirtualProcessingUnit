#include <iostream>
#include <string>

#include "action_decl.h"
#include "actions_symbols.h"

void* a_db[0x004250 + 1];

action makeAction(virtual_actions _action, void* value_ptr) {
	return action(_action, value_ptr);
}