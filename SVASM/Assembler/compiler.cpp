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
#include "util.h"

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
static const std::unordered_map<std::string, comn_registers> strreg_to_reg = {
	{"ax", comn_registers::AX},
	{"bx", comn_registers::BX},
	{"cx", comn_registers::CX},
	{"dx", comn_registers::DX},

	{"eax", comn_registers::EAX},
	{"ebx", comn_registers::EBX},
	{"ecx", comn_registers::ECX},
	{"edx", comn_registers::EDX},

	{"rax", comn_registers::RAX},
	{"rbx", comn_registers::RBX},
	{"rcx", comn_registers::RCX},
	{"rdx", comn_registers::RDX},

	{"rbp", comn_registers::RBP},
	{"rsp", comn_registers::RSP},
	{"rdi", comn_registers::RDI},
	{"rsi", comn_registers::RSI},

	{"sr", comn_registers::SR},
	{"cr", comn_registers::CR},

	{"fpr0", comn_registers::FPR0},
	{"fpr1", comn_registers::FPR1},
	{"fpr2", comn_registers::FPR2},
	{"fpr3", comn_registers::FPR3},

	{"efpr0", comn_registers::EFPR0},
	{"efpr1", comn_registers::EFPR1},
	{"efpr2", comn_registers::EFPR2},
	{"efpr3", comn_registers::EFPR3},

	{"rfpr0", comn_registers::RFPR0},
	{"rfpr1", comn_registers::RFPR1},
	{"rfpr2", comn_registers::RFPR2},
	{"rfpr3", comn_registers::RFPR3}
};

static bool unsafe_flag = false;
struct {
	bool mem_res_req = false;
	size_t new_mem_size = 0;
} memory_flags;

namespace {
	uint64_t cur_line;

	void pushAction(std::vector<action>& out_actions, const virtual_actions& vaction,
			const tokenTypes& var_type, const std::string& var_raw, uint8_t opt_arg = 0) {
		out_actions.emplace_back(action(vaction, std::make_shared<svasm_args>(var_type, var_raw, opt_arg)));
	}
	inline void preprocJmpCmpCalls(std::vector<action>& out_actions, const virtual_actions& vaction,
			const token& arg) {
		if (arg.type != tokenTypes::label)
			pushAction(out_actions, vaction, arg.type, arg.element);
		else {
			pushAction(out_actions, vaction, tokenTypes::unsigned_n, "1");
			pushAction(out_actions, virtual_actions::jmp, tokenTypes::unsigned_n, "1");
			pushAction(out_actions, virtual_actions::lcall, tokenTypes::label, arg.element);
		}
	}

	inline void CRToNum(std::vector<action>& out_actions, const bool useRAX) {
		if (useRAX) {
			// Set first 8 bytes of memory to 0
			pushAction(out_actions, virtual_actions::set, tokenTypes::unsigned_n, "0", (uint8_t)comn_registers::RAX);
			pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");
			pushAction(out_actions, virtual_actions::movsm, tokenTypes::reg, "rax");
			// Set value of CR (1 byte) into 7th of memory
			pushAction(out_actions, virtual_actions::set, tokenTypes::unsigned_n, "7", (uint8_t)comn_registers::RAX);
			pushAction(out_actions, virtual_actions::movsmCR, tokenTypes::reg, "rax");
			// Fetch first 8 bytes of memory and leave them on stack
			pushAction(out_actions, virtual_actions::set, tokenTypes::unsigned_n, "0", (uint8_t)comn_registers::RAX);
			pushAction(out_actions, virtual_actions::movgm, tokenTypes::reg, "rax");
		}
		else { // Use rbx instead
			// Set first 8 bytes of memory to 0
			pushAction(out_actions, virtual_actions::set, tokenTypes::unsigned_n, "0", (uint8_t)comn_registers::RBX);
			pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");
			pushAction(out_actions, virtual_actions::movsm, tokenTypes::reg, "rbx");
			// Set value of CR (1 byte) into 7th of memory
			pushAction(out_actions, virtual_actions::set, tokenTypes::unsigned_n, "7", (uint8_t)comn_registers::RBX);
			pushAction(out_actions, virtual_actions::movsmCR, tokenTypes::reg, "rbx");
			// Fetch first 8 bytes of memory and leave them on stack
			pushAction(out_actions, virtual_actions::set, tokenTypes::unsigned_n, "0", (uint8_t)comn_registers::RBX);
			pushAction(out_actions, virtual_actions::movgm, tokenTypes::reg, "rbx");
		}
	}
	inline void getFirstCharSR(std::vector<action>& out_actions) {
		// Get first char of SR
		pushAction(out_actions, virtual_actions::set, tokenTypes::unsigned_n, "2", (uint8_t)comn_registers::RAX);
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
		assert_err_wrnargs(1, "int", args, cur_line);

	if (args[0].type != tokenTypes::unsigned_n)
		assert_err_argv({"unsigned_n"}, "int", args, cur_line);

	pushAction(out_actions, virtual_actions::_int, tokenTypes::unsigned_n, args[0].element);

	return OK;
}
int preprocMov(const std::vector<token>& args, std::vector<action>& out_actions) {
	if (args.size() != 2)
		assert_err_wrnargs(2, "mov", args, cur_line);

	if (args[0].type == tokenTypes::reg) {
		if (args[1].type != tokenTypes::reg)
			pushAction(out_actions, virtual_actions::set, args[1].type, args[1].element, (uint8_t)strreg_to_reg.at(args[0].element));
		else
			pushAction(out_actions, virtual_actions::mov, tokenTypes::reg, args[1].element, (uint8_t)strreg_to_reg.at(args[0].element));
	}
	else if (args[0].type == tokenTypes::fp_reg) {
		if (args[1].type != tokenTypes::fp_reg)
			pushAction(out_actions, virtual_actions::set, args[1].type, args[1].element, (uint8_t)strreg_to_reg.at(args[0].element));
		else
			pushAction(out_actions, virtual_actions::mov, tokenTypes::fp_reg, args[1].element, (uint8_t)strreg_to_reg.at(args[0].element));
	}
	else
		assert_err_argv({ "reg|fp_reg", "any" }, "mov", args, cur_line);


	return OK;
}
int preprocStr(const std::vector<token>& args, std::vector<action>& out_actions) {
	if (args.size() != 1)
		assert_err_wrnargs(1, "str", args, cur_line);

	if (args[0].type != tokenTypes::reg && args[0].type != tokenTypes::fp_reg)
		assert_err_argv({ "reg|fp_reg" }, "str", args, cur_line);

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
		assert_err_wrnargs(1, "inc", args, cur_line);

	if (args[0].type != tokenTypes::reg && args[0].type != tokenTypes::fp_reg)
		assert_err_argv({ "reg|fp_reg" }, "inc", args, cur_line);

	if (args[0].element == "sr")
		assert_err_reguse("inc", args, cur_line);

	// Not yet implemented
	if (args[0].element == "cr")
		assert_err_nonimpl("inc", args, cur_line);
	else if (args[0].type == tokenTypes::fp_reg)
		pushAction(out_actions, virtual_actions::inc, tokenTypes::fp_reg, args[0].element);
	else
		pushAction(out_actions, virtual_actions::inc, tokenTypes::reg, args[0].element);
	
	return OK;
}
int preprocDec(const std::vector<token>& args, std::vector<action>& out_actions) {
	if (args.size() != 1)
		assert_err_wrnargs(1, "dec", args, cur_line);

	if (args[0].type != tokenTypes::reg && args[0].type != tokenTypes::fp_reg)
		assert_err_argv({ "reg|fp_reg" }, "dec", args, cur_line);

	if (args[0].element == "sr")
		assert_err_reguse("dec", args, cur_line);

	// Not yet implemented
	if (args[0].element == "cr")
		assert_err_nonimpl("dec", args, cur_line);
	else if (args[0].type == tokenTypes::fp_reg)
		pushAction(out_actions, virtual_actions::dec, tokenTypes::fp_reg, args[0].element);
	else
		pushAction(out_actions, virtual_actions::dec, tokenTypes::reg, args[0].element);

	return OK;
}
int preprocMath(const std::string& inst, const std::vector<token>& args, std::vector<action>& out_actions) {
	if (args.size() != 2)
		assert_err_wrnargs(2, inst, args, cur_line);

	if (inst == "mul") {
		if (args[0].type != tokenTypes::reg && args[0].type != tokenTypes::fp_reg)
			assert_err_argv({ "reg|fp_reg", "any" }, "mul", args, cur_line);

		if (args[0].element == "sr" || args[0].element == "cr")
			assert_err_nonimpl("mul", args, cur_line);
		else if (args[1].type != tokenTypes::reg && args[1].type != tokenTypes::fp_reg) {
			if (args[1].type != tokenTypes::unsigned_n && args[1].type != tokenTypes::signed_n && args[1].type != tokenTypes::double_n)
				assert_err_argv({ "reg|fp_reg", "unsigned_n|signed_n|double_n" }, "mul", args, cur_line);

			if (args[1].type != tokenTypes::double_n) {
				if (args[0].element != "rax") {
					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");

					pushAction(out_actions, virtual_actions::set, args[1].type, args[1].element, (uint8_t)comn_registers::RAX);
					pushAction(out_actions, virtual_actions::mul, tokenTypes::reg, "rax", (uint8_t)strreg_to_reg.at(args[0].element));

					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax");
				}
				else {
					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");

					pushAction(out_actions, virtual_actions::set, args[1].type, args[1].element, (uint8_t)comn_registers::RBX);
					pushAction(out_actions, virtual_actions::mul, tokenTypes::reg, "rbx", (uint8_t)comn_registers::RAX);

					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rbx");
				}
			}
			else {
				if (args[0].element != "rfpr3") {
					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr3");

					pushAction(out_actions, virtual_actions::set, tokenTypes::double_n, args[1].element, (uint8_t)comn_registers::RFPR3);
					pushAction(out_actions, virtual_actions::mul, tokenTypes::fp_reg, "rfpr3", (uint8_t)strreg_to_reg.at(args[0].element));

					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::popFP, tokenTypes::fp_reg, "rfpr3");
				}
				else {
					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr2");

					pushAction(out_actions, virtual_actions::set, tokenTypes::double_n, args[1].element, (uint8_t)comn_registers::RFPR2);
					pushAction(out_actions, virtual_actions::mul, tokenTypes::fp_reg, "rfpr2", (uint8_t)comn_registers::RFPR3);

					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::popFP, tokenTypes::fp_reg, "rfpr2");
				}
			}
		}
		else {
			if (args[0].type == tokenTypes::reg)
				pushAction(out_actions, virtual_actions::mul, tokenTypes::reg, args[1].element, (uint8_t)strreg_to_reg.at(args[0].element));
			else
				pushAction(out_actions, virtual_actions::mul, tokenTypes::fp_reg, args[1].element, (uint8_t)strreg_to_reg.at(args[0].element));
		}

		return OK;
	}
	else if (inst == "div") {
		if (args[0].type != tokenTypes::reg && args[0].type != tokenTypes::fp_reg)
			assert_err_argv({ "reg|fp_reg", "any" }, "div", args, cur_line);

		if (args[0].element == "sr" || args[0].element == "cr")
			assert_err_nonimpl("div", args, cur_line);
		else if (args[1].type != tokenTypes::reg && args[1].type != tokenTypes::fp_reg) {
			if (args[1].type != tokenTypes::unsigned_n && args[1].type != tokenTypes::signed_n && args[1].type != tokenTypes::double_n)
				assert_err_argv({ "reg|fp_reg", "unsigned_n|signed_n|double_n" }, "div", args, cur_line);

			if (args[1].type != tokenTypes::double_n) {
				if (args[0].element != "rax") {
					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");

					pushAction(out_actions, virtual_actions::set, args[1].type, args[1].element, (uint8_t)comn_registers::RAX);
					pushAction(out_actions, virtual_actions::div, tokenTypes::reg, "rax", (uint8_t)strreg_to_reg.at(args[0].element));

					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax");
				}
				else {
					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");

					pushAction(out_actions, virtual_actions::set, args[1].type, args[1].element, (uint8_t)comn_registers::RBX);
					pushAction(out_actions, virtual_actions::div, tokenTypes::reg, "rbx", (uint8_t)comn_registers::RAX);

					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rbx");
				}
			}
			else {
				if (args[0].element != "rfpr3") {
					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr3");

					pushAction(out_actions, virtual_actions::set, tokenTypes::double_n, args[1].element, (uint8_t)comn_registers::RFPR3);
					pushAction(out_actions, virtual_actions::div, tokenTypes::fp_reg, "rfpr3", (uint8_t)strreg_to_reg.at(args[0].element));

					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::popFP, tokenTypes::fp_reg, "rfpr3");
				}
				else {
					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr2");

					pushAction(out_actions, virtual_actions::set, tokenTypes::double_n, args[1].element, (uint8_t)comn_registers::RFPR2);
					pushAction(out_actions, virtual_actions::div, tokenTypes::fp_reg, "rfpr2", (uint8_t)comn_registers::RFPR3);

					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::popFP, tokenTypes::fp_reg, "rfpr2");
				}
			}
		}
		else {
			if (args[0].type == tokenTypes::reg)
				pushAction(out_actions, virtual_actions::div, tokenTypes::reg, args[1].element, (uint8_t)strreg_to_reg.at(args[0].element));
			else
				pushAction(out_actions, virtual_actions::div, tokenTypes::fp_reg, args[1].element, (uint8_t)strreg_to_reg.at(args[0].element));
		}

		return OK;
	}
	else if (inst == "add") {
		if (args[0].type != tokenTypes::reg && args[0].type != tokenTypes::fp_reg)
			assert_err_argv({"reg|fp_reg", "any"}, "add", args, cur_line);

		if (args[0].element == "sr" || args[0].element == "cr")
			assert_err_nonimpl("add", args, cur_line);
		else if (args[1].type != tokenTypes::reg && args[1].type != tokenTypes::fp_reg) {
			if (args[1].type != tokenTypes::unsigned_n && args[1].type != tokenTypes::signed_n && args[1].type != tokenTypes::double_n)
				assert_err_argv({ "reg|fp_reg", "unsigned_n|signed_n|double_n" }, "add", args, cur_line);

			if (args[1].type != tokenTypes::double_n) {
				if (args[0].element != "rax") {
					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");

					pushAction(out_actions, virtual_actions::set, args[1].type, args[1].element, (uint8_t)comn_registers::RAX);
					pushAction(out_actions, virtual_actions::add, tokenTypes::reg, "rax", (uint8_t)strreg_to_reg.at(args[0].element));

					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax");
				}
				else {
					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");

					pushAction(out_actions, virtual_actions::set, args[1].type, args[1].element, (uint8_t)comn_registers::RBX);
					pushAction(out_actions, virtual_actions::add, tokenTypes::reg, "rbx", (uint8_t)comn_registers::RAX);

					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rbx");
				}
			}
			else {
				if (args[0].element != "rfpr3") {
					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr3");

					pushAction(out_actions, virtual_actions::set, tokenTypes::double_n, args[1].element, (uint8_t)comn_registers::RFPR3);
					pushAction(out_actions, virtual_actions::add, tokenTypes::fp_reg, "rfpr3", (uint8_t)strreg_to_reg.at(args[0].element));

					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::popFP, tokenTypes::fp_reg, "rfpr3");
				}
				else {
					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr2");

					pushAction(out_actions, virtual_actions::set, tokenTypes::double_n, args[1].element, (uint8_t)comn_registers::RFPR2);
					pushAction(out_actions, virtual_actions::add, tokenTypes::fp_reg, "rfpr2", (uint8_t)comn_registers::RFPR3);

					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::popFP, tokenTypes::fp_reg, "rfpr2");
				}
			}
		}
		else {
			if (args[0].type == tokenTypes::reg)
				pushAction(out_actions, virtual_actions::add, tokenTypes::reg, args[1].element, (uint8_t)strreg_to_reg.at(args[0].element));
			else
				pushAction(out_actions, virtual_actions::add, tokenTypes::fp_reg, args[1].element, (uint8_t)strreg_to_reg.at(args[0].element));
		}

		return OK;
	}
	else if (inst == "sub") {
		if (args[0].type != tokenTypes::reg && args[0].type != tokenTypes::fp_reg)
			assert_err_argv({ "reg|fp_reg", "any" }, "sub", args, cur_line);

		if (args[0].element == "sr" || args[0].element == "cr")
			assert_err_nonimpl("sub", args, cur_line);
		else if (args[1].type != tokenTypes::reg && args[1].type != tokenTypes::fp_reg) {
			if (args[1].type != tokenTypes::unsigned_n && args[1].type != tokenTypes::signed_n && args[1].type != tokenTypes::double_n)
				assert_err_argv({ "reg|fp_reg", "unsigned_n|signed_n|double_n" }, "sub", args, cur_line);

			if (args[1].type != tokenTypes::double_n) {
				if (args[0].element != "rax") {
					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");

					pushAction(out_actions, virtual_actions::set, args[1].type, args[1].element, (uint8_t)comn_registers::RAX);
					pushAction(out_actions, virtual_actions::sub, tokenTypes::reg, "rax", (uint8_t)strreg_to_reg.at(args[0].element));

					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax");
				}
				else {
					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");

					pushAction(out_actions, virtual_actions::set, args[1].type, args[1].element, (uint8_t)comn_registers::RBX);
					pushAction(out_actions, virtual_actions::sub, tokenTypes::reg, "rbx", (uint8_t)comn_registers::RAX);

					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rbx");
				}
			}
			else {
				if (args[0].element != "rfpr3") {
					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr3");

					pushAction(out_actions, virtual_actions::set, tokenTypes::double_n, args[1].element, (uint8_t)comn_registers::RFPR3);
					pushAction(out_actions, virtual_actions::sub, tokenTypes::fp_reg, "rfpr3", (uint8_t)strreg_to_reg.at(args[0].element));

					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::popFP, tokenTypes::fp_reg, "rfpr3");
				}
				else {
					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr2");

					pushAction(out_actions, virtual_actions::set, tokenTypes::double_n, args[1].element, (uint8_t)comn_registers::RFPR2);
					pushAction(out_actions, virtual_actions::sub, tokenTypes::fp_reg, "rfpr2", (uint8_t)comn_registers::RFPR3);

					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::popFP, tokenTypes::fp_reg, "rfpr2");
				}
			}
		}
		else {
			if (args[0].type == tokenTypes::reg)
				pushAction(out_actions, virtual_actions::sub, tokenTypes::reg, args[1].element, (uint8_t)strreg_to_reg.at(args[0].element));
			else
				pushAction(out_actions, virtual_actions::sub, tokenTypes::fp_reg, args[1].element, (uint8_t)strreg_to_reg.at(args[0].element));
		}

		return OK;
	}

	return OK;
}
int preprocLgclMath(const std::string& inst, const std::vector<token>& args, std::vector<action>& out_actions) {
	if (inst == "not") {
		if (args.size() != 1)
			assert_err_wrnargs(1, "not", args, cur_line);

		if (args[0].type != tokenTypes::reg)
			assert_err_argv({ "reg" }, "not", args, cur_line);

		if (args[0].element == "sr")
			assert_err_reguse("not", args, cur_line);

		if (args[0].element == "cr") // Not yet implemented
			assert_err_nonimpl("not", args, cur_line);

		pushAction(out_actions, virtual_actions::_not, tokenTypes::reg, args[0].element);

		return OK;
	}
	else if (inst == "and") {
		if (args.size() != 2)
			assert_err_wrnargs(2, "and", args, cur_line);

		if (args[0].type != tokenTypes::reg)
			assert_err_argv({ "reg", "any"}, "and", args, cur_line);

		if (args[0].element == "sr" || args[1].element == "sr")
			assert_err_reguse("and", args, cur_line);

		if (args[0].element == "cr" || args[1].element == "cr") // Not yet implemented
			assert_err_nonimpl("and", args, cur_line);

		if (args[1].type != tokenTypes::reg) {
			if (args[1].type != tokenTypes::unsigned_n)
				assert_err_argv({ "reg", "unsigned_n" }, "and", args, cur_line);

			if (args[0].element == "rax") {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");

				pushAction(out_actions, virtual_actions::set, tokenTypes::unsigned_n, args[1].element, (uint8_t)comn_registers::RBX);
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");
				pushAction(out_actions, virtual_actions::_and, tokenTypes::reg, "rax");

				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rbx");
			}
			else {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");

				pushAction(out_actions, virtual_actions::set, tokenTypes::unsigned_n, args[1].element, (uint8_t)comn_registers::RAX);
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
			assert_err_wrnargs(2, "xor", args, cur_line);

		if (args[0].type != tokenTypes::reg)
			assert_err_argv({ "reg", "any" }, "xor", args, cur_line);

		if (args[0].element == "sr" || args[1].element == "sr")
			assert_err_reguse("xor", args, cur_line);

		if (args[0].element == "cr" || args[1].element == "cr") // Not yet implemented
			assert_err_nonimpl("xor", args, cur_line);

		if (args[1].type != tokenTypes::reg) {
			if (args[1].type != tokenTypes::unsigned_n)
				assert_err_argv({ "reg", "unsigned_n" }, "xor", args, cur_line);

			if (args[0].element == "rax") {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");

				pushAction(out_actions, virtual_actions::set, tokenTypes::unsigned_n, args[1].element, (uint8_t)comn_registers::RBX);
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");
				pushAction(out_actions, virtual_actions::_xor, tokenTypes::reg, "rax");

				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rbx");
			}
			else {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");

				pushAction(out_actions, virtual_actions::set, tokenTypes::unsigned_n, args[1].element, (uint8_t)comn_registers::RAX);
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
			assert_err_wrnargs(2, "or", args, cur_line);

		if (args[0].type != tokenTypes::reg)
			assert_err_argv({ "reg", "any" }, "or", args, cur_line);

		if (args[0].element == "sr" || args[1].element == "sr")
			assert_err_reguse("or", args, cur_line);

		if (args[0].element == "cr" || args[1].element == "cr")  // Not yet implemented			
			assert_err_nonimpl("or", args, cur_line);

		if (args[1].type != tokenTypes::reg) {
			if (args[1].type != tokenTypes::unsigned_n)
				assert_err_argv({ "reg", "unsigned_n" }, "or", args, cur_line);

			if (args[0].element == "rax") {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");

				pushAction(out_actions, virtual_actions::set, tokenTypes::unsigned_n, args[1].element, (uint8_t)comn_registers::RBX);
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");
				pushAction(out_actions, virtual_actions::_or, tokenTypes::reg, "rax");

				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rbx");
			}
			else {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");

				pushAction(out_actions, virtual_actions::set, tokenTypes::unsigned_n, args[1].element, (uint8_t)comn_registers::RAX);
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
			assert_err_wrnargs(2, "shr", args, cur_line);

		if (args[0].type != tokenTypes::reg)
			assert_err_argv({ "reg", "any" }, "shr", args, cur_line);

		if (args[0].element == "sr" || args[1].element == "sr")
			assert_err_reguse("shr", args, cur_line);

		if (args[0].element == "cr" || args[1].element == "cr") // Not yet implemented
			assert_err_reguse("shr", args, cur_line);

		if (args[1].type != tokenTypes::reg) {
			if (args[1].type != tokenTypes::unsigned_n)
				assert_err_argv({ "reg", "unsigned_n" }, "shr", args, cur_line);

			if (args[0].element == "rax") {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");

				pushAction(out_actions, virtual_actions::set, tokenTypes::unsigned_n, args[1].element, (uint8_t)comn_registers::RBX);
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");
				pushAction(out_actions, virtual_actions::_shr, tokenTypes::reg, "rax");

				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rbx");
			}
			else {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");

				pushAction(out_actions, virtual_actions::set, tokenTypes::unsigned_n, args[1].element, (uint8_t)comn_registers::RAX);
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
			assert_err_wrnargs(2, "shl", args, cur_line);

		if (args[0].type != tokenTypes::reg)
			assert_err_argv({ "reg", "any" }, "shl", args, cur_line);

		if (args[0].element == "sr" || args[1].element == "sr")
			assert_err_reguse("shl", args, cur_line);

		if (args[0].element == "cr" || args[1].element == "cr") // Not yet implemented
			assert_err_nonimpl("shl", args, cur_line);

		if (args[1].type != tokenTypes::reg) {
			if (args[1].type != tokenTypes::unsigned_n)
				assert_err_argv({ "reg", "unsigned_n" }, "shl", args, cur_line);

			if (args[0].element == "rax") {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");

				pushAction(out_actions, virtual_actions::set, tokenTypes::unsigned_n, args[1].element, (uint8_t)comn_registers::RBX);
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");
				pushAction(out_actions, virtual_actions::_shl, tokenTypes::reg, "rax");

				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rbx");
			}
			else {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");

				pushAction(out_actions, virtual_actions::set, tokenTypes::unsigned_n, args[1].element, (uint8_t)comn_registers::RAX);
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
			assert_err_wrnargs(1, "log", args, cur_line);

		if (args[0].type != tokenTypes::reg && args[0].type != tokenTypes::fp_reg)
			assert_err_argv({ "reg|fp_reg" }, "log", args, cur_line);

		if (args[0].element == "sr" || args[0].element == "cr")
			assert_err_reguse("log", args, cur_line);
		else if (args[0].type == tokenTypes::fp_reg)
			pushAction(out_actions, virtual_actions::_dlog, tokenTypes::fp_reg, args[0].element);
		else
			pushAction(out_actions, virtual_actions::_log, tokenTypes::reg, args[0].element);

		return OK;
	}
	else if (inst == "log2") {
		if (args.size() != 1)
			assert_err_wrnargs(1, "log2", args, cur_line);

		if (args[0].type != tokenTypes::reg && args[0].type != tokenTypes::fp_reg)
			assert_err_argv({ "reg|fp_reg" }, "log2", args, cur_line);

		if (args[0].element == "sr" || args[0].element == "cr")
			assert_err_reguse("log2", args, cur_line);
		else if (args[0].type == tokenTypes::fp_reg)
			pushAction(out_actions, virtual_actions::_dlog2, tokenTypes::fp_reg, args[0].element);
		else
			pushAction(out_actions, virtual_actions::_log2, tokenTypes::reg, args[0].element);

		return OK;
	}
	else if (inst == "log10") {
		if (args.size() != 1)
			assert_err_wrnargs(1, "log10", args, cur_line);

		if (args[0].type != tokenTypes::reg && args[0].type != tokenTypes::fp_reg)
			assert_err_argv({ "reg|fp_reg" }, "log10", args, cur_line);

		if (args[0].element == "sr" || args[0].element == "cr")
			assert_err_reguse("log10", args, cur_line);
		else if (args[0].type == tokenTypes::fp_reg)
			pushAction(out_actions, virtual_actions::_dlog10, tokenTypes::fp_reg, args[0].element);
		else
			pushAction(out_actions, virtual_actions::_log10, tokenTypes::reg, args[0].element);

		return OK;
	}
	else if (inst == "pow") {
		if (args.size() != 2)
			assert_err_wrnargs(2, "pow", args, cur_line);

		if (args[0].type != tokenTypes::reg && args[0].type != tokenTypes::fp_reg)
			assert_err_argv({ "reg|fp_reg", "any" }, "pow", args, cur_line);

		if (args[0].element == "sr" || args[0].element == "cr" || args[1].element == "sr" || args[1].element == "cr")
			assert_err_reguse("pow", args, cur_line);

		else if (args[0].type == tokenTypes::fp_reg) {
			if (args[1].type != tokenTypes::double_n && args[1].type != tokenTypes::fp_reg)
				assert_err_argv({ "fp_reg", "double_n|fp_reg" }, "pow", args, cur_line);
			else if (args[1].type == tokenTypes::fp_reg) {
				pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, args[1].element);
				pushAction(out_actions, virtual_actions::_dpow, tokenTypes::fp_reg, args[0].element);
			}
			else {
				if (args[0].element != "rfpr3") {
					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr3");

					pushAction(out_actions, virtual_actions::set, tokenTypes::double_n, args[1].element, (uint8_t)comn_registers::RFPR3);
					pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr3");
					pushAction(out_actions, virtual_actions::_dpow, tokenTypes::fp_reg, args[0].element);

					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::popFP, tokenTypes::fp_reg, "rpfr3");
				}
				else {
					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr2");

					pushAction(out_actions, virtual_actions::set, tokenTypes::double_n, args[1].element, (uint8_t)comn_registers::RFPR2);
					pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr2");
					pushAction(out_actions, virtual_actions::_dpow, tokenTypes::fp_reg, "rfpr3");

					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::popFP, tokenTypes::fp_reg, "rpfr2");
				}
			}
		}
		else {
			if (args[1].type != tokenTypes::unsigned_n && args[1].type != tokenTypes::reg && args[1].type != tokenTypes::fp_reg)
				assert_err_argv({ "reg", "unsigned_n|reg|fp_reg" }, "pow", args, cur_line);
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

						pushAction(out_actions, virtual_actions::set, tokenTypes::fp_reg, args[1].element, (uint8_t)comn_registers::RFPR3);
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

					pushAction(out_actions, virtual_actions::set, tokenTypes::unsigned_n, args[1].element, (uint8_t)comn_registers::RBX);
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");
					pushAction(out_actions, virtual_actions::_pow, tokenTypes::reg, args[0].element);

					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rbx");
				}
				else {
					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");

					pushAction(out_actions, virtual_actions::set, tokenTypes::unsigned_n, args[1].element, (uint8_t)comn_registers::RAX);
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
	if (args.size() == 1) {
		if (args[0].type == tokenTypes::type_quantifier || args[0].type == tokenTypes::label || args[0].type == tokenTypes::str) {
			if (inst == "push")
				assert_err_argv({ "reg|fp_reg|imm_addr|reg_addr|data_type(except str)" }, "push", args, cur_line);
			else
				assert_err_argv({ "reg|fp_reg" }, "pop", args, cur_line);
		}
	}
	else if (args.size() == 2) {
		if (args[0].type != tokenTypes::type_quantifier || args[1].type == tokenTypes::label || args[1].type == tokenTypes::str)
			assert_err_argv({ "type_quantifier", "reg|fp_reg|imm_addr|reg_addr" }, inst, args, cur_line);
	}
	else
		assert_err("Wrong numbers of arguments (only 1 (or 2 if type quantifier used) expected): ", inst, args, cur_line, WRONGNARGS);

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
					pushAction(out_actions, virtual_actions::sub, tokenTypes::unsigned_n, std::to_string(sizeof(size_t)),
						(uint8_t)comn_registers::RSP); // RSP - x
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax"); // RSP - 2x
					pushAction(out_actions, virtual_actions::add, tokenTypes::unsigned_n, std::to_string(2 * sizeof(size_t)),
						(uint8_t)comn_registers::RSP); // RSP
				}

				pushAction(out_actions, virtual_actions::set, args[0].type, args[0].element, (uint8_t)comn_registers::RAX);
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax"); // RSP - x

				if (!unsafe_flag) {
					pushAction(out_actions, virtual_actions::sub, tokenTypes::unsigned_n, std::to_string(sizeof(size_t)),
						(uint8_t)comn_registers::RSP); // RSP - 2x
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax"); // RSP - x => wanted result
				}
			}
			else if (args[0].type == tokenTypes::double_n) { // Assuming x is 1*sizeof(long double)
				if (!unsafe_flag) {
					pushAction(out_actions, virtual_actions::sub, tokenTypes::unsigned_n, std::to_string(sizeof(long double)),
						(uint8_t)comn_registers::RSP); // RSP - x
					pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr3"); // RSP - 2x
					pushAction(out_actions, virtual_actions::add, tokenTypes::unsigned_n, std::to_string(2 * sizeof(long double)),
						(uint8_t)comn_registers::RSP); // RSP
				}

				pushAction(out_actions, virtual_actions::set, tokenTypes::double_n, args[0].element, (uint8_t)comn_registers::RFPR3);
				pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr3");

				if (!unsafe_flag) {
					pushAction(out_actions, virtual_actions::sub, tokenTypes::unsigned_n, std::to_string(sizeof(long double)),
						(uint8_t)comn_registers::RSP); // RSP - 2x
					pushAction(out_actions, virtual_actions::popFP, tokenTypes::fp_reg, "rfpr3"); // RSP - x
				}
			}
		}
		else if (inst == "pop") { // Not possible			
			assert_err_argv({ "reg|fp_reg" }, "pop", args, cur_line);
		}
	}
	else if (args[0].type == tokenTypes::type_quantifier) {
		if (args[1].type != tokenTypes::reg && args[1].type != tokenTypes::stored_addr_raw && args[1].type != tokenTypes::stored_addr_reg)
			assert_err_argv({ "type_quantifier", "reg|imm_addr|reg_addr" }, inst, args, cur_line);

		if (inst == "push") {
			if (args[1].type == tokenTypes::reg)
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, args[1].element, SVOPTTOTQ(args[0].element));
			else if (args[1].type == tokenTypes::stored_addr_raw) {
				// Using this syntax shoudl be avoided, unless you are in unsafe mode, because that could generate a lot of instructions
				// So if you could move the address into a register before, do it then

				if (!unsafe_flag) {
					pushAction(out_actions, virtual_actions::sub, tokenTypes::unsigned_n, std::to_string(SVOPTTOTS(args[0].element)),
						(uint8_t)comn_registers::RSP);
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");
					pushAction(out_actions, virtual_actions::add, tokenTypes::unsigned_n, 
						std::to_string(sizeof(size_t) + SVOPTTOTS(args[0].element)), (uint8_t)comn_registers::RSP);
				}

				pushAction(out_actions, virtual_actions::set, tokenTypes::unsigned_n, args[1].element, (uint8_t)comn_registers::RAX);
				pushAction(out_actions, virtual_actions::movgm, tokenTypes::reg, "rax", SVOPTTOTQ(args[0].element));

				if (!unsafe_flag) {
					pushAction(out_actions, virtual_actions::sub, tokenTypes::unsigned_n, std::to_string(sizeof(size_t)),
						(uint8_t)comn_registers::RSP);
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
					pushAction(out_actions, virtual_actions::add, tokenTypes::unsigned_n, std::to_string(sizeof(size_t)),
						(uint8_t)comn_registers::RSP);
				}

				pushAction(out_actions, virtual_actions::set, tokenTypes::unsigned_n, args[1].element, (uint8_t)comn_registers::RAX);
				pushAction(out_actions, virtual_actions::movsm, tokenTypes::reg, "rax", SVOPTTOTQ(args[1].element));

				if (!unsafe_flag) {
					pushAction(out_actions, virtual_actions::sub, tokenTypes::unsigned_n, 
						std::to_string(SVOPTTOTS(args[0].element) + sizeof(size_t)), (uint8_t)comn_registers::RSP);
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax");
					pushAction(out_actions, virtual_actions::add, tokenTypes::unsigned_n, std::to_string(SVOPTTOTS(args[0].element)),
						(uint8_t)comn_registers::RSP);
				}
			}
			else if (args[1].type == tokenTypes::stored_addr_reg) {
				const std::string reg_addr = args[1].element.substr(1, args[1].element.size() - 2);
				pushAction(out_actions, virtual_actions::movsm, tokenTypes::reg, reg_addr, SVOPTTOTQ(args[1].element));
			}
		}
	}
	else {
		if (args.size() == 1) {
			if (inst == "push")
				assert_err_argv({ "reg|fp_reg|imm_addr|reg_addr|data_type(except str)" }, "push", args, cur_line);
			else
				assert_err_argv({ "reg|fp_reg" }, "pop", args, cur_line);
		}
		else if (args.size() == 2) {
			assert_err_argv({ "type_quantifier", "reg|fp_reg|imm_addr|reg_addr" }, inst, args, cur_line);
		}
	}

	return OK;
}
int preprocHeap(const std::string& inst, const std::vector<token>& args, std::vector<action>& out_actions) {
	if (inst == "mload") {
		if (args.size() != 2)
			assert_err_wrnargs(2, "mload", args, cur_line);

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

							pushAction(out_actions, virtual_actions::mov, tokenTypes::reg, reg_addr, (uint8_t)comn_registers::RAX);
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
						pushAction(out_actions, virtual_actions::set, tokenTypes::unsigned_n, args[0].element, (uint8_t)comn_registers::RAX);

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
						pushAction(out_actions, virtual_actions::set, tokenTypes::unsigned_n, args[0].element, (uint8_t)comn_registers::RBX);

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
						pushAction(out_actions, virtual_actions::set, args[1].type, args[1].element, (uint8_t)comn_registers::RBX);

						pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");
						pushAction(out_actions, virtual_actions::movsm, tokenTypes::reg, "rax");

						if (!unsafe_flag)
							pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rbx");
					}
					else {
						if (!unsafe_flag)
							pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");
						pushAction(out_actions, virtual_actions::set, args[1].type, args[1].element, (uint8_t)comn_registers::RAX);

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
					pushAction(out_actions, virtual_actions::set, tokenTypes::unsigned_n, args[0].element, (uint8_t)comn_registers::RAX);
					pushAction(out_actions, virtual_actions::set, args[1].type, args[1].element, (uint8_t)comn_registers::RBX);

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

						pushAction(out_actions, virtual_actions::set, tokenTypes::double_n, args[1].element, (uint8_t)comn_registers::RFPR3);
						pushAction(out_actions, virtual_actions::movsmFP, tokenTypes::fp_reg, "rfpr3");

						if (!unsafe_flag)
							pushAction(out_actions, virtual_actions::popFP, tokenTypes::fp_reg, "rfpr3");
					}
					else {
						if (!unsafe_flag) {
							pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr3");
							pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");
						}

						pushAction(out_actions, virtual_actions::mov, tokenTypes::reg, args[0].element.substr(1, args[0].element.size() - 2), (uint8_t)comn_registers::RAX);
						pushAction(out_actions, virtual_actions::set, tokenTypes::double_n, args[1].element, (uint8_t)comn_registers::RFPR3);
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

					pushAction(out_actions, virtual_actions::set, tokenTypes::unsigned_n, args[0].element, (uint8_t)comn_registers::RAX);
					pushAction(out_actions, virtual_actions::set, tokenTypes::double_n, args[1].element, (uint8_t)comn_registers::RFPR3);
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

						pushAction(out_actions, virtual_actions::mov, tokenTypes::reg, reg_addr, (uint8_t)comn_registers::RAX);
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
					pushAction(out_actions, virtual_actions::set, tokenTypes::unsigned_n, args[1].element, (uint8_t)comn_registers::RAX);

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
					pushAction(out_actions, virtual_actions::set, tokenTypes::unsigned_n, args[1].element, (uint8_t)comn_registers::RBX);

					pushAction(out_actions, virtual_actions::movgm, tokenTypes::reg, "rbx");
					pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rax");

					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rbx");
				}
			}
		}
		else {
			std::cerr << "Incorrect usage of mload: " << std::endl;
			std::cerr << "mload <imm_addr|reg_addr>, <reg|fp_reg|unsigned_n|signed_n|double_n>" << std::endl;
			std::cerr << "mload <reg|fp_reg>, <imm_addr|reg_addr>" << std::endl;
			assert_err("Your usage of mload: ", "mov", args, cur_line, ARGV_ERROR);
		}

		return OK;
	}
	else if (inst == "sdzs") {
		if (args.size() != 1)
			assert_err_wrnargs(1, "sdzs", args, cur_line);

		if (args[0].type != tokenTypes::reg)
			assert_err_argv({ "reg" }, "sdzs", args, cur_line);

		pushAction(out_actions, virtual_actions::sdzs, tokenTypes::reg, args[0].element);
	}

	return OK;
}

int preprocCalls(const std::string& inst, const std::vector<token>& args, std::vector<action>& out_actions) {
	if (args.size() != 1) {
		if (args.size() == 0 || args.empty()) {
			if (inst == "ret")
				pushAction(out_actions, virtual_actions::ret, tokenTypes::reg, "");
			else
				assert_err_wrnargs(1, inst, args, cur_line);

			return OK;
		}
		else
			assert_err_wrnargs(1, inst, args, cur_line);
	}

	if (args[0].type != tokenTypes::label && args[0].type != tokenTypes::unsigned_n && args[0].type != tokenTypes::signed_n)
		assert_err_argv({ "label|unsigned_n|signed_n" }, inst, args, cur_line);

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
		else
			assert_err_argv({ "label|unsigned_n" }, "call", args, cur_line);
	}
	else if (inst == "jmp") {
		if (args[0].type == tokenTypes::label)
			pushAction(out_actions, virtual_actions::lcall, tokenTypes::label, args[0].element);
		else
			pushAction(out_actions, virtual_actions::jmp, args[0].type, args[0].element);
	}

	return OK;
}
int preprocCmpCalls(const std::string& inst, const std::vector<token>& args, std::vector<action>& out_actions) {
	if (args.size() != 1)
		assert_err_wrnargs(1, inst, args, cur_line);

	if (args[0].type != tokenTypes::label && args[0].type != tokenTypes::unsigned_n && args[0].type != tokenTypes::signed_n)
		assert_err_argv({ "label|unsigned_n|signed_n" }, inst, args, cur_line);

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
		assert_err_wrnargs(2, "cmp", args, cur_line);

	if ((args[0].type != tokenTypes::reg && args[0].type != tokenTypes::fp_reg)
		|| (args[1].type == tokenTypes::stored_addr_reg || args[1].type == tokenTypes::stored_addr_raw))
		assert_err_argv({ "reg|fp_reg", "not imm_addr nor reg_addr" }, "cmp", args, cur_line);

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

				pushAction(out_actions, virtual_actions::set, tokenTypes::str, args[1].element, (uint8_t)comn_registers::SR);
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
				pushAction(out_actions, virtual_actions::set, tokenTypes::double_n, args[1].element, (uint8_t)comn_registers::RFPR3);

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

					pushAction(out_actions, virtual_actions::set, args[1].type, args[1].element, (uint8_t)comn_registers::RBX);
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, args[0].element);
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rbx");
					pushAction(out_actions, virtual_actions::cmp, tokenTypes::reg, "");

					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::pop, tokenTypes::reg, "rbx");
				}
				else {
					if (!unsafe_flag)
						pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");

					pushAction(out_actions, virtual_actions::set, args[1].type, args[1].element, (uint8_t)comn_registers::RAX);
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

				pushAction(out_actions, virtual_actions::set, tokenTypes::str, args[1].element, (uint8_t)comn_registers::CR);
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
				
				pushAction(out_actions, virtual_actions::set, tokenTypes::double_n, args[1].element, (uint8_t)comn_registers::RFPR3);
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

				pushAction(out_actions, virtual_actions::set, args[1].type, args[1].element, (uint8_t)comn_registers::RAX);
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
				pushAction(out_actions, virtual_actions::set, tokenTypes::unsigned_n, "2", (uint8_t)comn_registers::RAX);
				// Prepare recast arguments
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");
				pushAction(out_actions, virtual_actions::pushSR, tokenTypes::reg, "");
				// Invoke recast inerrupt
				pushAction(out_actions, virtual_actions::_int, tokenTypes::unsigned_n, "9");
				// Pop first char of SR from stack
				pushAction(out_actions, virtual_actions::popCR, tokenTypes::reg, "");
				// Set first 8 bytes of memory to 0
				pushAction(out_actions, virtual_actions::set, tokenTypes::unsigned_n, "0", (uint8_t)comn_registers::RAX);
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");
				pushAction(out_actions, virtual_actions::movsm, tokenTypes::reg, "rax");
				// Set value of CR into 7th byte of memory
				pushAction(out_actions, virtual_actions::set, tokenTypes::unsigned_n, std::to_string(sizeof(size_t) - 1), (uint8_t)comn_registers::RAX);
				pushAction(out_actions, virtual_actions::movsmCR, tokenTypes::reg, "rax");
				pushAction(out_actions, virtual_actions::popCR, tokenTypes::reg, ""); // Restore cr

				// Fetch first 8 bytes of memory and leave them on stack
				pushAction(out_actions, virtual_actions::set, tokenTypes::unsigned_n, "0", (uint8_t)comn_registers::RAX);
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
				pushAction(out_actions, virtual_actions::set, tokenTypes::str, args[1].element, (uint8_t)comn_registers::SR);

				// Get first char of SR
				pushAction(out_actions, virtual_actions::set, tokenTypes::unsigned_n, "2", (uint8_t)comn_registers::RAX);
				// Prepare recast arguments
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");
				pushAction(out_actions, virtual_actions::pushSR, tokenTypes::reg, "");
				// Invoke recast inerrupt
				pushAction(out_actions, virtual_actions::_int, tokenTypes::unsigned_n, "9");
				// Pop first char of SR from stack
				pushAction(out_actions, virtual_actions::popCR, tokenTypes::reg, "");
				// Set first 8 bytes of memory to 0
				pushAction(out_actions, virtual_actions::set, tokenTypes::unsigned_n, "0", (uint8_t)comn_registers::RAX);
				pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax");
				pushAction(out_actions, virtual_actions::movsm, tokenTypes::reg, "rax");
				// Set value of CR into 7th byte of memory
				pushAction(out_actions, virtual_actions::set, tokenTypes::unsigned_n, "7", (uint8_t)comn_registers::RAX);
				pushAction(out_actions, virtual_actions::movsmCR, tokenTypes::reg, "rax");
				pushAction(out_actions, virtual_actions::popCR, tokenTypes::reg, ""); // Restore cr

				// Fetch first 8 bytes of memory and leave them on stack
				pushAction(out_actions, virtual_actions::set, tokenTypes::unsigned_n, "0", (uint8_t)comn_registers::RAX);
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
			assert_err_reguse("cmp", args, cur_line);
		}
		else {
			if (args[1].type != tokenTypes::reg) {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::push, tokenTypes::reg, "rax"); // Save rax

				pushAction(out_actions, virtual_actions::set, args[1].type, args[1].element, (uint8_t)comn_registers::RAX);
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

				pushAction(out_actions, virtual_actions::set, tokenTypes::str, args[1].element, (uint8_t)comn_registers::SR);
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
			assert_err_reguse("cmp", args, cur_line);
		}
		else if (args[1].type == tokenTypes::double_n) {
			if (args[0].element == "rfpr3") {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr2");

				pushAction(out_actions, virtual_actions::set, tokenTypes::double_n, args[1].element, (uint8_t)comn_registers::RFPR2);
				pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr2");
				pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr3");
				pushAction(out_actions, virtual_actions::cmpdbl, tokenTypes::reg, "");

				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::popFP, tokenTypes::reg, "rfpr2");
			}
			else {
				if (!unsafe_flag)
					pushAction(out_actions, virtual_actions::pushFP, tokenTypes::fp_reg, "rfpr3");

				pushAction(out_actions, virtual_actions::set, tokenTypes::double_n, args[1].element, (uint8_t)comn_registers::RFPR3);
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

				pushAction(out_actions, virtual_actions::set, tokenTypes::unsigned_n, args[1].element, (uint8_t)comn_registers::RAX);
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
	
	for (size_t i = 0; i < tokens.size(); ++i) {
		cur_line = vline_rline[i];
		preprocInst(tokens[i], preprocLabels, out_actions);
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
	static const std::unordered_map<std::string, comn_registers> storeg = {
		{"ax", comn_registers::AX},
		{"bx", comn_registers::BX},
		{"cx", comn_registers::CX},
		{"dx", comn_registers::DX},

		{"eax", comn_registers::EAX},
		{"ebx", comn_registers::EBX},
		{"ecx", comn_registers::ECX},
		{"edx", comn_registers::EDX},

		{"rax", comn_registers::RAX},
		{"rbx", comn_registers::RBX},
		{"rcx", comn_registers::RCX},
		{"rdx", comn_registers::RDX},
		{"rbp", comn_registers::RBP},
		{"rsp", comn_registers::RSP},
	};
	static const std::unordered_map<std::string, comn_registers> xstoreg = {
		{"fpr0", comn_registers::FPR0},
		{"fpr1", comn_registers::FPR1},
		{"fpr2", comn_registers::FPR2},
		{"fpr3", comn_registers::FPR3},

		{"efpr0", comn_registers::EFPR0},
		{"efpr1", comn_registers::EFPR1},
		{"efpr2", comn_registers::EFPR2},
		{"efpr3", comn_registers::EFPR3},

		{"rfpr0", comn_registers::RFPR0},
		{"rfpr1", comn_registers::RFPR1},
		{"rfpr2", comn_registers::RFPR2},
		{"rfpr3", comn_registers::RFPR3},
	};

	const byte comp_action = instructions_set[raw_action.getAction()];
	const auto v_ptr = *std::static_pointer_cast<svasm_args>(raw_action.getValuePtr());

	auto [v_type, v_raw, v_opt] = *std::static_pointer_cast<svasm_args>(raw_action.getValuePtr());
	out_bytes.push_back(comp_action);

	if (opt_arg_ops.find(comp_action) != opt_arg_ops.end())
		out_bytes.push_back(v_opt);

	if (comp_action == ops[virtual_actions::set]) {
		if (comn_registers_table::is_num_reg((comn_registers)out_bytes.back())) {
			if (v_type != tokenTypes::signed_n && v_type != tokenTypes::unsigned_n) {
				assert_err("Can't compile opcode (" + std::to_string(comp_action) + "): ", "",
					{ v_raw + ":" + std::to_string((uint64_t)v_type) }, cur_line, ARGV_ERROR);
			}

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
		else if (comn_registers_table::is_fp_reg((comn_registers)out_bytes.back())) {
			if (v_type != tokenTypes::double_n) {
				assert_err("Can't compile opcode (" + std::to_string(comp_action) + "): ", "",
					{ v_raw + ":" + std::to_string((uint64_t)v_type) }, cur_line, ARGV_ERROR);
			}

			auto uc_a = std::make_unique<uint8_t[]>(sizeof(long double));
			long double d = std::stold(v_raw);
			mp_memcpy(&d, uc_a.get(), sizeof(long double));

			for (size_t i = 0; i < sizeof(long double); i++)
				out_bytes.push_back(uc_a[i]);
			return OK;
		}
		else if ((comn_registers)out_bytes.back() == comn_registers::SR) {
			if (v_type != tokenTypes::str) {
				assert_err("Can't compile opcode (" + std::to_string(comp_action) + "): ", "",
					{ v_raw + ":" + std::to_string((uint64_t)v_type) }, cur_line, ARGV_ERROR);
			}

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
		else if ((comn_registers)out_bytes.back() == comn_registers::CR) {
			if (v_type != tokenTypes::str) {
				assert_err("Can't compile opcode (" + std::to_string(comp_action) + "): ", "",
					{ v_raw + ":" + std::to_string((uint64_t)v_type) }, cur_line, ARGV_ERROR);
			}


			v_raw = v_raw.substr(1, v_raw.size() - 2);
			byte c;

			if (v_raw.empty() || v_raw.size() < 1)
				c = 0;
			else
				c = (byte)v_raw[0];

			out_bytes.push_back(c);
			return OK;
		}
	}

	if (zero_args_opcodes.find(comp_action) != zero_args_opcodes.end())		
		return OK;
	else if (uint64_args_opcodes.find(comp_action) != uint64_args_opcodes.end()) {
		if (v_type != tokenTypes::signed_n && v_type != tokenTypes::unsigned_n) {
			assert_err("Can't compile opcode (" + std::to_string(comp_action) + "): ", "",
				{ v_raw + ":" + std::to_string((uint64_t)v_type) }, cur_line, ARGV_ERROR);
		}

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
	else if (reg_args_opcodes.find(comp_action) != reg_args_opcodes.end()) {
		if (v_type != tokenTypes::reg && v_type != tokenTypes::fp_reg) {
			assert_err("Can't compile opcode (" + std::to_string(comp_action) + "): ", "",
				{ v_raw + ":" + std::to_string((uint64_t)v_type) }, cur_line, ARGV_ERROR);
		}

		byte comp_reg = (byte)strreg_to_reg.at(v_raw);

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
		ULLTOA((size_t)0x100000, &req_mem_size); // Default value

	for (size_t i = 0; i < sizeof(size_t); i++)
		out_bytes.emplace_back(req_mem_size[i]);
	delete[] req_mem_size;


	for (action raw_a : raw_actions)
		compileInst(raw_a, out_bytes);

	return OK;
}