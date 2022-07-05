#include <iostream>
#include <map>
#include <string>
#include <unordered_set>
#include <vector>

#include "../Compiler/action_parser.h" 

typedef unsigned char byte;

std::map<comn_registers, byte> registers_set = {
	{comn_registers::AH,  (byte)comn_registers::AH},
	{comn_registers::AL,  (byte)comn_registers::AL},
	{comn_registers::AX,  (byte)comn_registers::AX},

	{comn_registers::BH,  (byte)comn_registers::BH},
	{comn_registers::BL,  (byte)comn_registers::BL},
	{comn_registers::BX,  (byte)comn_registers::BX},

	{comn_registers::CH,  (byte)comn_registers::CH},
	{comn_registers::CL,  (byte)comn_registers::CL},
	{comn_registers::CX,  (byte)comn_registers::CX},

	{comn_registers::DH,  (byte)comn_registers::DH},
	{comn_registers::DL,  (byte)comn_registers::DL},
	{comn_registers::DX,  (byte)comn_registers::DX},

	{comn_registers::EAX, (byte)comn_registers::EAX},
	{comn_registers::EBX, (byte)comn_registers::EBX},
	{comn_registers::ECX, (byte)comn_registers::ECX},
	{comn_registers::EDX, (byte)comn_registers::EDX},

	{comn_registers::RAX, (byte)comn_registers::RAX},
	{comn_registers::RBX, (byte)comn_registers::RBX},
	{comn_registers::RCX, (byte)comn_registers::RCX},
	{comn_registers::RDX, (byte)comn_registers::RDX},

	{comn_registers::RBP, (byte)comn_registers::RBP},
	{comn_registers::RSP, (byte)comn_registers::RSP},

	{comn_registers::RDI, (byte)comn_registers::RDI},
	{comn_registers::RSI, (byte)comn_registers::RSI}
};
std::map<comn_registers, byte> fp_registers_set = {
	{comn_registers::FPR0, (byte)comn_registers::FPR0},
	{comn_registers::FPR1, (byte)comn_registers::FPR1},
	{comn_registers::FPR2, (byte)comn_registers::FPR2},
	{comn_registers::FPR3, (byte)comn_registers::FPR3},

	{comn_registers::EFPR0, (byte)comn_registers::EFPR0},
	{comn_registers::EFPR1, (byte)comn_registers::EFPR1},
	{comn_registers::EFPR2, (byte)comn_registers::EFPR2},
	{comn_registers::EFPR3, (byte)comn_registers::EFPR3},

	{comn_registers::RFPR0, (byte)comn_registers::RFPR0},
	{comn_registers::RFPR1, (byte)comn_registers::RFPR1},
	{comn_registers::RFPR2, (byte)comn_registers::RFPR2},
	{comn_registers::RFPR3, (byte)comn_registers::RFPR3}
};
std::map<virtual_actions, byte> instructions_set = {
	{virtual_actions::_int,   0x00},

	{virtual_actions::set,	  0x01},
	{virtual_actions::mov,	  0x02},

	{virtual_actions::inc,	  0x03},
	{virtual_actions::dec,	  0x04},

	{virtual_actions::mul,	  0x2F},
	{virtual_actions::div,	  0x30},
	{virtual_actions::add,   0x31},
	{virtual_actions::sub,   0x32},

	{virtual_actions::toString,	0x65},
	{virtual_actions::castreg,  0x66},

	{virtual_actions::push, 0x67},
	{virtual_actions::pop,  0x68},

	{virtual_actions::pushCR, 0x6B},
	{virtual_actions::popCR,  0x6C},

	{virtual_actions::_not, 0x6F},
	{virtual_actions::_and, 0x70},
	{virtual_actions::_or,  0x71},
	{virtual_actions::_xor, 0x72},
	{virtual_actions::_shl, 0x73},
	{virtual_actions::_shr, 0x74},
	
	{virtual_actions::_log,   0x75},
	{virtual_actions::_log2,  0x76},
	{virtual_actions::_log10, 0x77},
	{virtual_actions::_pow,   0x78},

	{virtual_actions::jmp,    0x79},
	{virtual_actions::cmp,    0x7A},
	{virtual_actions::je,     0x7B},
	{virtual_actions::jne,    0x7C},
	{virtual_actions::jl,     0x7D},
	{virtual_actions::jg,     0x7E},
	{virtual_actions::jle,    0x7F},
	{virtual_actions::jge,    0x80},
	
	{virtual_actions::gca,    0x82},
	{virtual_actions::hlt,    0x83},
	{virtual_actions::call,   0x84},
	{virtual_actions::lcall,  0x85},
	{virtual_actions::ret,    0x86},
	{virtual_actions::svcall, 0x87},
	{virtual_actions::rscall, 0x88},

	{virtual_actions::_dlog,   0x89},
	{virtual_actions::_dlog2,  0x8A},
	{virtual_actions::_dlog10, 0x8B},
	{virtual_actions::_dpow,   0x8C},

	{virtual_actions::movsm,   0x8D},
	{virtual_actions::movgm,   0x8E},

	{virtual_actions::movsmCR, 0x91},
	{virtual_actions::movgmCR, 0x92},

	{virtual_actions::nopen,   0x95},
	{virtual_actions::nclose,  0x96},
	{virtual_actions::nget,	   0x97},
	{virtual_actions::nsend,   0x98},
	{virtual_actions::nhrecv,  0x99},
	{virtual_actions::ncrtep,  0x9A},
	{virtual_actions::nselep,  0x9B},

	{virtual_actions::sdzs, 0x9F},
	{virtual_actions::cli,	0xA0},
	{virtual_actions::sti,	0xA1},

	{virtual_actions::pushFP,	0xA2},
	{virtual_actions::popFP,	0xA3},

	{virtual_actions::movsmFP,	0xA4},
	{virtual_actions::movgmFP,	0xA5},

	{virtual_actions::cmpdbl,	0xA6},

	{virtual_actions::rfread,	0xA7},
	{virtual_actions::rfwrite,	0xA8},
	{virtual_actions::rflen,	0xA9}
};

std::map<virtual_actions, byte>& ops = instructions_set;

std::unordered_set<byte> zero_args_opcodes = {
	ops[virtual_actions::pushCR],
	ops[virtual_actions::popCR],


	ops[virtual_actions::cmp],
	ops[virtual_actions::cmpdbl],

	ops[virtual_actions::gca],
	ops[virtual_actions::cli],
	ops[virtual_actions::sti],
	ops[virtual_actions::hlt],
	ops[virtual_actions::ret],
	ops[virtual_actions::svcall],
	ops[virtual_actions::rscall],

	ops[virtual_actions::rflen]
};
std::unordered_set<byte> uint64_args_opcodes = {
	ops[virtual_actions::_int],

	ops[virtual_actions::jmp],
	ops[virtual_actions::je],
	ops[virtual_actions::jne],
	ops[virtual_actions::jl],
	ops[virtual_actions::jg],
	ops[virtual_actions::jle],
	ops[virtual_actions::jge],


	ops[virtual_actions::call],
	ops[virtual_actions::lcall],

	ops[virtual_actions::nopen],
	ops[virtual_actions::nclose],
	ops[virtual_actions::nget],
	ops[virtual_actions::nsend],
	ops[virtual_actions::nhrecv],
	ops[virtual_actions::ncrtep],
	ops[virtual_actions::nselep],

	ops[virtual_actions::rfread],
	ops[virtual_actions::rfwrite]
};
std::unordered_set<byte> reg_args_opcodes = {
	ops[virtual_actions::mov],

	ops[virtual_actions::inc],
	ops[virtual_actions::dec],

	ops[virtual_actions::mul],
	ops[virtual_actions::div],
	ops[virtual_actions::add],
	ops[virtual_actions::sub],

	ops[virtual_actions::sdzs],

	ops[virtual_actions::push],
	ops[virtual_actions::pop],

	ops[virtual_actions::pushFP],
	ops[virtual_actions::popFP],

	ops[virtual_actions::movsm],
	ops[virtual_actions::movgm],

	ops[virtual_actions::movsmCR],
	ops[virtual_actions::movgmCR],

	ops[virtual_actions::movsmFP],
	ops[virtual_actions::movgmFP],

	ops[virtual_actions::toString],
	ops[virtual_actions::castreg],

	ops[virtual_actions::_not],
	ops[virtual_actions::_and],
	ops[virtual_actions::_or],
	ops[virtual_actions::_xor],
	ops[virtual_actions::_shl],
	ops[virtual_actions::_shr],

	ops[virtual_actions::_log],
	ops[virtual_actions::_log2],
	ops[virtual_actions::_log10],
	ops[virtual_actions::_pow],

	ops[virtual_actions::_dlog],
	ops[virtual_actions::_dlog2],
	ops[virtual_actions::_dlog10],
	ops[virtual_actions::_dpow]
};

std::unordered_set<byte> opt_arg_ops = {
	ops[virtual_actions::push],
	ops[virtual_actions::pop],

	ops[virtual_actions::movsm],
	ops[virtual_actions::movgm],

	ops[virtual_actions::set],
	ops[virtual_actions::mov],

	ops[virtual_actions::mul],
	ops[virtual_actions::div],
	ops[virtual_actions::add],
	ops[virtual_actions::sub]
};