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
#include "../../VirtualAssembler/assembler.h"
#include "../../utility.h"
#include "compiler.h"
#include "parser.h"

typedef std::tuple <tokenTypes, std::string, uint8_t> svasm_args;

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
	"rdx",

	"rbp",
	"rsp"
};
static const std::unordered_set<std::string> fp_registers = {
	"fpr0",
	"fpr1",
	"fpr2",
	"fpr3",
	"efpr0",
	"efpr1",
	"efpr2",
	"efpr3",
	"rfpr0",
	"rfpr1",
	"rfpr2",
	"rfpr3"
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

	{"fpr0", virtual_actions::setFPR0},
	{"fpr1", virtual_actions::setFPR1},
	{"fpr2", virtual_actions::setFPR2},
	{"fpr3", virtual_actions::setFPR3},

	{"efpr0", virtual_actions::setEFPR0},
	{"efpr1", virtual_actions::setEFPR1},
	{"efpr2", virtual_actions::setEFPR2},
	{"efpr3", virtual_actions::setEFPR3},

	{"rfpr0", virtual_actions::setRFPR0},
	{"rfpr1", virtual_actions::setRFPR1},
	{"rfpr2", virtual_actions::setRFPR2},
	{"rfpr3", virtual_actions::setRFPR3}
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
	{"rdx", virtual_actions::movRDX},

	{"rbp", virtual_actions::movRBP},
	{"rsp", virtual_actions::movRSP},

	{"fpr0", virtual_actions::movFPR0},
	{"fpr1", virtual_actions::movFPR1},
	{"fpr2", virtual_actions::movFPR2},
	{"fpr3", virtual_actions::movFPR3},

	{"efpr0", virtual_actions::movEFPR0},
	{"efpr1", virtual_actions::movEFPR1},
	{"efpr2", virtual_actions::movEFPR2},
	{"efpr3", virtual_actions::movEFPR3},

	{"rfpr0", virtual_actions::movRFPR0},
	{"rfpr1", virtual_actions::movRFPR1},
	{"rfpr2", virtual_actions::movRFPR2},
	{"rfpr3", virtual_actions::movRFPR3}
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
	{"rdx", virtual_actions::mulRDX},

	{"fpr0", virtual_actions::mulFPR0},
	{"fpr1", virtual_actions::mulFPR1},
	{"fpr2", virtual_actions::mulFPR2},
	{"fpr3", virtual_actions::mulFPR3},

	{"efpr0", virtual_actions::mulEFPR0},
	{"efpr1", virtual_actions::mulEFPR1},
	{"efpr2", virtual_actions::mulEFPR2},
	{"efpr3", virtual_actions::mulEFPR3},

	{"rfpr0", virtual_actions::mulRFPR0},
	{"rfpr1", virtual_actions::mulRFPR1},
	{"rfpr2", virtual_actions::mulRFPR2},
	{"rfpr3", virtual_actions::mulRFPR3}
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
	{"rdx", virtual_actions::divRDX},

	{"fpr0", virtual_actions::divFPR0},
	{"fpr1", virtual_actions::divFPR1},
	{"fpr2", virtual_actions::divFPR2},
	{"fpr3", virtual_actions::divFPR3},

	{"efpr0", virtual_actions::divEFPR0},
	{"efpr1", virtual_actions::divEFPR1},
	{"efpr2", virtual_actions::divEFPR2},
	{"efpr3", virtual_actions::divEFPR3},

	{"rfpr0", virtual_actions::divRFPR0},
	{"rfpr1", virtual_actions::divRFPR1},
	{"rfpr2", virtual_actions::divRFPR2},
	{"rfpr3", virtual_actions::divRFPR3}
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
	{"rdx", virtual_actions::addRDX},

	{"rbp", virtual_actions::addRBP},
	{"rsp", virtual_actions::addRSP},

	{"fpr0", virtual_actions::addFPR0},
	{"fpr1", virtual_actions::addFPR1},
	{"fpr2", virtual_actions::addFPR2},
	{"fpr3", virtual_actions::addFPR3},

	{"efpr0", virtual_actions::addEFPR0},
	{"efpr1", virtual_actions::addEFPR1},
	{"efpr2", virtual_actions::addEFPR2},
	{"efpr3", virtual_actions::addEFPR3},

	{"rfpr0", virtual_actions::addRFPR0},
	{"rfpr1", virtual_actions::addRFPR1},
	{"rfpr2", virtual_actions::addRFPR2},
	{"rfpr3", virtual_actions::addRFPR3}
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
	{"rdx", virtual_actions::subRDX},

	{"rbp", virtual_actions::subRBP},
	{"rsp", virtual_actions::subRSP},

	{"fpr0", virtual_actions::subFPR0},
	{"fpr1", virtual_actions::subFPR1},
	{"fpr2", virtual_actions::subFPR2},
	{"fpr3", virtual_actions::subFPR3},

	{"efpr0", virtual_actions::subEFPR0},
	{"efpr1", virtual_actions::subEFPR1},
	{"efpr2", virtual_actions::subEFPR2},
	{"efpr3", virtual_actions::subEFPR3},

	{"rfpr0", virtual_actions::subRFPR0},
	{"rfpr1", virtual_actions::subRFPR1},
	{"rfpr2", virtual_actions::subRFPR2},
	{"rfpr3", virtual_actions::subRFPR3}
};

static bool unsafe_flag = false;
struct {
	bool mem_res_req = false;
	size_t new_mem_size = 0;
} memory_flags;

namespace {
	void pushAction(std::vector<action>& out_actions, const virtual_actions& vaction,
			const tokenTypes& var_type, const std::string& var_raw, uint8_t opt_arg = 0) {
		out_actions.emplace_back(action(vaction, std::make_shared<svasm_args>(var_type, var_raw, opt_arg)));
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

	inline void CRToNum(std::vector<action>& out_actions, const bool useRAX) {
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

	inline void ASMBL_FP_NMATHS(std::vector<byte>& out, const std::map<virtual_actions, byte>& map2ndOPC,
			action& _action, const byte reg_value) {

		out.push_back(map2ndOPC.at(_action.getAction()));
		out.push_back(reg_value);
	}

	// Convert SVOPT to Type Quantifier
	inline uint8_t SVOPTTOTQ(const std::string& svopt) {
		if (svopt == "QWORD" || svopt == "qword")
			return 0b100u;
		else if (svopt == "DWORD" || svopt == "dword")
			return 0b011u;
		else if (svopt == "WORD" || svopt == "word")
			return 0b010u;
		else if (svopt == "BYTE" || svopt == "byte")
			return 0b001u;
		else
			return 0b000u;
	}
	// Convert SVOPT to Type Size
	inline size_t SVOPTTOTS(const std::string& svopt) {
		if (svopt == "QWORD" || svopt == "qword")
			return 8;
		else if (svopt == "DWORD" || svopt == "dword")
			return 4;
		else if (svopt == "WORD" || svopt == "word")
			return 2;
		else if (svopt == "BYTE" || svopt == "byte")
			return 4;
		else
			return sizeof(size_t);
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

	if (args[0].type == tokenTypes::reg) {
		if (args[1].type != tokenTypes::reg)
			pushAction(out_actions, fetch_set_input.at(args[0].element), args[1].type, args[1].element);
		else
			pushAction(out_actions, fetch_mov_input.at(args[0].element), tokenTypes::reg, args[1].element);
	}
	else if (args[0].type == tokenTypes::fp_reg) {
		if (args[1].type != tokenTypes::fp_reg)
			pushAction(out_actions, fetch_set_input.at(args[0].element), args[1].type, args[1].element);
		else
			pushAction(out_actions, fetch_mov_input.at(args[0].element), tokenTypes::fp_reg, args[1].element);
	}
	else { return ARGV_ERROR; }

	return OK;
}
int preprocStr(const std::vector<token>& args, std::vector<action>& out_actions) {
	if (args.size() != 1)
		return WRONGNARGS;

	if (args[0].type != tokenTypes::reg && args[0].type != tokenTypes::fp_reg)
		return ARGV_ERROR;

	if (args[0].element == "sr")
		return OK;
	else if (args[0].element == "cr")
		pushAction(out_actions, virtual_actions::_int, tokenTypes::unsigned_n, "10");
	else if (args[0].type == tokenTypes::fp_reg) {
		pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, args[0].element);
		pushAction(out_actions, virtual_actions::_int, tokenTypes::unsigned_n, "12");
	}
	else
		pushAction(out_actions, virtual_actions::toString, tokenTypes::reg, args[0].element);

	return OK;
}

int preprocInc(const std::vector<token>& args, std::vector<action>& out_actions) {
	if (args.size() != 1)
		return WRONGNARGS;

	if (args[0].type != tokenTypes::reg && args[0].type != tokenTypes::fp_reg)
		return ARGV_ERROR;

	if (args[0].element == "sr")
		return ARGV_ERROR; // No purpose of incrementing sr

	// Not yet implemented
	if (args[0].element == "cr")
		return ARGV_ERROR;
	else if (args[0].type == tokenTypes::fp_reg)
		pushAction(out_actions, virtual_actions::incFP, tokenTypes::fp_reg, args[0].element);
	else
		pushAction(out_actions, virtual_actions::inc, tokenTypes::reg, args[0].element);
	
	return OK;
}
int preprocDec(const std::vector<token>& args, std::vector<action>& out_actions) {
	if (args.size() != 1)
		return WRONGNARGS;

	if (args[0].type != tokenTypes::reg)
		return ARGV_ERROR;

	if (args[0].element == "sr")
		return ARGV_ERROR;

	// Not yet implemented
	if (args[0].element == "cr")
		return ARGV_ERROR;
	else if (args[0].type == tokenTypes::fp_reg)
		pushAction(out_actions, virtual_actions::decFP, tokenTypes::fp_reg, args[0].element);
	else
		pushAction(out_actions, virtual_actions::dec, tokenTypes::reg, args[0].element);

	return OK;
}
int preprocMath(const std::string& inst, const std::vector<token>& args, std::vector<action>& out_actions) {
	if (inst == "mul") {
		if (args.size() != 2)
			return WRONGNARGS;

		if (args[0].type != tokenTypes::reg && args[0].type != tokenTypes::fp_reg)
			return ARGV_ERROR;

		if (args[0].element == "sr" || args[0].element == "cr")
			{ return ARGV_ERROR; /* Not yet implemented */ }
		else if (args[1].type != tokenTypes::reg && args[1].type != tokenTypes::fp_reg) {
			if (args[1].type != tokenTypes::unsigned_n && args[1].type != tokenTypes::signed_n && args[1].type != tokenTypes::double_n)
				return ARGV_ERROR; // Not possible

			if (args[1].type != tokenTypes::double_n) {
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
			else {
				if (args[0].element != "rfpr3") {
					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr3");

					pushAction(out_actions, virtual_actions::setRFPR3, tokenTypes::double_n, args[1].element);
					pushAction(out_actions, fetch_mul_input.at(args[0].element), tokenTypes::fp_reg, "rfpr3");

					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::popFP, tokenTypes::fp_reg, "rfpr3");
				}
				else {
					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr2");

					pushAction(out_actions, virtual_actions::setRFPR2, tokenTypes::double_n, args[1].element);
					pushAction(out_actions, fetch_mul_input.at(args[0].element), tokenTypes::fp_reg, "rfpr2");

					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::popFP, tokenTypes::fp_reg, "rfpr2");
				}
			}
		}
		else {
			if (args[0].type == tokenTypes::reg)
				pushAction(out_actions, fetch_mul_input.at(args[0].element), tokenTypes::reg, args[1].element);
			else
				pushAction(out_actions, fetch_mul_input.at(args[0].element), tokenTypes::fp_reg, args[1].element);
		}

		return OK;
	}
	else if (inst == "div") {
		if (args.size() != 2)
			return WRONGNARGS;

		if (args[0].type != tokenTypes::reg && args[0].type != tokenTypes::fp_reg)
			return ARGV_ERROR;

		if (args[0].element == "sr" || args[0].element == "dr" || args[0].element == "cr")
			{ return ARGV_ERROR; /* Not yet implemented */ }
		else if (args[1].type != tokenTypes::reg && args[1].type != tokenTypes::fp_reg) {
			if (args[1].type != tokenTypes::unsigned_n && args[1].type != tokenTypes::signed_n && args[1].type != tokenTypes::double_n)
				return ARGV_ERROR; // Not possible

			if (args[1].type != tokenTypes::double_n) {
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
			else {
				if (args[0].element != "rfpr3") {
					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr3");

					pushAction(out_actions, virtual_actions::setRFPR3, tokenTypes::double_n, args[1].element);
					pushAction(out_actions, fetch_div_input.at(args[0].element), tokenTypes::fp_reg, "rfpr3");

					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::popFP, tokenTypes::fp_reg, "rfpr3");
				}
				else {
					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr2");

					pushAction(out_actions, virtual_actions::setRFPR2, tokenTypes::double_n, args[1].element);
					pushAction(out_actions, fetch_div_input.at(args[0].element), tokenTypes::fp_reg, "rfpr2");

					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::popFP, tokenTypes::fp_reg, "rfpr2");
				}
			}
		}
		else {
			if (args[0].type == tokenTypes::reg)
				pushAction(out_actions, fetch_div_input.at(args[0].element), tokenTypes::reg, args[1].element);
			else
				pushAction(out_actions, fetch_div_input.at(args[0].element), tokenTypes::fp_reg, args[1].element);
		}

		return OK;
	}
	else if (inst == "add") {
		if (args.size() != 2)
			return WRONGNARGS;

		if (args[0].type != tokenTypes::reg && args[0].type != tokenTypes::fp_reg)
			return ARGV_ERROR;

		if (args[0].element == "sr" || args[0].element == "cr")
			{ return ARGV_ERROR; /* Not yet implemented */ }
		else if (args[1].type != tokenTypes::reg && args[1].type != tokenTypes::fp_reg) {
			if (args[1].type != tokenTypes::unsigned_n && args[1].type != tokenTypes::signed_n && args[1].type != tokenTypes::double_n)
				return ARGV_ERROR; // Not possible

			if (args[1].type != tokenTypes::double_n) {
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
			else {
				if (args[0].element != "rfpr3") {
					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr3");

					pushAction(out_actions, virtual_actions::setRFPR3, tokenTypes::double_n, args[1].element);
					pushAction(out_actions, fetch_add_input.at(args[0].element), tokenTypes::fp_reg, "rfpr3");

					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::popFP, tokenTypes::fp_reg, "rfpr3");
				}
				else {
					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr2");

					pushAction(out_actions, virtual_actions::setRFPR2, tokenTypes::double_n, args[1].element);
					pushAction(out_actions, fetch_add_input.at(args[0].element), tokenTypes::fp_reg, "rfpr2");

					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::popFP, tokenTypes::fp_reg, "rfpr2");
				}
			}
		}
		else {
			if (args[0].type == tokenTypes::reg)
				pushAction(out_actions, fetch_add_input.at(args[0].element), tokenTypes::reg, args[1].element);
			else
				pushAction(out_actions, fetch_add_input.at(args[0].element), tokenTypes::fp_reg, args[1].element);
		}

		return OK;
	}
	else if (inst == "sub") {
		if (args.size() != 2)
			return WRONGNARGS;

		if (args[0].type != tokenTypes::reg && args[0].type != tokenTypes::fp_reg)
			return ARGV_ERROR;

		if (args[0].element == "sr" || args[0].element == "dr" || args[0].element == "cr")
			{ return ARGV_ERROR; /* Not yet implemented */ }
		else if (args[1].type != tokenTypes::reg && args[1].type != tokenTypes::fp_reg) {
			if (args[1].type != tokenTypes::unsigned_n && args[1].type != tokenTypes::signed_n && args[1].type != tokenTypes::double_n)
				return ARGV_ERROR; // Not possible

			if (args[1].type != tokenTypes::double_n) {
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
			else {
				if (args[0].element != "rfpr3") {
					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr3");

					pushAction(out_actions, virtual_actions::setRFPR3, tokenTypes::double_n, args[1].element);
					pushAction(out_actions, fetch_sub_input.at(args[0].element), tokenTypes::fp_reg, "rfpr3");

					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::popFP, tokenTypes::fp_reg, "rfpr3");
				}
				else {
					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr2");

					pushAction(out_actions, virtual_actions::setRFPR2, tokenTypes::double_n, args[1].element);
					pushAction(out_actions, fetch_sub_input.at(args[0].element), tokenTypes::fp_reg, "rfpr2");

					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::popFP, tokenTypes::fp_reg, "rfpr2");
				}
			}
		}
		else {
			if (args[0].type == tokenTypes::reg)
				pushAction(out_actions, fetch_sub_input.at(args[0].element), tokenTypes::reg, args[1].element);
			else
				pushAction(out_actions, fetch_sub_input.at(args[0].element), tokenTypes::fp_reg, args[1].element);
		}

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

		if (args[0].element == "sr")
			return ARGV_ERROR; // No purpose of doing that

		if (args[0].element == "cr") // Not yet implemented
			return OK;

		pushAction(out_actions, virtual_actions::_not, tokenTypes::reg, args[0].element);

		return OK;
	}
	else if (inst == "and") {
		if (args.size() != 2)
			return WRONGNARGS;

		if (args[0].type != tokenTypes::reg)
			return ARGV_ERROR;

		if (args[0].element == "sr" || args[1].element == "sr")
			return ARGV_ERROR; // No purpose of doing that

		if (args[0].element == "cr" || args[1].element == "cr") // Not yet implemented
			return OK;

		if (args[1].type != tokenTypes::reg) {
			if (args[1].type != tokenTypes::unsigned_n)
				return ARGV_ERROR;

			if (args[0].element == "rax") {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");

				pushAction(out_actions, virtual_actions::setRBX, tokenTypes::unsigned_n, args[1].element);
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");
				pushAction(out_actions, virtual_actions::_and, tokenTypes::reg, "rax");

				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rbx");
			}
			else {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");

				pushAction(out_actions, virtual_actions::setRAX, tokenTypes::unsigned_n, args[1].element);
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");
				pushAction(out_actions, virtual_actions::_and, tokenTypes::reg, args[0].element);

				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax");
			}
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

		if (args[0].element == "sr" || args[1].element == "sr")
			return ARGV_ERROR; // No purpose of doing that

		if (args[0].element == "cr" || args[1].element == "cr") // Not yet implemented
			return OK;

		if (args[1].type != tokenTypes::reg) {
			if (args[1].type != tokenTypes::unsigned_n)
				return ARGV_ERROR;

			if (args[0].element == "rax") {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");

				pushAction(out_actions, virtual_actions::setRBX, tokenTypes::unsigned_n, args[1].element);
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");
				pushAction(out_actions, virtual_actions::_xor, tokenTypes::reg, "rax");

				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rbx");
			}
			else {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");

				pushAction(out_actions, virtual_actions::setRAX, tokenTypes::unsigned_n, args[1].element);
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");
				pushAction(out_actions, virtual_actions::_xor, tokenTypes::reg, args[0].element);

				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax");
			}
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

		if (args[0].element == "sr" || args[1].element == "sr")
			return ARGV_ERROR; // No purpose of doing that

		if (args[0].element == "cr" || args[1].element == "cr")  // Not yet implemented			
			return OK;

		if (args[1].type != tokenTypes::reg) {
			if (args[1].type != tokenTypes::unsigned_n)
				return ARGV_ERROR;

			if (args[0].element == "rax") {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");

				pushAction(out_actions, virtual_actions::setRBX, tokenTypes::unsigned_n, args[1].element);
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");
				pushAction(out_actions, virtual_actions::_or, tokenTypes::reg, "rax");

				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rbx");
			}
			else {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");

				pushAction(out_actions, virtual_actions::setRAX, tokenTypes::unsigned_n, args[1].element);
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");
				pushAction(out_actions, virtual_actions::_or, tokenTypes::reg, args[0].element);

				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax");
			}
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

		if (args[0].element == "sr" || args[1].element == "sr")
			return ARGV_ERROR; // Won't probably be implemented (or maybe in a long time)

		if (args[0].element == "cr" || args[1].element == "cr") // Not yet implemented
			return OK;

		if (args[1].type != tokenTypes::reg) {
			if (args[1].type != tokenTypes::unsigned_n)
				return ARGV_ERROR;

			if (args[0].element == "rax") {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");

				pushAction(out_actions, virtual_actions::setRBX, tokenTypes::unsigned_n, args[1].element);
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");
				pushAction(out_actions, virtual_actions::_shr, tokenTypes::reg, "rax");

				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rbx");
			}
			else {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");

				pushAction(out_actions, virtual_actions::setRAX, tokenTypes::unsigned_n, args[1].element);
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");
				pushAction(out_actions, virtual_actions::_shr, tokenTypes::reg, args[0].element);

				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax");
			}
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

		if (args[0].element == "sr" || args[1].element == "sr")
			return ARGV_ERROR; // Won't probably be implemented (or maybe in a long time)

		if (args[0].element == "cr" || args[1].element == "cr") // Not yet implemented
			return OK;

		if (args[1].type != tokenTypes::reg) {
			if (args[1].type != tokenTypes::unsigned_n)
				return ARGV_ERROR;

			if (args[0].element == "rax") {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");

				pushAction(out_actions, virtual_actions::setRBX, tokenTypes::unsigned_n, args[1].element);
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");
				pushAction(out_actions, virtual_actions::_shl, tokenTypes::reg, "rax");

				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rbx");
			}
			else {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");

				pushAction(out_actions, virtual_actions::setRAX, tokenTypes::unsigned_n, args[1].element);
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");
				pushAction(out_actions, virtual_actions::_shl, tokenTypes::reg, args[0].element);

				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax");
			}
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

		if (args[0].type != tokenTypes::reg && args[0].type != tokenTypes::fp_reg)
			return ARGV_ERROR;

		if (args[0].element == "sr" || args[0].element == "cr")
			return OK; // No purpose of doing that
		else if (args[0].type == tokenTypes::fp_reg)
			pushAction(out_actions, virtual_actions::_dlog, tokenTypes::fp_reg, args[0].element);
		else
			pushAction(out_actions, virtual_actions::_log, tokenTypes::reg, args[0].element);

		return OK;
	}
	else if (inst == "log2") {
		if (args.size() != 1)
			return WRONGNARGS;

		if (args[0].type != tokenTypes::reg && args[0].type != tokenTypes::fp_reg)
			return ARGV_ERROR;

		if (args[0].element == "sr" || args[0].element == "cr")
			return ARGV_ERROR; // No purpose of doing that
		else if (args[0].type == tokenTypes::fp_reg)
			pushAction(out_actions, virtual_actions::_dlog2, tokenTypes::fp_reg, args[0].element);
		else
			pushAction(out_actions, virtual_actions::_log2, tokenTypes::reg, args[0].element);

		return OK;
	}
	else if (inst == "log10") {
		if (args.size() != 1)
			return WRONGNARGS;

		if (args[0].type != tokenTypes::reg && args[0].type != tokenTypes::fp_reg)
			return ARGV_ERROR;

		if (args[0].element == "sr" || args[0].element == "cr")
			return ARGV_ERROR; // No purpose of doing that
		else if (args[0].type == tokenTypes::fp_reg)
			pushAction(out_actions, virtual_actions::_dlog10, tokenTypes::fp_reg, args[0].element);
		else
			pushAction(out_actions, virtual_actions::_log10, tokenTypes::reg, args[0].element);

		return OK;
	}
	else if (inst == "pow") {
		if (args.size() != 2)
			return WRONGNARGS;

		if (args[0].type != tokenTypes::reg && args[0].type != tokenTypes::fp_reg)
			return ARGV_ERROR;

		if (args[0].element == "sr" || args[0].element == "cr" || args[1].element == "sr" || args[1].element == "cr")
			return ARGV_ERROR; // No purpose of doing that

		else if (args[0].type == tokenTypes::fp_reg) {
			if (args[1].type != tokenTypes::double_n && args[1].type != tokenTypes::fp_reg)
				return ARGV_ERROR; // Not possible
			else if (args[1].type == tokenTypes::fp_reg) {
				pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, args[1].element);
				pushAction(out_actions, virtual_actions::_dpow, tokenTypes::fp_reg, args[0].element);
			}
			else {
				if (args[0].element != "rfpr3") {
					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr3");

					pushAction(out_actions, virtual_actions::setRFPR3, tokenTypes::double_n, args[1].element);
					pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr3");
					pushAction(out_actions, virtual_actions::_dpow, tokenTypes::fp_reg, args[0].element);

					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::popFP, tokenTypes::fp_reg, "rpfr3");
				}
				else {
					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr2");

					pushAction(out_actions, virtual_actions::setRFPR2, tokenTypes::double_n, args[1].element);
					pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr2");
					pushAction(out_actions, virtual_actions::_dpow, tokenTypes::fp_reg, "rfpr3");

					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::popFP, tokenTypes::fp_reg, "rpfr2");
				}
			}
		}
		else {
			if (args[1].type != tokenTypes::unsigned_n && args[1].type != tokenTypes::reg && args[1].type != tokenTypes::fp_reg)
				return ARGV_ERROR; // Not possible;
			else if (args[1].type != tokenTypes::unsigned_n) {
				if (args[1].type == tokenTypes::reg) {
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, args[1].element);
					pushAction(out_actions, virtual_actions::_pow, tokenTypes::reg, args[0].element);
				}
				else {
					if (args[1].element.starts_with("rfpr")) {
						pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, args[1].element);
						pushAction(out_actions, virtual_actions::_int, tokenTypes::unsigned_n, "13");
						pushAction(out_actions, virtual_actions::_pow, tokenTypes::reg, args[0].element);
					}
					else {
						if (!unsafe_flag)
							pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr3");

						pushAction(out_actions, virtual_actions::movRFPR3, tokenTypes::fp_reg, args[1].element);
						pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr3");
						pushAction(out_actions, virtual_actions::_int, tokenTypes::unsigned_n, "13");
						pushAction(out_actions, virtual_actions::_pow, tokenTypes::reg, args[0].element);

						if (!unsafe_flag)
							pushAction(out_actions, virtual_actions::popFP, tokenTypes::fp_reg, "rfpr3");
					}
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
	if (args.size() != 1 && args.size() != 2) {
		if (args[0].type != tokenTypes::type_quantifier)
			return ARGV_ERROR;
	}

	if (args[0].type == tokenTypes::label || args[0].type == tokenTypes::str) // Str is not enough stable to be processed in this case
		return ARGV_ERROR;

	if (args[0].type == tokenTypes::reg || args[0].type == tokenTypes::fp_reg) {
		if (inst == "push") {
			if (args[0].element == "sr")
				pushAction(out_actions, virtual_actions::pushSR, tokenTypes::reg, "");
			else if (args[0].element == "cr")
				pushAction(out_actions, virtual_actions::pushCR, tokenTypes::reg, "");
			else if (args[0].type == tokenTypes::fp_reg)
				pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, args[0].element);
			else
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, args[0].element);
		}
		else if (inst == "pop") {
			if (args[0].element == "sr")
				pushAction(out_actions, virtual_actions::popSR, tokenTypes::reg, "");
			else if (args[0].element == "cr")
				pushAction(out_actions, virtual_actions::popCR, tokenTypes::reg, "");
			else if (args[0].type == tokenTypes::fp_reg)
				pushAction(out_actions, virtual_actions::popFP, tokenTypes::fp_reg, args[0].element);
			else
				pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, args[0].element);
		}
	}
	else if (args[0].type != tokenTypes::type_quantifier && args[0].type != tokenTypes::stored_addr_raw 
			&& args[0].type != tokenTypes::stored_addr_reg) {
		if (inst == "push") {
			if (args[0].type == tokenTypes::unsigned_n || args[0].type == tokenTypes::signed_n) { // Assuming x is 1*sizeof(size_t)
				if (!unsafe_flag) {
					pushAction(out_actions, virtual_actions::subRSP, tokenTypes::unsigned_n, std::to_string(sizeof(size_t))); // RSP - x
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax"); // RSP - 2x
					pushAction(out_actions, virtual_actions::addRSP, tokenTypes::unsigned_n, std::to_string(2 * sizeof(size_t))); // RSP
				}

				pushAction(out_actions, virtual_actions::setRAX, args[0].type, args[0].element);
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax"); // RSP - x

				if (!unsafe_flag) {
					pushAction(out_actions, virtual_actions::subRSP, tokenTypes::unsigned_n, std::to_string(sizeof(size_t))); // RSP - 2x
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax"); // RSP - x => wanted result
				}
			}
			else if (args[0].type == tokenTypes::double_n) { // Assuming x is 1*sizeof(long double)
				if (!unsafe_flag) {
					pushAction(out_actions, virtual_actions::subRSP, tokenTypes::unsigned_n, std::to_string(sizeof(long double))); // RSP - x
					pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr3"); // RSP - 2x
					pushAction(out_actions, virtual_actions::addRSP, tokenTypes::unsigned_n, std::to_string(2 * sizeof(long double))); // RSP
				}

				pushAction(out_actions, virtual_actions::setRFPR3, tokenTypes::double_n, args[0].element);
				pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr3");

				if (!unsafe_flag) {
					pushAction(out_actions, virtual_actions::subRSP, tokenTypes::unsigned_n, std::to_string(sizeof(long double))); // RSP - 2x
					pushAction(out_actions, virtual_actions::popFP, tokenTypes::fp_reg, "rfpr3"); // RSP - x
				}
			}
		}
		else if (inst == "pop") { // Not possible			
			return ARGV_ERROR;
		}
	}
	else if (args[0].type == tokenTypes::type_quantifier) {
		if (args[1].type != tokenTypes::reg && args[1].type != tokenTypes::stored_addr_raw && args[1].type != tokenTypes::stored_addr_reg)
			return ARGV_ERROR;

		if (inst == "push") {
			if (args[1].type == tokenTypes::reg)
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, args[1].element, SVOPTTOTQ(args[0].element));
			else if (args[1].type == tokenTypes::stored_addr_raw) {
				// Using this syntax shoudl be avoided, unless you are in unsafe mode, because that could generate a lot of instructions
				// So if you could move the address into a register before, do it then

				if (!unsafe_flag) {
					pushAction(out_actions, virtual_actions::subRSP, tokenTypes::unsigned_n, std::to_string(SVOPTTOTS(args[0].element)));
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");
					pushAction(out_actions, virtual_actions::addRSP, tokenTypes::unsigned_n, 
						std::to_string(sizeof(size_t) + SVOPTTOTS(args[0].element)));
				}

				pushAction(out_actions, virtual_actions::setRAX, tokenTypes::unsigned_n, args[1].element);
				pushAction(out_actions, virtual_actions::movgm, tokenTypes::reg, "rax", SVOPTTOTQ(args[0].element));

				if (!unsafe_flag) {
					pushAction(out_actions, virtual_actions::subRSP, tokenTypes::unsigned_n, std::to_string(sizeof(size_t)));
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax");
				}
			}
			else if (args[1].type == tokenTypes::stored_addr_reg) {
				const std::string reg_addr = args[1].element.substr(1, args[1].element.size() - 2);
				pushAction(out_actions, virtual_actions::movgm, tokenTypes::reg, reg_addr, SVOPTTOTQ(args[0].element));
			}
		}
		else if (inst == "pop") {
			if (args[1].type == tokenTypes::reg)
				pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, args[1].element, SVOPTTOTQ(args[0].element));
			else if (args[1].type == tokenTypes::stored_addr_raw) {
				if (!unsafe_flag) {
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");
					pushAction(out_actions, virtual_actions::addRSP, tokenTypes::unsigned_n, std::to_string(sizeof(size_t)));
				}

				pushAction(out_actions, virtual_actions::setRAX, tokenTypes::unsigned_n, args[1].element);
				pushAction(out_actions, virtual_actions::movsm, tokenTypes::reg, "rax", SVOPTTOTQ(args[1].element));

				if (!unsafe_flag) {
					pushAction(out_actions, virtual_actions::subRSP, tokenTypes::unsigned_n, 
						std::to_string(SVOPTTOTS(args[0].element) + sizeof(size_t)));
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax");
					pushAction(out_actions, virtual_actions::addRSP, tokenTypes::unsigned_n, std::to_string(SVOPTTOTS(args[0].element)));
				}
			}
			else if (args[1].type == tokenTypes::stored_addr_reg) {
				const std::string reg_addr = args[1].element.substr(1, args[1].element.size() - 2);
				pushAction(out_actions, virtual_actions::movsm, tokenTypes::reg, reg_addr, SVOPTTOTQ(args[1].element));
			}
		}
	}
	else
		return ARGV_ERROR;

	return OK;
}
int preprocHeap(const std::string& inst, const std::vector<token>& args, std::vector<action>& out_actions) {
	if (inst == "mload") {
		if (args.size() != 2)
			return WRONGNARGS;

		if (args[0].type == tokenTypes::stored_addr_reg || args[0].type == tokenTypes::stored_addr_raw) {
			if (args[1].type == tokenTypes::reg || args[1].type == tokenTypes::fp_reg) {
				if (args[0].type == tokenTypes::stored_addr_reg) {
					if (args[1].element == "sr")
						pushAction(out_actions, virtual_actions::movsmSR, tokenTypes::reg, args[0].element.substr(1, args[0].element.size() - 2));
					else if (args[1].element == "cr")
						pushAction(out_actions, virtual_actions::movsmCR, tokenTypes::reg, args[0].element.substr(1, args[0].element.size() - 2));
					else if (args[1].type == tokenTypes::fp_reg) {
						std::string reg_addr = args[0].element.substr(1, args[0].element.size() - 2);
						if (reg_addr == "rax")
							pushAction(out_actions, virtual_actions::movsmFP, tokenTypes::fp_reg, args[1].element);
						else {
							if (!unsafe_flag)
								pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");

							pushAction(out_actions, virtual_actions::movRAX, tokenTypes::reg, reg_addr);
							pushAction(out_actions, virtual_actions::movsmFP, tokenTypes::fp_reg, args[1].element);

							if (!unsafe_flag)
								pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax");
						}
					}
					else {
						pushAction(out_actions, virtual_actions::push, tokenTypes::reg, args[1].element);
						pushAction(out_actions, virtual_actions::movsm, tokenTypes::reg, args[0].element.substr(1, args[0].element.size() - 2));
					}
				}
				else {
					if (args[1].element != "rax") {
						if (!unsafe_flag)
							pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");
						pushAction(out_actions, virtual_actions::setRAX, tokenTypes::unsigned_n, args[0].element);

						if (args[1].element == "sr")
							pushAction(out_actions, virtual_actions::movsmSR, tokenTypes::reg, "rax");
						else if (args[1].element == "cr")
							pushAction(out_actions, virtual_actions::movsmCR, tokenTypes::reg, "rax");
						else if (args[1].type == tokenTypes::fp_reg)
							pushAction(out_actions, virtual_actions::movsmFP, tokenTypes::fp_reg, args[1].element);
						else {
							pushAction(out_actions, virtual_actions::push, tokenTypes::reg, args[1].element);
							pushAction(out_actions, virtual_actions::movsm, tokenTypes::reg, "rax");
						}

						if (!unsafe_flag)
							pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax");
					}
					else {
						if (!unsafe_flag)
							pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");
						pushAction(out_actions, virtual_actions::setRBX, tokenTypes::unsigned_n, args[0].element);

						// If args[1].element == "rax" then the only possibility of movsm is that one
						pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");
						pushAction(out_actions, virtual_actions::movsm, tokenTypes::reg, "rbx");

						if (args[1].element == "sr")
							pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rbx");
					}
				}
			}
			else if (args[1].type == tokenTypes::unsigned_n || args[1].type == tokenTypes::signed_n) {
				if (args[0].type == tokenTypes::stored_addr_reg) {
					if (args[0].element == "[rax]") {
						if (!unsafe_flag)
							pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");
						pushAction(out_actions, virtual_actions::setRBX, args[1].type, args[1].element);

						pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");
						pushAction(out_actions, virtual_actions::movsm, tokenTypes::reg, "rax");

						if (!unsafe_flag)
							pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rbx");
					}
					else {
						if (!unsafe_flag)
							pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");
						pushAction(out_actions, virtual_actions::setRAX, args[1].type, args[1].element);

						pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");
						pushAction(out_actions, virtual_actions::movsm, tokenTypes::reg, args[0].element.substr(1, args[0].element.size() - 2));

						if (!unsafe_flag)
							pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax");
					}
				}
				else {
					if (!unsafe_flag) {
						pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");
						pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");
					}
					pushAction(out_actions, virtual_actions::setRAX, tokenTypes::unsigned_n, args[0].element);
					pushAction(out_actions, virtual_actions::setRBX, args[1].type, args[1].element);

					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");
					pushAction(out_actions, virtual_actions::movsm, tokenTypes::reg, "rax");

					if (!unsafe_flag) {
						pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rbx");
						pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax");
					}
				}
			}
			else if (args[1].type == tokenTypes::double_n) {
				if (args[0].type == tokenTypes::stored_addr_reg) {
					if (args[0].element == "[rax]") {
						if (!unsafe_flag)
							pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr3");

						pushAction(out_actions, virtual_actions::setRFPR3, tokenTypes::double_n, args[1].element);
						pushAction(out_actions, virtual_actions::movsmFP, tokenTypes::fp_reg, "rfpr3");

						if (!unsafe_flag)
							pushAction(out_actions, virtual_actions::popFP, tokenTypes::fp_reg, "rfpr3");
					}
					else {
						if (!unsafe_flag) {
							pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr3");
							pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");
						}

						pushAction(out_actions, virtual_actions::movRAX, tokenTypes::reg, args[0].element.substr(1, args[0].element.size() - 2));
						pushAction(out_actions, virtual_actions::setRFPR3, tokenTypes::double_n, args[1].element);
						pushAction(out_actions, virtual_actions::movsmFP, tokenTypes::fp_reg, "rfpr3");

						if (!unsafe_flag) {
							pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax");
							pushAction(out_actions, virtual_actions::popFP, tokenTypes::fp_reg, "rfpr3");
						}
					}
				}
				else {
					if (!unsafe_flag) {
						pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr3");
						pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");
					}

					pushAction(out_actions, virtual_actions::setRAX, tokenTypes::unsigned_n, args[0].element);
					pushAction(out_actions, virtual_actions::setRFPR3, tokenTypes::double_n, args[1].element);
					pushAction(out_actions, virtual_actions::movsmFP, tokenTypes::fp_reg, "rfpr3");

					if (!unsafe_flag) {
						pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax");
						pushAction(out_actions, virtual_actions::popFP, tokenTypes::fp_reg, "rfpr3");
					}
				}
			}
		}
		else if ((args[0].type == tokenTypes::reg || args[0].type == tokenTypes::fp_reg) 
				&& (args[1].type == tokenTypes::stored_addr_reg || args[1].type == tokenTypes::stored_addr_raw)) {
			if (args[1].type == tokenTypes::stored_addr_reg) {
				if (args[0].element == "sr")
					pushAction(out_actions, virtual_actions::movgmSR, tokenTypes::reg, args[1].element.substr(1, args[1].element.size() - 2));
				else if (args[0].element == "cr")
					pushAction(out_actions, virtual_actions::movgmCR, tokenTypes::reg, args[1].element.substr(1, args[1].element.size() - 2));
				else if (args[0].type == tokenTypes::fp_reg) {
					std::string reg_addr = args[1].element.substr(1, args[1].element.size() - 2);
					if (reg_addr == "rax")
						pushAction(out_actions, virtual_actions::movgmFP, tokenTypes::fp_reg, args[0].element);
					else {
						if (!unsafe_flag)
							pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");

						pushAction(out_actions, virtual_actions::movRAX, tokenTypes::reg, reg_addr);
						pushAction(out_actions, virtual_actions::movgmFP, tokenTypes::fp_reg, args[0].element);

						if (!unsafe_flag)
							pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax");
					}
				}
				else {
					pushAction(out_actions, virtual_actions::movgm, tokenTypes::reg, args[1].element.substr(1, args[1].element.size() - 2));
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, args[0].element);
				}
			}
			else {
				if (args[0].element != "rax") {
					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");
					pushAction(out_actions, virtual_actions::setRAX, tokenTypes::unsigned_n, args[1].element);

					if (args[0].element == "sr")
						pushAction(out_actions, virtual_actions::movgmSR, tokenTypes::reg, "rax");
					else if (args[0].element == "cr")
						pushAction(out_actions, virtual_actions::movgmCR, tokenTypes::reg, "rax");
					else if (args[0].type == tokenTypes::fp_reg)
						pushAction(out_actions, virtual_actions::movgmFP, tokenTypes::fp_reg, args[0].element);
					else {
						pushAction(out_actions, virtual_actions::movgm, tokenTypes::reg, "rax");
						pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, args[0].element);
					}

					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax");
				}
				else {
					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");
					pushAction(out_actions, virtual_actions::setRBX, tokenTypes::unsigned_n, args[1].element);

					pushAction(out_actions, virtual_actions::movgm, tokenTypes::reg, "rbx");
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax");

					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rbx");
				}
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

	if (inst == "je")
		preprocJmpCmpCalls(out_actions, virtual_actions::je, args[0]);
	else if (inst == "jne")
		preprocJmpCmpCalls(out_actions, virtual_actions::jne, args[0]);
	else if (inst == "jl")
		preprocJmpCmpCalls(out_actions, virtual_actions::jl, args[0]);
	else if (inst == "jle")
		preprocJmpCmpCalls(out_actions, virtual_actions::jle, args[0]);
	else if (inst == "jg")
		preprocJmpCmpCalls(out_actions, virtual_actions::jg, args[0]);
	else if (inst == "jge")
		preprocJmpCmpCalls(out_actions, virtual_actions::jge, args[0]);

	return OK;
}
int preprocComps(const std::vector<token>& args, std::vector<action>& out_actions) {
	if (args.size() != 2)
		return WRONGNARGS;

	if ((args[0].type != tokenTypes::reg && args[0].type != tokenTypes::fp_reg) 
			|| (args[1].type == tokenTypes::stored_addr_reg || args[1].type == tokenTypes::stored_addr_raw))
		return ARGV_ERROR;

	if (num_registers.find(args[0].element) != num_registers.end()) {
		if ((args[1].element == "sr" && args[1].type == tokenTypes::reg) || args[1].type == tokenTypes::str) {
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
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::pushSR, tokenTypes::reg, "");

				pushAction(out_actions, virtual_actions::setSR, tokenTypes::str, args[1].element);
				pushAction(out_actions, virtual_actions::pushSR, tokenTypes::reg, "");

				pushAction(out_actions, virtual_actions::toString, tokenTypes::reg, args[0].element);
				pushAction(out_actions, virtual_actions::pushSR, tokenTypes::reg, "");

				pushAction(out_actions, virtual_actions::cmpstr, tokenTypes::reg, "");

				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::popSR, tokenTypes::reg, "");
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
		else if (args[1].type == tokenTypes::fp_reg || args[1].type == tokenTypes::double_n) {
			if (args[1].type == tokenTypes::fp_reg) {
				pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, args[1].element);
				pushAction(out_actions, virtual_actions::_int, tokenTypes::unsigned_n, "13");
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, args[0].element);
				pushAction(out_actions, virtual_actions::cmp, tokenTypes::reg, "");
			}
			else {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr3");
				pushAction(out_actions, virtual_actions::setRFPR3, tokenTypes::double_n, args[1].element);

				pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr3");
				pushAction(out_actions, virtual_actions::_int, tokenTypes::unsigned_n, "13");
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, args[0].element);
				pushAction(out_actions, virtual_actions::cmp, tokenTypes::reg, "");

				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::popFP, tokenTypes::fp_reg, "rfpr3");
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
				if (!unsafe_flag) {
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax"); // Save rax
					pushAction(out_actions, virtual_actions::pushCR, tokenTypes::reg, ""); // Save cr
				}

				pushAction(out_actions, virtual_actions::setCR, tokenTypes::str, args[1].element);
				CRToNum(out_actions, true);
				getFirstCharSR(out_actions);

				// Do comparison
				pushAction(out_actions, virtual_actions::cmp, tokenTypes::reg, "");

				if (!unsafe_flag) {
					pushAction(out_actions, virtual_actions::popCR, tokenTypes::reg, ""); // Restore cr
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax"); // Restore rax
				}
			}
		}
		else if (args[1].type == tokenTypes::fp_reg || args[1].type == tokenTypes::double_n) {
			if (args[1].type == tokenTypes::fp_reg) {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::pushSR, tokenTypes::reg, ""); // Save sr

				pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, args[1].element);
				pushAction(out_actions, virtual_actions::_int, tokenTypes::unsigned_n, "12");
				pushAction(out_actions, virtual_actions::pushSR, tokenTypes::reg, "");

				pushAction(out_actions, virtual_actions::pushSR, tokenTypes::reg, "");

				pushAction(out_actions, virtual_actions::cmpstr, tokenTypes::reg, "");

				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::popSR, tokenTypes::reg, ""); // Restore sr
			}
			else {
				if (!unsafe_flag) {
					pushAction(out_actions, virtual_actions::pushSR, tokenTypes::reg, ""); // Save sr
					pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr3");
				}
				
				pushAction(out_actions, virtual_actions::setRFPR3, tokenTypes::double_n, args[1].element);
				pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rpfr3");
				pushAction(out_actions, virtual_actions::_int, tokenTypes::unsigned_n, "12");
				pushAction(out_actions, virtual_actions::pushSR, tokenTypes::reg, "");

				pushAction(out_actions, virtual_actions::pushSR, tokenTypes::reg, "");

				pushAction(out_actions, virtual_actions::cmpstr, tokenTypes::reg, "");

				if (!unsafe_flag) {
					pushAction(out_actions, virtual_actions::popFP, tokenTypes::fp_reg, "rfpr3");
					pushAction(out_actions, virtual_actions::popSR, tokenTypes::reg, ""); // Save sr
				}
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
				pushAction(out_actions, virtual_actions::setRAX, tokenTypes::unsigned_n, std::to_string(sizeof(size_t) - 1));
				pushAction(out_actions, virtual_actions::movsmCR, tokenTypes::reg, "rax");
				pushAction(out_actions, virtual_actions::popCR, tokenTypes::reg, ""); // Restore cr

				// Fetch first 8 bytes of memory and leave them on stack
				pushAction(out_actions, virtual_actions::setRAX, tokenTypes::unsigned_n, "0");
				pushAction(out_actions, virtual_actions::movgm, tokenTypes::reg, "rax");

				CRToNum(out_actions, true); // Convert CR to ULL and leave its value on stack

				pushAction(out_actions, virtual_actions::cmp, tokenTypes::reg, "");

				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax"); // Restore rax
			}
			else {
				if (!unsafe_flag) {
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax"); // Save rax
					pushAction(out_actions, virtual_actions::pushSR, tokenTypes::reg, "");
				}

				pushAction(out_actions, virtual_actions::pushCR, tokenTypes::reg, ""); // Save cr
				pushAction(out_actions, virtual_actions::setSR, tokenTypes::str, args[1].element);

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

				CRToNum(out_actions, true); // Convert CR to ULL and leave its value on stack

				pushAction(out_actions, virtual_actions::cmp, tokenTypes::reg, "");

				if (!unsafe_flag) {
					pushAction(out_actions, virtual_actions::popSR, tokenTypes::reg, "");
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax"); // Restore rax
				}
			}
		}
		else if (args[1].type == tokenTypes::fp_reg || args[1].type == tokenTypes::double_n) {
			return ARGV_ERROR; // No real purpose of doing that comparison
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

				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, args[1].element);

				CRToNum(out_actions, true);

				pushAction(out_actions, virtual_actions::cmp, tokenTypes::reg, "");

				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax"); // Restore rax
			}
		}
	}
	else if (args[0].type == tokenTypes::fp_reg) {
		if (args[1].element == "sr" || args[1].type == tokenTypes::str) {
			if (args[1].type == tokenTypes::reg) {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::pushSR, tokenTypes::reg, ""); // Save sr

				pushAction(out_actions, virtual_actions::pushSR, tokenTypes::reg, "");

				pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, args[0].element);
				pushAction(out_actions, virtual_actions::_int, tokenTypes::unsigned_n, "12");
				pushAction(out_actions, virtual_actions::pushSR, tokenTypes::reg, "");

				pushAction(out_actions, virtual_actions::cmpstr, tokenTypes::reg, "");

				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::popSR, tokenTypes::reg, ""); // Restore sr
			}
			else {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::pushSR, tokenTypes::reg, ""); // Save sr

				pushAction(out_actions, virtual_actions::setSR, tokenTypes::str, args[1].element);
				pushAction(out_actions, virtual_actions::pushSR, tokenTypes::reg, "");

				pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, args[0].element);
				pushAction(out_actions, virtual_actions::_int, tokenTypes::unsigned_n, "12");
				pushAction(out_actions, virtual_actions::pushSR, tokenTypes::reg, "");

				pushAction(out_actions, virtual_actions::cmpstr, tokenTypes::reg, "");

				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::popSR, tokenTypes::reg, ""); // Restore sr
			}
		}
		else if (args[1].element == "cr") {
			return ARGV_ERROR; // No real meaning of doing that comparison
		}
		else if (args[1].type == tokenTypes::double_n) {
			if (args[0].element == "rfpr3") {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr2");

				pushAction(out_actions, virtual_actions::setRFPR2, tokenTypes::double_n, args[1].element);
				pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr2");
				pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr3");
				pushAction(out_actions, virtual_actions::cmpdbl, tokenTypes::reg, "");

				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::popFP, tokenTypes::reg, "rfpr2");
			}
			else {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr3");

				pushAction(out_actions, virtual_actions::setRFPR3, tokenTypes::double_n, args[1].element);
				pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr3");
				pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, args[0].element);
				pushAction(out_actions, virtual_actions::cmpdbl, tokenTypes::reg, "");

				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::popFP, tokenTypes::reg, "rfpr3");
			}
		}
		else {
			if (args[1].type == tokenTypes::reg) {
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, args[1].element);

				pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, args[0].element);
				pushAction(out_actions, virtual_actions::_int, tokenTypes::unsigned_n, "13");
				pushAction(out_actions, virtual_actions::cmp, tokenTypes::reg, "");
			}
			else {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");

				pushAction(out_actions, virtual_actions::setRAX, tokenTypes::unsigned_n, args[1].element);
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");

				pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, args[0].element);
				pushAction(out_actions, virtual_actions::_int, tokenTypes::unsigned_n, "13");
				pushAction(out_actions, virtual_actions::cmp, tokenTypes::reg, "");

				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");
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
		auto tpl = std::static_pointer_cast<svasm_args>(_a.getValuePtr());
		auto[vtype, vval, vopt] = *tpl;
		if (vtype == tokenTypes::label) {
			vval = std::to_string(preprocLabels[vval]);
			*tpl = std::make_tuple(tokenTypes::unsigned_n, vval, vopt);
		}
	}

	unsafe_flag = false;

	return OK;
}

int compileInst(action& raw_action, std::vector<byte>& out_bytes) {
	static const std::unordered_map<std::string, registries_def> storeg = {
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
		{"rdx", registries_def::RDX},
		{"rbp", registries_def::RBP},
		{"rsp", registries_def::RSP}
	};
	static const std::unordered_map<std::string, extra_registries> xstoreg = {
		{"fpr0", extra_registries::FPR0},
		{"fpr1", extra_registries::FPR1},
		{"fpr2", extra_registries::FPR2},
		{"fpr3", extra_registries::FPR3},

		{"efpr0", extra_registries::EFPR0},
		{"efpr1", extra_registries::EFPR1},
		{"efpr2", extra_registries::EFPR2},
		{"efpr3", extra_registries::EFPR3},

		{"rfpr0", extra_registries::RFPR0},
		{"rfpr1", extra_registries::RFPR1},
		{"rfpr2", extra_registries::RFPR2},
		{"rfpr3", extra_registries::RFPR3},
	};

	const byte comp_action = instructions_set[raw_action.getAction()];
	const auto v_ptr = *std::static_pointer_cast<svasm_args>(raw_action.getValuePtr());

	auto [v_type, v_raw, v_opt] = *std::static_pointer_cast<svasm_args>(raw_action.getValuePtr());
	out_bytes.push_back(comp_action);

	if (opt_arg_ops.find(comp_action) != opt_arg_ops.end())
		out_bytes.push_back(v_opt);

	if (zero_args_opcodes.find(comp_action) != zero_args_opcodes.end())		
		return OK;
	else if (uint64_args_opcodes.find(comp_action) != uint64_args_opcodes.end()) {
		if (v_type != tokenTypes::signed_n && v_type != tokenTypes::unsigned_n)
			return ARGV_ERROR;

		if (v_type == tokenTypes::unsigned_n) {
			const size_t value = std::stoull(v_raw);
			byte* uc_n = nullptr;

			ULLTOA(value, &uc_n);
			const size_t compressed_len = COMPBA(uc_n, sizeof(size_t));

			for (byte i = 0; i < compressed_len; ++i)
				out_bytes.push_back(uc_n[i]);

			delete[] uc_n;
			return OK;
		}
		else if (v_type == tokenTypes::signed_n) {
			const long long value = std::stoll(v_raw);
			byte* uc_n = nullptr;

			ULLTOA((size_t)value, &uc_n);
			const size_t compressed_len = COMPBA(uc_n, sizeof(size_t));

			for (byte i = 0; i < compressed_len; ++i)
				out_bytes.push_back(uc_n[i]);

			delete[] uc_n;
			return OK;
		}
	}
	else if (comp_action == instructions_set[virtual_actions::setSR]) {
		if (v_type != tokenTypes::str)
			return ARGV_ERROR;

		v_raw = v_raw.substr(1, v_raw.size() - 2);
		size_t str_size = v_raw.size() + 1;

		byte* b_str_size = nullptr;
		ULLTOA(str_size, &b_str_size);

		size_t compressed_len = COMPBA(b_str_size, sizeof(size_t));

		for (byte i = 0; i < compressed_len; ++i)
			out_bytes.push_back(b_str_size[i]);
		delete[] b_str_size;

		byte* b_str = new byte[str_size];
#if defined(ISWIN)
		memcpy_s(b_str, str_size, v_raw.c_str(), str_size);
#else
		std::memcpy(b_str, v_raw.c_str(), str_size);
#endif

		for (size_t i = 0; i < str_size; ++i)
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

		out_bytes.push_back(c);

		return OK;
	}
	else if (reg_args_opcodes.find(comp_action) != reg_args_opcodes.end()) {
		if (v_type != tokenTypes::reg && v_type != tokenTypes::fp_reg)
			return ARGV_ERROR;

		byte comp_reg;

		if (v_type == tokenTypes::reg)
			comp_reg = registers_set[storeg.at(v_raw)];
		else
			comp_reg = fp_registers_set[xstoreg.at(v_raw)];

		out_bytes.push_back(comp_reg);
		return OK;
	}
	else if (parted_opcodes.find(comp_action) != parted_opcodes.end()) {
		if (comp_action == instructions_set[virtual_actions::setFPR0]) {// All set(E|R|)FPRs have the same first opc
			if (v_type != tokenTypes::double_n)
				return ARGV_ERROR;

			out_bytes.push_back(map_FPR_set_2nd_opc[raw_action.getAction()]);

			auto uc_a = std::make_unique<uint8_t[]>(sizeof(long double));
			long double d = std::stold(v_raw);
			mp_memcpy(&d, uc_a.get(), sizeof(long double));

			for (size_t i = 0; i < sizeof(long double); i++)
				out_bytes.push_back(uc_a[i]);
		}
		else if (comp_action == instructions_set[virtual_actions::movFPR0]) // All mov(E|R|)FPRs have the same first opc
			ASMBL_FP_NMATHS(out_bytes, map_FPR_mov_2nd_opc, raw_action, fp_registers_set[xstoreg.at(v_raw)]);
		else if (comp_action == instructions_set[virtual_actions::mulFPR0])
			ASMBL_FP_NMATHS(out_bytes, map_FPR_mul_2nd_opc, raw_action, fp_registers_set[xstoreg.at(v_raw)]);
		else if (comp_action == instructions_set[virtual_actions::divFPR0])
			ASMBL_FP_NMATHS(out_bytes, map_FPR_div_2nd_opc, raw_action, fp_registers_set[xstoreg.at(v_raw)]);
		else if (comp_action == instructions_set[virtual_actions::addFPR0])
			ASMBL_FP_NMATHS(out_bytes, map_FPR_add_2nd_opc, raw_action, fp_registers_set[xstoreg.at(v_raw)]);
		else if (comp_action == instructions_set[virtual_actions::subFPR0])
			ASMBL_FP_NMATHS(out_bytes, map_FPR_sub_2nd_opc, raw_action, fp_registers_set[xstoreg.at(v_raw)]);
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
		ULLTOA((size_t)0x100000, &req_mem_size); // Default value

	for (size_t i = 0; i < sizeof(size_t); i++)
		out_bytes.emplace_back(req_mem_size[i]);
	delete[] req_mem_size;


	for (action raw_a : raw_actions) {
		if (compileInst(raw_a, out_bytes))
			return ARGV_ERROR;
	}

	return OK;
}