#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <stack>

#include "../../Actions/action_decl.h"
#include "../../Actions/actions_symbols.h"
#include "../../utility.h"
#include "compiler.h"
#include "parser.h"

static const std::unordered_set<std::string> num_registers = {
	"ax",
	"bx",
	"cx",
	"dx",
	"eax",
	"ebx",
	"ecx",
	"edx",
	"rax",
	"rbx",
	"rcx",
	"rdx"
};
static const std::unordered_map<std::string, virtual_actions> fetch_set_input = {
	{"ax", virtual_actions::setAX},
	{"bx", virtual_actions::setBX},
	{"cx", virtual_actions::setCX},
	{"dx", virtual_actions::setDX},

	{"eax", virtual_actions::setEAX},
	{"ebx", virtual_actions::setEBX},
	{"ecx", virtual_actions::setECX},
	{"edx", virtual_actions::setEDX},

	{"rax", virtual_actions::setRAX},
	{"rbx", virtual_actions::setRBX},
	{"rcx", virtual_actions::setRCX},
	{"rdx", virtual_actions::setRDX},

	{"sr", virtual_actions::setSR},
	{"cr", virtual_actions::setCR},
	{"dr", virtual_actions::setDR}
};
static const std::unordered_map<std::string, virtual_actions> fetch_mov_input = {
	{"ax", virtual_actions::movAX},
	{"bx", virtual_actions::movBX},
	{"cx", virtual_actions::movCX},
	{"dx", virtual_actions::movDX},

	{"eax", virtual_actions::movEAX},
	{"ebx", virtual_actions::movEBX},
	{"ecx", virtual_actions::movECX},
	{"edx", virtual_actions::movEDX},

	{"rax", virtual_actions::movRAX},
	{"rbx", virtual_actions::movRBX},
	{"rcx", virtual_actions::movRCX},
	{"rdx", virtual_actions::movRDX}
};
static const std::unordered_map<std::string, virtual_actions> fetch_inc_input = {
	{"ax", virtual_actions::inc},
	{"bx", virtual_actions::inc},
	{"cx", virtual_actions::inc},
	{"dx", virtual_actions::inc},
	{"eax", virtual_actions::inc},
	{"ebx", virtual_actions::inc},
	{"ecx", virtual_actions::inc},
	{"edx", virtual_actions::inc},
	{"rax", virtual_actions::inc},
	{"rbx", virtual_actions::inc},
	{"rcx", virtual_actions::inc},
	{"rdx", virtual_actions::inc},

	{"dr", virtual_actions::incDR}
};
static const std::unordered_map<std::string, virtual_actions> fetch_mul_input = {
	{"ax", virtual_actions::mulAX},
	{"bx", virtual_actions::mulBX},
	{"cx", virtual_actions::mulCX},
	{"dx", virtual_actions::mulDX},

	{"eax", virtual_actions::mulEAX},
	{"ebx", virtual_actions::mulEBX},
	{"ecx", virtual_actions::mulECX},
	{"edx", virtual_actions::mulEDX},

	{"rax", virtual_actions::mulRAX},
	{"rbx", virtual_actions::mulRBX},
	{"rcx", virtual_actions::mulRCX},
	{"rdx", virtual_actions::mulRDX}
};
static const std::unordered_map<std::string, virtual_actions> fetch_div_input = {
	{"ax", virtual_actions::divAX},
	{"bx", virtual_actions::divBX},
	{"cx", virtual_actions::divCX},
	{"dx", virtual_actions::divDX},

	{"eax", virtual_actions::divEAX},
	{"ebx", virtual_actions::divEBX},
	{"ecx", virtual_actions::divECX},
	{"edx", virtual_actions::divEDX},

	{"rax", virtual_actions::divRAX},
	{"rbx", virtual_actions::divRBX},
	{"rcx", virtual_actions::divRCX},
	{"rdx", virtual_actions::divRDX}
};
static const std::unordered_map<std::string, virtual_actions> fetch_add_input = {
	{"ax", virtual_actions::addAX},
	{"bx", virtual_actions::addBX},
	{"cx", virtual_actions::addCX},
	{"dx", virtual_actions::addDX},

	{"eax", virtual_actions::addEAX},
	{"ebx", virtual_actions::addEBX},
	{"ecx", virtual_actions::addECX},
	{"edx", virtual_actions::addEDX},

	{"rax", virtual_actions::addRAX},
	{"rbx", virtual_actions::addRBX},
	{"rcx", virtual_actions::addRCX},
	{"rdx", virtual_actions::addRDX}
};
static const std::unordered_map<std::string, virtual_actions> fetch_sub_input = {
	{"ax", virtual_actions::subAX},
	{"bx", virtual_actions::subBX},
	{"cx", virtual_actions::subCX},
	{"dx", virtual_actions::subDX},

	{"eax", virtual_actions::subEAX},
	{"ebx", virtual_actions::subEBX},
	{"ecx", virtual_actions::subECX},
	{"edx", virtual_actions::subEDX},

	{"rax", virtual_actions::subRAX},
	{"rbx", virtual_actions::subRBX},
	{"rcx", virtual_actions::subRCX},
	{"rdx", virtual_actions::subRDX}
};

static bool unsafe_flag = false;
struct {
	bool mem_res_req = false;
	size_t new_mem_size = 0;
} memory_flags;

namespace {
	inline void pushAction(std::vector<action>& out_actions, const virtual_actions& vaction,
			const tokenTypes& var_type, const std::string& var_raw) {
		out_actions.emplace_back(
			action(vaction,
				std::make_shared<std::tuple<tokenTypes, std::string>>(
					std::make_tuple<const tokenTypes&, const std::string&>(var_type, var_raw)
				)
			)
		);
	}
	inline void preprocJmpCmpCalls(std::vector<action>& out_actions, const virtual_actions& vaction,
			const token& arg) {
		if (arg.type != tokenTypes::label) {
			pushAction(out_actions, vaction, arg.type, arg.element);
		}
		else {
			pushAction(out_actions, vaction, tokenTypes::unsigned_n, "1");
			pushAction(out_actions, virtual_actions::jmp, tokenTypes::unsigned_n, "1");
			pushAction(out_actions, virtual_actions::lcall, tokenTypes::label, arg.element);
		}
	}

	inline void CRToNum(std::vector<action>& out_actions, const bool& useRAX) {
		if (useRAX) {
			// Set first 8 bytes of memory to 0
			pushAction(out_actions, virtual_actions::setRAX, tokenTypes::unsigned_n, "0");
			pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");
			pushAction(out_actions, virtual_actions::movsm, tokenTypes::reg, "rax");
			// Set value of CR (1 byte) into 7th of memory
			pushAction(out_actions, virtual_actions::setRAX, tokenTypes::unsigned_n, "7");
			pushAction(out_actions, virtual_actions::movsmCR, tokenTypes::reg, "rax");
			// Fetch first 8 bytes of memory and leave them on stack
			pushAction(out_actions, virtual_actions::setRAX, tokenTypes::unsigned_n, "0");
			pushAction(out_actions, virtual_actions::movgm, tokenTypes::reg, "rax");
		}
		else { // Use rbx instead
			// Set first 8 bytes of memory to 0
			pushAction(out_actions, virtual_actions::setRBX, tokenTypes::unsigned_n, "0");
			pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");
			pushAction(out_actions, virtual_actions::movsm, tokenTypes::reg, "rbx");
			// Set value of CR (1 byte) into 7th of memory
			pushAction(out_actions, virtual_actions::setRBX, tokenTypes::unsigned_n, "7");
			pushAction(out_actions, virtual_actions::movsmCR, tokenTypes::reg, "rbx");
			// Fetch first 8 bytes of memory and leave them on stack
			pushAction(out_actions, virtual_actions::setRBX, tokenTypes::unsigned_n, "0");
			pushAction(out_actions, virtual_actions::movgm, tokenTypes::reg, "rbx");
		}
	}
	inline void getFirstCharSR(std::vector<action>& out_actions) {
		// Get first char of SR
		pushAction(out_actions, virtual_actions::setRAX, tokenTypes::unsigned_n, "2");
		// Prepare recast arguments
		pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");
		pushAction(out_actions, virtual_actions::pushSR, tokenTypes::reg, "");
		// Invoke recast inerrupt
		pushAction(out_actions, virtual_actions::_int, tokenTypes::unsigned_n, "9");
		// Pop first char of SR from stack
		pushAction(out_actions, virtual_actions::popCR, tokenTypes::reg, "");
		CRToNum(out_actions, true);
	}
}

int preprocInt(const std::vector<token>& args, std::vector<action>& out_actions) {
	if (args.size() != 1)
		return WRONGNARGS;

	if (args[0].type != tokenTypes::unsigned_n)
		return ARGV_ERROR;

	pushAction(out_actions, virtual_actions::_int, tokenTypes::unsigned_n, args[0].element);

	return OK;
}
int preprocMov(const std::vector<token>& args, std::vector<action>& out_actions) {
	if (args.size() != 2)
		return WRONGNARGS;

	if (args[0].type != tokenTypes::reg)
		return ARGV_ERROR;

	if (args[1].type != tokenTypes::reg)
		pushAction(out_actions, fetch_set_input.at(args[0].element), args[1].type, args[1].element);
	else
		pushAction(out_actions, fetch_mov_input.at(args[0].element), tokenTypes::reg, args[1].element);

	return OK;
}
int preprocStr(const std::vector<token>& args, std::vector<action>& out_actions) {
	if (args.size() != 1)
		return WRONGNARGS;

	if (args[0].type != tokenTypes::reg)
		return ARGV_ERROR;

	if (args[0].element == "sr")
		return OK;
	else if (args[0].element == "dr")
		pushAction(out_actions, virtual_actions::_int, tokenTypes::unsigned_n, "12");
	else if (args[0].element == "cr")
		pushAction(out_actions, virtual_actions::_int, tokenTypes::unsigned_n, "10");
	else
		pushAction(out_actions, virtual_actions::toString, tokenTypes::reg, args[0].element);

	return OK;
}

int preprocInc(const std::vector<token>& args, std::vector<action>& out_actions) {
	if (args.size() != 1)
		return WRONGNARGS;

	if (args[0].type != tokenTypes::reg)
		return ARGV_ERROR;

	// Not yet implemented
	if (args[0].element == "sr" || args[0].element == "cr")
		return ARGV_ERROR;
	else if (args[0].element == "dr")
		pushAction(out_actions, virtual_actions::incDR, tokenTypes::reg, "");
	else
		pushAction(out_actions, virtual_actions::inc, tokenTypes::reg, args[0].element);
	
	return OK;
}
int preprocDec(const std::vector<token>& args, std::vector<action>& out_actions) {
	if (args.size() != 1)
		return WRONGNARGS;

	if (args[0].type != tokenTypes::reg)
		return ARGV_ERROR;

	// Not yet implemented
	if (args[0].element == "sr" || args[0].element == "cr")
		return ARGV_ERROR;
	else if (args[0].element == "dr")
		pushAction(out_actions, virtual_actions::decDR, tokenTypes::reg, "");
	else
		pushAction(out_actions, virtual_actions::dec, tokenTypes::reg, args[0].element);

	return OK;
}
int preprocMath(const std::string& inst, const std::vector<token>& args, std::vector<action>& out_actions) {
	if (inst == "mul") {
		if (args.size() != 2)
			return WRONGNARGS;

		if (args[0].type != tokenTypes::reg)
			return ARGV_ERROR;

		if (args[0].element == "sr" || args[0].element == "dr" || args[0].element == "cr")
			{ return ARGV_ERROR; /* Not yet implemented */ }
		else if (args[1].type != tokenTypes::reg) {
			if (args[1].type != tokenTypes::unsigned_n && args[1].type != tokenTypes::signed_n)
				return ARGV_ERROR; // Not possible
			
			if (args[0].element != "rax") {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");

				pushAction(out_actions, virtual_actions::setRAX, args[1].type, args[1].element);
				pushAction(out_actions, fetch_mul_input.at(args[0].element), tokenTypes::reg, "rax");

				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax");
			}
			else {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");

				pushAction(out_actions, virtual_actions::setRBX, args[1].type, args[1].element);
				pushAction(out_actions, fetch_mul_input.at(args[0].element), tokenTypes::reg, "rbx");

				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rbx");
			}
		}
		else { pushAction(out_actions, fetch_mul_input.at(args[0].element), tokenTypes::reg, args[1].element); }

		return OK;
	}
	else if (inst == "div") {
		if (args.size() != 2)
			return WRONGNARGS;

		if (args[0].type != tokenTypes::reg)
			return ARGV_ERROR;

		if (args[0].element == "sr" || args[0].element == "dr" || args[0].element == "cr")
			{ return ARGV_ERROR; /* Not yet implemented */ }
		else if (args[1].type != tokenTypes::reg) {
			if (args[1].type != tokenTypes::unsigned_n && args[1].type != tokenTypes::signed_n)
				return ARGV_ERROR; // Not possible

			if (args[0].element != "rax") {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");

				pushAction(out_actions, virtual_actions::setRAX, args[1].type, args[1].element);
				pushAction(out_actions, fetch_div_input.at(args[0].element), tokenTypes::reg, "rax");

				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax");
			}
			else {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");

				pushAction(out_actions, virtual_actions::setRBX, args[1].type, args[1].element);
				pushAction(out_actions, fetch_div_input.at(args[0].element), tokenTypes::reg, "rbx");

				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rbx");
			}
		}
		else { pushAction(out_actions, fetch_div_input.at(args[0].element), tokenTypes::reg, args[1].element); }

		return OK;
	}
	else if (inst == "add") {
		if (args.size() != 2)
			return WRONGNARGS;

		if (args[0].type != tokenTypes::reg)
			return ARGV_ERROR;

		if (args[0].element == "sr" || args[0].element == "dr" || args[0].element == "cr")
			{ return ARGV_ERROR; /* Not yet implemented */ }
		else if (args[1].type != tokenTypes::reg) {
			if (args[1].type != tokenTypes::unsigned_n && args[1].type != tokenTypes::signed_n)
				return ARGV_ERROR; // Not possible

			if (args[0].element != "rax") {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");

				pushAction(out_actions, virtual_actions::setRAX, args[1].type, args[1].element);
				pushAction(out_actions, fetch_add_input.at(args[0].element), tokenTypes::reg, "rax");

				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax");
			}
			else {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");

				pushAction(out_actions, virtual_actions::setRBX, args[1].type, args[1].element);
				pushAction(out_actions, fetch_add_input.at(args[0].element), tokenTypes::reg, "rbx");

				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rbx");
			}
		}
		else { pushAction(out_actions, fetch_add_input.at(args[0].element), tokenTypes::reg, args[1].element); }

		return OK;
	}
	else if (inst == "sub") {
		if (args.size() != 2)
			return WRONGNARGS;

		if (args[0].type != tokenTypes::reg)
			return ARGV_ERROR;

		if (args[0].element == "sr" || args[0].element == "dr" || args[0].element == "cr")
			{ return ARGV_ERROR; /* Not yet implemented */ }
		else if (args[1].type != tokenTypes::reg) {
			if (args[1].type != tokenTypes::unsigned_n && args[1].type != tokenTypes::signed_n)
				return ARGV_ERROR; // Not possible

			if (args[0].element != "rax") {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");

				pushAction(out_actions, virtual_actions::setRAX, args[1].type, args[1].element);
				pushAction(out_actions, fetch_sub_input.at(args[0].element), tokenTypes::reg, "rax");
				
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax");
			}
			else {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");

				pushAction(out_actions, virtual_actions::setRBX, args[1].type, args[1].element);
				pushAction(out_actions, fetch_sub_input.at(args[0].element), tokenTypes::reg, "rbx");

				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rbx");
			}
		}
		else { pushAction(out_actions, fetch_sub_input.at(args[0].element), tokenTypes::reg, args[1].element); }

		return OK;
	}

	return OK;
}
int preprocLgclMath(const std::string& inst, const std::vector<token>& args, std::vector<action>& out_actions) {
	if (inst == "not") {
		if (args.size() != 1)
			return WRONGNARGS;

		if (args[0].type != tokenTypes::reg)
			return ARGV_ERROR;

		if (args[0].element == "sr") {
			return ARGV_ERROR; // Won't probably be implemented (or maybe in a long time)
		}

		if (args[0].element == "cr" || args[0].element == "dr") {
			// Not yet implemented
			return OK;
		}

		pushAction(out_actions, virtual_actions::_not, tokenTypes::reg, args[0].element);

		return OK;
	}
	else if (inst == "and") {
		if (args.size() != 2)
			return WRONGNARGS;

		if (args[0].type != tokenTypes::reg)
			return ARGV_ERROR;

		if (args[0].element == "sr" || args[1].element == "sr") {
			return ARGV_ERROR; // Won't probably be implemented (or maybe in a long time)
		}

		if (args[0].element == "cr" || args[0].element == "dr" || args[1].element == "cr" || args[1].element == "dr") {
			// Not yet implemented
			return OK;
		}

		if (args[1].type != tokenTypes::reg) {
			// Not yet implemented
		}
		else {
			pushAction(out_actions, virtual_actions::push, tokenTypes::reg, args[1].element);
			pushAction(out_actions, virtual_actions::_and, tokenTypes::reg, args[0].element);
		}
	}
	else if (inst == "xor") {
		if (args.size() != 2)
			return WRONGNARGS;

		if (args[0].type != tokenTypes::reg)
			return ARGV_ERROR;

		if (args[0].element == "sr" || args[1].element == "sr") {
			return ARGV_ERROR; // Won't probably be implemented (or maybe in a long time)
		}

		if (args[0].element == "cr" || args[0].element == "dr" || args[1].element == "cr" || args[1].element == "dr") {
			// Not yet implemented
			return OK;
		}

		if (args[1].type != tokenTypes::reg) {
			// Not yet implemented
		}
		else {
			pushAction(out_actions, virtual_actions::push, tokenTypes::reg, args[1].element);
			pushAction(out_actions, virtual_actions::_xor, tokenTypes::reg, args[0].element);
		}
	}
	else if (inst == "or") {
		if (args.size() != 2)
			return WRONGNARGS;

		if (args[0].type != tokenTypes::reg)
			return ARGV_ERROR;

		if (args[0].element == "sr" || args[1].element == "sr") {
			return ARGV_ERROR; // Won't probably be implemented (or maybe in a long time)
		}

		if (args[0].element == "cr" || args[0].element == "dr" || args[1].element == "cr" || args[1].element == "dr") {
			// Not yet implemented
			return OK;
		}

		if (args[1].type != tokenTypes::reg) {
			// Not yet implemented
		}
		else {
			pushAction(out_actions, virtual_actions::push, tokenTypes::reg, args[1].element);
			pushAction(out_actions, virtual_actions::_or, tokenTypes::reg, args[0].element);
		}
	}
	else if (inst == "shr") {
		if (args.size() != 2)
			return WRONGNARGS;

		if (args[0].type != tokenTypes::reg)
			return ARGV_ERROR;

		if (args[0].element == "sr" || args[1].element == "sr") {
			return ARGV_ERROR; // Won't probably be implemented (or maybe in a long time)
		}

		if (args[0].element == "cr" || args[0].element == "dr" || args[1].element == "cr" || args[1].element == "dr") {
			// Not yet implemented
			return OK;
		}

		if (args[1].type != tokenTypes::reg) {
			// Not yet implemented
		}
		else {
			pushAction(out_actions, virtual_actions::push, tokenTypes::reg, args[1].element);
			pushAction(out_actions, virtual_actions::_shr, tokenTypes::reg, args[0].element);
		}
	}
	else if (inst == "shl") {
		if (args.size() != 2)
			return WRONGNARGS;

		if (args[0].type != tokenTypes::reg)
			return ARGV_ERROR;

		if (args[0].element == "sr" || args[1].element == "sr") {
			return ARGV_ERROR; // Won't probably be implemented (or maybe in a long time)
		}

		if (args[0].element == "cr" || args[0].element == "dr" || args[1].element == "cr" || args[1].element == "dr") {
			// Not yet implemented
			return OK;
		}

		if (args[1].type != tokenTypes::reg) {
			// Not yet implemented
		}
		else {
			pushAction(out_actions, virtual_actions::push, tokenTypes::reg, args[1].element);
			pushAction(out_actions, virtual_actions::_shl, tokenTypes::reg, args[0].element);
		}
	}

	return OK;
}
int preprocAdvMath(const std::string& inst, const std::vector<token>& args, std::vector<action>& out_actions) {
	if (inst == "log") {
		if (args.size() != 1)
			return WRONGNARGS;

		if (args[0].type != tokenTypes::reg)
			return ARGV_ERROR;

		if (args[0].element == "sr" || args[0].element == "cr")
			return OK; // Probably won't be implemented
		else if (args[0].element == "dr")
			pushAction(out_actions, virtual_actions::_dlog, tokenTypes::reg, "");
		else
			pushAction(out_actions, virtual_actions::_log, tokenTypes::reg, args[0].element);

		return OK;
	}
	else if (inst == "log2") {
		if (args.size() != 1)
			return WRONGNARGS;

		if (args[0].type != tokenTypes::reg)
			return ARGV_ERROR;

		if (args[0].element == "sr" || args[0].element == "cr")
			return ARGV_ERROR; // Probably won't be implemented
		else if (args[0].element == "dr")
			pushAction(out_actions, virtual_actions::_dlog2, tokenTypes::reg, "");
		else
			pushAction(out_actions, virtual_actions::_log2, tokenTypes::reg, args[0].element);

		return OK;
	}
	else if (inst == "log10") {
		if (args.size() != 1)
			return WRONGNARGS;

		if (args[0].type != tokenTypes::reg)
			return ARGV_ERROR;

		if (args[0].element == "sr" || args[0].element == "cr")
			return ARGV_ERROR; // Probably won't be implemented
		else if (args[0].element == "dr")
			pushAction(out_actions, virtual_actions::_dlog10, tokenTypes::reg, "");
		else
			pushAction(out_actions, virtual_actions::_log10, tokenTypes::reg, args[0].element);

		return OK;
	}
	else if (inst == "pow") {
		if (args.size() != 2)
			return WRONGNARGS;

		if (args[0].type != tokenTypes::reg)
			return ARGV_ERROR;

		if (args[0].element == "sr" || args[0].element == "cr" || args[1].element == "sr" || args[1].element == "cr") {
			return ARGV_ERROR; // Probably won't be implemented
		}

		else if (args[0].element == "dr") {
			if (args[1].type != tokenTypes::double_n && args[1].element != "dr") {
				return ARGV_ERROR; // Not possible
			}
			else if (args[1].element == "dr") {
				pushAction(out_actions, virtual_actions::pushDR, tokenTypes::reg, "");
				pushAction(out_actions, virtual_actions::_dpow, tokenTypes::reg, "");
			}
			else {
				// args[1].element represents a double number
				// Allocate 8 bytes (sizeof double) of memory
				pushAction(out_actions, virtual_actions::pushDR, tokenTypes::reg, "");
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");
				pushAction(out_actions, virtual_actions::setRAX, tokenTypes::unsigned_n, "0");
				pushAction(out_actions, virtual_actions::movgmDR, tokenTypes::reg, "rax");
				pushAction(out_actions, virtual_actions::setRAX, tokenTypes::unsigned_n, "8");
				pushAction(out_actions, virtual_actions::movsmDR, tokenTypes::reg, "rax");
				pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax");
				pushAction(out_actions, virtual_actions::popDR, tokenTypes::reg, "");

				// prepare DR and stack before calling
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");
				pushAction(out_actions, virtual_actions::setRAX, tokenTypes::unsigned_n, "0");
				pushAction(out_actions, virtual_actions::movsmDR, tokenTypes::reg, "rax");
				pushAction(out_actions, virtual_actions::setDR, tokenTypes::double_n, args[1].element);
				pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax");
				pushAction(out_actions, virtual_actions::pushDR, tokenTypes::reg, "");
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");
				pushAction(out_actions, virtual_actions::setRAX, tokenTypes::unsigned_n, "0");
				pushAction(out_actions, virtual_actions::movgmDR, tokenTypes::reg, "rax");
				pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax");

				// Call _dpow
				pushAction(out_actions, virtual_actions::_dpow, tokenTypes::reg, "");
				pushAction(out_actions, virtual_actions::popDR, tokenTypes::reg, "");
			}
		}
		else {
			if (args[1].type != tokenTypes::unsigned_n && args[1].type != tokenTypes::reg) {
				return ARGV_ERROR; // Not possible;
			}
			else if (args[1].type == tokenTypes::reg) {
				if (args[1].element != "dr") {
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, args[1].element);
					pushAction(out_actions, virtual_actions::_pow, tokenTypes::reg, args[0].element);
				}
				else {
					// Round value of dr in this case, by using interrupt 0x0D (13)
					pushAction(out_actions, virtual_actions::_int, tokenTypes::unsigned_n, "13");
					pushAction(out_actions, virtual_actions::_pow, tokenTypes::reg, args[0].element);
				}
			}
			else {
				if (args[0].element == "rax") {
					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");

					pushAction(out_actions, virtual_actions::setRBX, tokenTypes::unsigned_n, args[1].element);
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");
					pushAction(out_actions, virtual_actions::_pow, tokenTypes::reg, args[0].element);

					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rbx");
				}
				else {
					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");

					pushAction(out_actions, virtual_actions::setRAX, tokenTypes::unsigned_n, args[1].element);
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");
					pushAction(out_actions, virtual_actions::_pow, tokenTypes::reg, args[0].element);

					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax");
				}
			}
		}

		return OK;
	}
	return OK;
}

int preprocStack(const std::string& inst, const std::vector<token>& args, std::vector<action>& out_actions) {
	if (args.size() != 1)
		return WRONGNARGS;

	if (args[0].type != tokenTypes::reg)
		return ARGV_ERROR;

	if (inst == "push") {
		if (args[0].element == "sr")
			pushAction(out_actions, virtual_actions::pushSR, tokenTypes::reg, "");
		else if (args[0].element == "cr")
			pushAction(out_actions, virtual_actions::pushCR, tokenTypes::reg, "");
		else if (args[0].element == "dr")
			pushAction(out_actions, virtual_actions::pushDR, tokenTypes::reg, "");
		else
			pushAction(out_actions, virtual_actions::push, tokenTypes::reg, args[0].element);
	}
	else if (inst == "pop") {
		if (args[0].element == "sr")
			pushAction(out_actions, virtual_actions::popSR, tokenTypes::reg, "");
		else if (args[0].element == "cr")
			pushAction(out_actions, virtual_actions::popCR, tokenTypes::reg, "");
		else if (args[0].element == "dr")
			pushAction(out_actions, virtual_actions::popDR, tokenTypes::reg, "");
		else
			pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, args[0].element);
	}

	return OK;
}
int preprocHeap(const std::string& inst, const std::vector<token>& args, std::vector<action>& out_actions) {
	if (inst == "mload") {
		if (args.size() != 2)
			return WRONGNARGS;

		/* MLOAD:
		*	if args[0] := stored_addr_reg && args[1] := reg. interpreted as movsm
		*	else if args[0] := reg && args[1] := stored_addr_reg. interpreted as movgm
		*	else returns ARGV_ERROR
		*/
		if (args[0].type == tokenTypes::stored_addr_reg && args[1].type == tokenTypes::reg) {
			if (args[1].element == "sr") {
				pushAction(out_actions, virtual_actions::movsmSR, tokenTypes::reg, args[0].element.substr(1, args[0].element.size() - 2));
			}
			else if (args[1].element == "cr") {
				pushAction(out_actions, virtual_actions::movsmCR, tokenTypes::reg, args[0].element.substr(1, args[0].element.size() - 2));
			}
			else if (args[1].element == "dr") {
				pushAction(out_actions, virtual_actions::movsmDR, tokenTypes::reg, args[0].element.substr(1, args[0].element.size() - 2));
			}
			else {
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, args[1].element);
				pushAction(out_actions, virtual_actions::movsm, tokenTypes::reg, args[0].element.substr(1, args[0].element.size() - 2));
			}
		}
		else if (args[0].type == tokenTypes::reg && args[1].type == tokenTypes::stored_addr_reg) {
			if (args[0].element == "sr") {
				pushAction(out_actions, virtual_actions::movgmSR, tokenTypes::reg, args[1].element.substr(1, args[1].element.size() - 2));
			}
			else if (args[0].element == "cr") {
				pushAction(out_actions, virtual_actions::movgmCR, tokenTypes::reg, args[1].element.substr(1, args[1].element.size() - 2));
			}
			else if (args[0].element == "dr") {
				pushAction(out_actions, virtual_actions::movgmDR, tokenTypes::reg, args[1].element.substr(1, args[1].element.size() - 2));
			}
			else {
				pushAction(out_actions, virtual_actions::movgm, tokenTypes::reg, args[1].element.substr(1, args[1].element.size() - 2));
				pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, args[0].element);
			}
		}
		else { return ARGV_ERROR; }

		return OK;
	}
	else if (inst == "sdzs") {
		if (args.size() != 1)
			return WRONGNARGS;

		if (args[0].type != tokenTypes::reg)
			return ARGV_ERROR;

		pushAction(out_actions, virtual_actions::sdzs, tokenTypes::reg, args[0].element);
	}

	return OK;
}

int preprocCalls(const std::string& inst, const std::vector<token>& args, std::vector<action>& out_actions) {
	if (args.size() != 1) {
		if (args.size() == 0) {
			if (inst == "ret")
				pushAction(out_actions, virtual_actions::ret, tokenTypes::reg, "");
			else
				return WRONGNARGS;

			return OK;
		}
		else { return WRONGNARGS;  }
	}

	if (args[0].type != tokenTypes::label && args[0].type != tokenTypes::unsigned_n && args[0].type != tokenTypes::signed_n)
		return ARGV_ERROR;

	if (inst == "call") {
		if (args[0].type == tokenTypes::label) {
			pushAction(out_actions, virtual_actions::svcall, tokenTypes::reg, "");
			pushAction(out_actions, virtual_actions::call, tokenTypes::label, args[0].element);
			pushAction(out_actions, virtual_actions::rscall, tokenTypes::reg, "");
		}
		else if (args[0].type == tokenTypes::unsigned_n) {
			pushAction(out_actions, virtual_actions::svcall, tokenTypes::reg, "");
			pushAction(out_actions, virtual_actions::call, tokenTypes::unsigned_n, args[0].element);
			pushAction(out_actions, virtual_actions::rscall, tokenTypes::reg, "");
		}
		else { return ARGV_ERROR; }
	}
	else if (inst == "jmp") {
		if (args[0].type == tokenTypes::label)
			pushAction(out_actions, virtual_actions::lcall, tokenTypes::label, args[0].element);
		else {
			pushAction(out_actions, virtual_actions::jmp, args[0].type, args[0].element);
		}
	}

	return OK;
}
int preprocCmpCalls(const std::string& inst, const std::vector<token>& args, std::vector<action>& out_actions) {
	if (args.size() != 1)
		return WRONGNARGS;

	if (args[0].type != tokenTypes::label && args[0].type != tokenTypes::unsigned_n && args[0].type != tokenTypes::signed_n)
		return ARGV_ERROR;

	if (inst == "je") {
		preprocJmpCmpCalls(out_actions, virtual_actions::je, args[0]);
	}
	else if (inst == "jne") {
		preprocJmpCmpCalls(out_actions, virtual_actions::jne, args[0]);
	}
	else if (inst == "jl") {
		preprocJmpCmpCalls(out_actions, virtual_actions::jl, args[0]);
	}
	else if (inst == "jle") {
		preprocJmpCmpCalls(out_actions, virtual_actions::jle, args[0]);
	}
	else if (inst == "jg") {
		preprocJmpCmpCalls(out_actions, virtual_actions::jg, args[0]);
	}
	else if (inst == "jge") {
		preprocJmpCmpCalls(out_actions, virtual_actions::jge, args[0]);
	}

	return OK;
}
int preprocComps(const std::vector<token>& args, std::vector<action>& out_actions) {
	if (args.size() != 2)
		return WRONGNARGS;

	if (args[0].type != tokenTypes::reg || args[1].type == tokenTypes::stored_addr_reg || args[1].type == tokenTypes::stored_addr_raw)
		return ARGV_ERROR;

	if (num_registers.find(args[0].element) != num_registers.end()) {
		if (args[1].element == "sr" || args[1].type == tokenTypes::str) {
			if (args[1].type == tokenTypes::reg) { // TODO: change to compare SR with string repr of register
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::pushSR, tokenTypes::reg, ""); // Save SR

				pushAction(out_actions, virtual_actions::pushSR, tokenTypes::reg, "");

				pushAction(out_actions, virtual_actions::toString, tokenTypes::reg, args[0].element); // Cast value of GP register
				pushAction(out_actions, virtual_actions::pushSR, tokenTypes::reg, "");

				pushAction(out_actions, virtual_actions::cmpstr, tokenTypes::reg, "");

				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::popSR, tokenTypes::reg, ""); // Restore SR
			}
			else {
				// Not yet implemented
			}
		}
		else if (args[1].element == "cr") {
			if (args[0].element == "rax") {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx"); // Save rbx

				CRToNum(out_actions, false);
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");

				// Do comparison
				pushAction(out_actions, virtual_actions::cmp, tokenTypes::reg, "");

				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rbx"); // Restore rbx
			}
			else {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax"); // Save rax
				
				CRToNum(out_actions, true);
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, args[0].element);

				// Do comparison
				pushAction(out_actions, virtual_actions::cmp, tokenTypes::reg, "");

				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax"); // Restore rax
			}
		}
		else if (args[1].element == "dr" || args[1].type == tokenTypes::double_n) {
			if (args[1].element == "dr") {
				pushAction(out_actions, virtual_actions::_int, tokenTypes::unsigned_n, "13");
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, args[0].element);
				pushAction(out_actions, virtual_actions::cmp, tokenTypes::reg, "");
			}
			else {
				// Not yet implemented
			}
		}
		else {
			if (args[1].type != tokenTypes::reg) { // args[1] is a signed/unsigned number
				if (args[0].element == "rax") {
					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");

					pushAction(out_actions, virtual_actions::setRBX, args[1].type, args[1].element);
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, args[0].element);
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");
					pushAction(out_actions, virtual_actions::cmp, tokenTypes::reg, "");

					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rbx");
				}
				else {
					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");

					pushAction(out_actions, virtual_actions::setRAX, args[1].type, args[1].element);
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, args[0].element);
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");
					pushAction(out_actions, virtual_actions::cmp, tokenTypes::reg, "");

					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax");
				}
			}
			else {
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, args[1].element);
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, args[0].element);

				pushAction(out_actions, virtual_actions::cmp, tokenTypes::reg, "");
			}
		}
	}
	else if (args[0].element == "sr") {
		if (args[1].element == "cr" || args[1].type == tokenTypes::str) {
			if (args[1].type == tokenTypes::reg) {
				if (!unsafe_flag) {
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax"); // Save rax
					pushAction(out_actions, virtual_actions::pushCR, tokenTypes::reg, ""); // Save cr
				}

				CRToNum(out_actions, true);
				getFirstCharSR(out_actions);

				// Do comparison
				pushAction(out_actions, virtual_actions::cmp, tokenTypes::reg, "");

				if (!unsafe_flag) {
					pushAction(out_actions, virtual_actions::popCR, tokenTypes::reg, ""); // Restore cr
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax"); // Restore rax
				}
			}
			else {
				// Not yet implemented
			}
		}
		else if (args[1].element == "dr" || args[1].type == tokenTypes::double_n) {
			if (args[1].element == "dr") {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::pushSR, tokenTypes::reg, ""); // Save sr

				pushAction(out_actions, virtual_actions::_int, tokenTypes::unsigned_n, "12");
				pushAction(out_actions, virtual_actions::pushSR, tokenTypes::reg, "");

				pushAction(out_actions, virtual_actions::pushSR, tokenTypes::reg, "");

				pushAction(out_actions, virtual_actions::cmpstr, tokenTypes::reg, "");

				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::popSR, tokenTypes::reg, ""); // Restore sr
			}
			else {
				// Not yet implemented
			}
		}
		else {
			// Will need to treat output as if it was the inverse of the comparison result in case
			// you planned to use jl/jg/jle/jge only, since the strings are not pushed into the right order
			// it has been done like so to avoid potentially damaging memory after 128 first byte of memory (because SR could hold
			// a string which length is greater than 120 byte - with 8 bytes used to store string length)

			if (args[1].type != tokenTypes::reg) {
				if (!unsafe_flag) {
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax"); // Save rax
					pushAction(out_actions, virtual_actions::pushSR, tokenTypes::reg, ""); // Save SR
				}

				pushAction(out_actions, virtual_actions::pushSR, tokenTypes::reg, "");

				pushAction(out_actions, virtual_actions::setRAX, args[1].type, args[1].element);
				pushAction(out_actions, virtual_actions::toString, tokenTypes::reg, "rax"); // Cast value of GP register
				pushAction(out_actions, virtual_actions::pushSR, tokenTypes::reg, "");

				pushAction(out_actions, virtual_actions::cmpstr, tokenTypes::reg, "");

				if (!unsafe_flag) {
					pushAction(out_actions, virtual_actions::popSR, tokenTypes::reg, ""); // Restore SR
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax"); // Restore rax
				}
			}
			else {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::pushSR, tokenTypes::reg, ""); // Save SR

				pushAction(out_actions, virtual_actions::pushSR, tokenTypes::reg, "");

				pushAction(out_actions, virtual_actions::toString, tokenTypes::reg, args[1].element); // Cast value of GP register
				pushAction(out_actions, virtual_actions::pushSR, tokenTypes::reg, "");

				pushAction(out_actions, virtual_actions::cmpstr, tokenTypes::reg, "");

				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::popSR, tokenTypes::reg, ""); // Restore SR
			}
		}
	}
	else if (args[0].element == "cr") {
		if (args[1].element == "sr" || args[1].type == tokenTypes::str) {
			if (args[1].type == tokenTypes::reg) {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax"); // Save rax

				pushAction(out_actions, virtual_actions::pushCR, tokenTypes::reg, ""); // Save cr

				// Get first char of SR
				pushAction(out_actions, virtual_actions::setRAX, tokenTypes::unsigned_n, "2");
				// Prepare recast arguments
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");
				pushAction(out_actions, virtual_actions::pushSR, tokenTypes::reg, "");
				// Invoke recast inerrupt
				pushAction(out_actions, virtual_actions::_int, tokenTypes::unsigned_n, "9");
				// Pop first char of SR from stack
				pushAction(out_actions, virtual_actions::popCR, tokenTypes::reg, "");
				// Set first 8 bytes of memory to 0
				pushAction(out_actions, virtual_actions::setRAX, tokenTypes::unsigned_n, "0");
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");
				pushAction(out_actions, virtual_actions::movsm, tokenTypes::reg, "rax");
				// Set value of CR into 7th byte of memory
				pushAction(out_actions, virtual_actions::setRAX, tokenTypes::unsigned_n, "7");
				pushAction(out_actions, virtual_actions::movsmCR, tokenTypes::reg, "rax");
				pushAction(out_actions, virtual_actions::popCR, tokenTypes::reg, ""); // Restore cr

				// Fetch first 8 bytes of memory and leave them on stack
				pushAction(out_actions, virtual_actions::setRAX, tokenTypes::unsigned_n, "0");
				pushAction(out_actions, virtual_actions::movgm, tokenTypes::reg, "rax");

				CRToNum(out_actions, true);

				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax"); // Restore rax
			}
			else {
				// Not yet implemented
			}
		}
		else if (args[1].element == "dr" || args[1].type == tokenTypes::double_n) {
			return ARGV_ERROR; // No real meaning of doing that comparison
		}
		else {
			if (args[1].type != tokenTypes::reg) {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax"); // Save rax

				pushAction(out_actions, virtual_actions::setRAX, args[1].type, args[1].element);
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");

				CRToNum(out_actions, true);

				pushAction(out_actions, virtual_actions::cmp, tokenTypes::reg, "");

				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax"); // Restore rax
			}
			else {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax"); // Save rax

				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");

				CRToNum(out_actions, true);

				pushAction(out_actions, virtual_actions::cmp, tokenTypes::reg, "");

				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax"); // Restore rax
			}
		}
	}
	else if (args[0].element == "dr") {
		if (args[1].element == "sr" || args[1].type == tokenTypes::str) {
			if (args[1].type == tokenTypes::reg) {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::pushSR, tokenTypes::reg, ""); // Save sr

				pushAction(out_actions, virtual_actions::pushSR, tokenTypes::reg, "");

				pushAction(out_actions, virtual_actions::_int, tokenTypes::unsigned_n, "12");
				pushAction(out_actions, virtual_actions::pushSR, tokenTypes::reg, "");

				pushAction(out_actions, virtual_actions::cmpstr, tokenTypes::reg, "");

				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::popSR, tokenTypes::reg, ""); // Restore sr
			}
			else {
				// Not yet implemented
			}
		}
		else if (args[1].element == "cr") {
			return ARGV_ERROR; // No real meaning of doing that comparison
		}
		else if (args[1].type == tokenTypes::double_n) {
			if (!unsafe_flag)
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax"); // Save rax

			pushAction(out_actions, virtual_actions::pushDR, tokenTypes::reg, ""); // Save DR

			pushAction(out_actions, virtual_actions::setDR, tokenTypes::double_n, args[1].element);
			// Set first 8 bytes of memory to 0
			pushAction(out_actions, virtual_actions::setRAX, tokenTypes::unsigned_n, "0");
			pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");
			pushAction(out_actions, virtual_actions::movsm, tokenTypes::reg, "rax");
			// Set value of DR (8 bytes) into first 8 bytes of memory
			pushAction(out_actions, virtual_actions::movsmDR, tokenTypes::reg, "rax");

			pushAction(out_actions, virtual_actions::popDR, tokenTypes::reg, ""); // Restore DR
			// Fetch first 8 bytes of memory and leave them on stack
			// movgm to push on stack (movgmDR would have set the value into DR)
			pushAction(out_actions, virtual_actions::movgm, tokenTypes::reg, "rax");

			pushAction(out_actions, virtual_actions::pushDR, tokenTypes::reg, "");
			pushAction(out_actions, virtual_actions::cmp, tokenTypes::reg, "");

			if (!unsafe_flag)
				pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax"); // Restore rax
		}
		else {
			if (args[1].type == tokenTypes::reg) {
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, args[1].element);
				pushAction(out_actions, virtual_actions::_int, tokenTypes::unsigned_n, "13");
				pushAction(out_actions, virtual_actions::cmp, tokenTypes::reg, "");
			}
			else {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax"); // Save rax

				pushAction(out_actions, virtual_actions::setRAX, tokenTypes::unsigned_n, args[1].element);
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");
				pushAction(out_actions, virtual_actions::_int, tokenTypes::unsigned_n, "13");
				pushAction(out_actions, virtual_actions::cmp, tokenTypes::reg, "");

				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax"); // Restore rax
			}
		}
	}

	return OK;
}

int preprocInst(const tokenized& tokens, std::unordered_map<std::string, size_t>& preprocLabels, std::vector<action>& out_actions) {
	const std::string& inst = tokens.instruction;

	if (inst == "[labeldef]") {
		preprocLabels[tokens.arguments[0].element] = out_actions.size() - 1;
		return OK;
	}
	else if (inst == "[safety_control]" && tokens.arguments[0].type == tokenTypes::safety_directive) {
		if (tokens.arguments[0].element == "safe")
			unsafe_flag = false;
		else
			unsafe_flag = true;
		return OK;
	}
	else if (inst == "[memory_control]" && tokens.arguments[0].type == tokenTypes::memory_directive) {
		if (!memory_flags.mem_res_req) {
			memory_flags.new_mem_size = std::stoull(tokens.arguments[0].element);
			memory_flags.mem_res_req = true;
		}
		return OK;
	}
	else if (inst == "int")
		return preprocInt(tokens.arguments, out_actions);
	else if (inst == "mov")
		return preprocMov(tokens.arguments, out_actions);
	else if (inst == "inc")
		return preprocInc(tokens.arguments, out_actions);
	else if (inst == "dec")
		return preprocDec(tokens.arguments, out_actions);
	else if (inst == "mul" || inst == "div" || inst == "add" || inst == "sub")
		return preprocMath(inst, tokens.arguments, out_actions);
	else if (inst == "not" || inst == "and" || inst == "xor" || inst == "or" || inst == "shr" || inst == "shl")
		return preprocLgclMath(inst, tokens.arguments, out_actions);
	else if (inst == "log" || inst == "log2" || inst == "log10" || inst == "pow")
		return preprocAdvMath(inst, tokens.arguments, out_actions);
	else if (inst == "push" || inst == "pop")
		return preprocStack(inst, tokens.arguments, out_actions);
	else if (inst == "mload" || inst == "sdzs")
		return preprocHeap(inst, tokens.arguments, out_actions);
	else if (inst == "str")
		return preprocStr(tokens.arguments, out_actions);
	else if (inst == "call" || inst == "jmp" || inst == "ret")
		return preprocCalls(inst, tokens.arguments, out_actions);
	else if (inst == "je" || inst == "jge" || inst == "jl" || inst == "jle" || inst == "jg" || inst == "jge")
		return preprocCmpCalls(inst, tokens.arguments, out_actions);
	else if (inst == "cmp")
		return preprocComps(tokens.arguments, out_actions);

	return OK;
}
int preprocTokenized(const std::vector<tokenized> tokens, std::vector<action>& out_actions) {
	out_actions.reserve(tokens.size()); // Contains at least, as many elements as there are in "std::vector<tokenized> tokens"
	std::unordered_map<std::string, size_t> preprocLabels;
	
	for (const tokenized token : tokens) {
		int ret = preprocInst(token, preprocLabels, out_actions);
		if (ret)
			return ret;
	}
	for (action& _a : out_actions) {
		auto tpl = std::static_pointer_cast<std::tuple<tokenTypes, std::string>>(_a.getValuePtr());
		auto[vtype, vval] = *tpl;
		if (vtype == tokenTypes::label) {
			vval = std::to_string(preprocLabels[vval]);
			*tpl = std::make_tuple<const tokenTypes&, const std::string&>(tokenTypes::unsigned_n, vval);
		}
	}

	unsafe_flag = false;

	return OK;
}


int compileInst(action& raw_action, std::vector<byte>& out_bytes) {
	static const std::map<std::string, registries_def> storeg = {
		{"ax", registries_def::AX},
		{"bx", registries_def::BX},
		{"cx", registries_def::CX},
		{"dx", registries_def::DX},

		{"eax", registries_def::EAX},
		{"ebx", registries_def::EBX},
		{"ecx", registries_def::ECX},
		{"edx", registries_def::EDX},

		{"rax", registries_def::RAX},
		{"rbx", registries_def::RBX},
		{"rcx", registries_def::RCX},
		{"rdx", registries_def::RDX}
	};

	const byte comp_action = instructions_set[raw_action.getAction()];
	const auto v_ptr = *std::static_pointer_cast<std::tuple<tokenTypes, std::string>>(raw_action.getValuePtr());
	
	const tokenTypes v_type = std::get<0>(v_ptr);
	std::string v_raw = std::get<1>(v_ptr);

	if (zero_args_opcodes.find(comp_action) != zero_args_opcodes.end()) {
		out_bytes.push_back(comp_action);
		return OK;
	}
	else if (uint64_args_opcodes.find(comp_action) != uint64_args_opcodes.end()) {
		if (v_type != tokenTypes::signed_n && v_type != tokenTypes::unsigned_n)
			return ARGV_ERROR;

		if (v_type == tokenTypes::unsigned_n) {
			const size_t value = std::stoull(v_raw);
			byte* uc_n = nullptr;

			ULLTOA(value, &uc_n);
			const size_t compressed_len = COMPBA(uc_n, sizeof(size_t));

			out_bytes.push_back(comp_action);
			for (byte i = 0; i < compressed_len; i++)
				out_bytes.push_back(uc_n[i]);

			delete[] uc_n;
			return OK;
		}
		else if (v_type == tokenTypes::signed_n) {
			const long long value = std::stoll(v_raw);
			byte* uc_n = nullptr;

			ULLTOA((size_t)value, &uc_n);
			const size_t compressed_len = COMPBA(uc_n, sizeof(size_t));

			out_bytes.push_back(comp_action);
			for (byte i = 0; i < compressed_len; i++)
				out_bytes.push_back(uc_n[i]);

			delete[] uc_n;
			return OK;
		}
	}
	else if (comp_action == instructions_set[virtual_actions::setSR]) {
		if (v_type != tokenTypes::str)
			return ARGV_ERROR;

		out_bytes.push_back(comp_action);

		v_raw = v_raw.substr(1, v_raw.size() - 2);
		size_t str_size = v_raw.size() + 1;

		byte* b_str_size = nullptr;
		ULLTOA(str_size, &b_str_size);

		size_t compressed_len = COMPBA(b_str_size, sizeof(size_t));

		for (byte i = 0; i < compressed_len; i++)
			out_bytes.push_back(b_str_size[i]);
		delete[] b_str_size;

		byte* b_str = new byte[str_size];
#if defined(ISWIN)
		memcpy_s(b_str, str_size, v_raw.c_str(), str_size);
#else
		std::memcpy(b_str, v_raw.c_str(), str_size);
#endif

		for (size_t i = 0; i < str_size; i++)
			out_bytes.push_back(b_str[i]);
		delete[] b_str;

		return OK;
	}
	else if (comp_action == instructions_set[virtual_actions::setCR]) {
		if (v_type != tokenTypes::str)
			return ARGV_ERROR;

		v_raw = v_raw.substr(1, v_raw.size() - 2);
		if (v_raw.empty() || v_raw.size() < 1)
			return ARGV_ERROR;

		byte c = (byte)v_raw[0];

		out_bytes.push_back(comp_action);
		out_bytes.push_back(c);

		return OK;
	}
	else if (comp_action == instructions_set[virtual_actions::setDR]) {
		if (v_type != tokenTypes::double_n)
			return ARGV_ERROR;

		const double value = std::stod(v_raw);
		byte* uc_d = nullptr;

		DTOA(value, &uc_d);

		out_bytes.push_back(comp_action);
		for (byte i = 0; i < sizeof(double); i++)
			out_bytes.push_back(uc_d[i]);
		delete[] uc_d;

		return OK;
	}
	else if (reg_args_opcodes.find(comp_action) != reg_args_opcodes.end()) {
		if (v_type != tokenTypes::reg)
			return ARGV_ERROR;

		const byte comp_reg = registers_set[storeg.at(v_raw)];

		out_bytes.push_back(comp_action);
		out_bytes.push_back(comp_reg);
		return OK;
	}

	return OK;
}
int compileAll(const std::vector<action>& raw_actions, std::vector<byte>& out_bytes) {
	out_bytes.reserve(raw_actions.size() * 2); // In average, an action needs two bytes

	// Insert amount memory requested by program at the beginning of VEXE file
	byte* req_mem_size = nullptr;
	if (memory_flags.mem_res_req)
		ULLTOA(memory_flags.new_mem_size, &req_mem_size);
	else
		ULLTOA(0x100000, &req_mem_size); // Default value

	for (size_t i = 0; i < sizeof(size_t); i++)
		out_bytes.emplace_back(req_mem_size[i]);
	delete[] req_mem_size;


	for (action raw_a : raw_actions) {
		if (compileInst(raw_a, out_bytes))
			return ARGV_ERROR;
	}

	return OK;
}