#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

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
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");
				pushAction(out_actions, virtual_actions::setRAX, args[1].type, args[1].element);
				pushAction(out_actions, fetch_mul_input.at(args[0].element), tokenTypes::reg, "rax");
				pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax");
			}
			else {
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");
				pushAction(out_actions, virtual_actions::setRBX, args[1].type, args[1].element);
				pushAction(out_actions, fetch_mul_input.at(args[0].element), tokenTypes::reg, "rbx");
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
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");
				pushAction(out_actions, virtual_actions::setRAX, args[1].type, args[1].element);
				pushAction(out_actions, fetch_div_input.at(args[0].element), tokenTypes::reg, "rax");
				pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax");
			}
			else {
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");
				pushAction(out_actions, virtual_actions::setRBX, args[1].type, args[1].element);
				pushAction(out_actions, fetch_div_input.at(args[0].element), tokenTypes::reg, "rbx");
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
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");
				pushAction(out_actions, virtual_actions::setRAX, args[1].type, args[1].element);
				pushAction(out_actions, fetch_add_input.at(args[0].element), tokenTypes::reg, "rax");
				pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax");
			}
			else {
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");
				pushAction(out_actions, virtual_actions::setRBX, args[1].type, args[1].element);
				pushAction(out_actions, fetch_add_input.at(args[0].element), tokenTypes::reg, "rbx");
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
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");
				pushAction(out_actions, virtual_actions::setRAX, args[1].type, args[1].element);
				pushAction(out_actions, fetch_sub_input.at(args[0].element), tokenTypes::reg, "rax");
				pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax");
			}
			else {
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");
				pushAction(out_actions, virtual_actions::setRBX, args[1].type, args[1].element);
				pushAction(out_actions, fetch_sub_input.at(args[0].element), tokenTypes::reg, "rbx");
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
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");
					pushAction(out_actions, virtual_actions::setRBX, tokenTypes::unsigned_n, args[1].element);
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");
					pushAction(out_actions, virtual_actions::_pow, tokenTypes::reg, args[0].element);
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rbx");
				}
				else {
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");
					pushAction(out_actions, virtual_actions::setRAX, tokenTypes::unsigned_n, args[1].element);
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");
					pushAction(out_actions, virtual_actions::_pow, tokenTypes::reg, args[0].element);
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax");
				}
			}
		}

		return OK;
	}
	return OK;
}

int preprocInst(const tokenized& tokens, std::vector<action>& out_actions) {
	const std::string& inst = tokens.instruction;

	if (inst == "int")
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

	return OK;
}
int preprocTokenized(const std::vector<tokenized> tokens, std::vector<action>& out_actions) {
	out_actions.reserve(tokens.size());
	
	for (const tokenized token : tokens) {
		int ret = preprocInst(token, out_actions);
		if (ret)
			return ret;
	}

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

	for (action raw_a : raw_actions) {
		if (compileInst(raw_a, out_bytes))
			return ARGV_ERROR;
	}

	return OK;
}