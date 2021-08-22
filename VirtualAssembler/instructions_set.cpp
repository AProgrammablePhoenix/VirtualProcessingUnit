#include <iostream>
#include <map>
#include <string>
#include <unordered_set>
#include <vector>

#include "../Compiler/action_parser.h" 

#ifndef INST_Set_PREPROCESS
	#define PREFIX_1 virtual_actions
	#define PREFIX_2 comn_registers

	#define MConcat(a,b) a ## b

	#define FPROPC_PROTO(prefix, regname, opcode) \
		{PREFIX_1::prefix##regname, opcode}
	#define ALL_FPRs_PROTO(constructor, opcode) \
		constructor(FPR0, opcode),				\
		constructor(FPR1, opcode), 				\
		constructor(FPR2, opcode), 				\
		constructor(FPR3, opcode), 				\
		constructor(EFPR0, opcode), 			\
		constructor(EFPR1, opcode), 			\
		constructor(EFPR2, opcode), 			\
		constructor(EFPR3, opcode),				\
		constructor(RFPR0, opcode), 			\
		constructor(RFPR1, opcode), 			\
		constructor(RFPR2, opcode), 			\
		constructor(RFPR3, opcode)
	#define FPR2ndOPCSingle_PROTO(prefix, regname) \
		{PREFIX_1::prefix##regname, fp_registers_set[PREFIX_2::regname]}
	#define ALL_FPRs2ndOPC_PROTO(constructor)	\
		constructor(FPR0),						\
		constructor(FPR1),						\
		constructor(FPR2),						\
		constructor(FPR3),						\
		constructor(EFPR0),						\
		constructor(EFPR1),						\
		constructor(EFPR2),						\
		constructor(EFPR3),						\
		constructor(RFPR0),						\
		constructor(RFPR1),						\
		constructor(RFPR2),						\
		constructor(RFPR3)

	#define setFPROPC(regname, opcode) FPROPC_PROTO(set, regname, opcode)
	#define setFPRs(opcode) ALL_FPRs_PROTO(setFPROPC, opcode)
	#define setFPR_2ndSingle(regname) FPR2ndOPCSingle_PROTO(set, regname)
	#define setFPRs2nd() ALL_FPRs2ndOPC_PROTO(setFPR_2ndSingle)

	#define movFPROPC(regname, opcode) FPROPC_PROTO(mov, regname, opcode)
	#define movFPRs(opcode)	ALL_FPRs_PROTO(movFPROPC, opcode)
	#define movFPR_2ndSingle(regname) FPR2ndOPCSingle_PROTO(mov, regname)
	#define movFPRs2nd() ALL_FPRs2ndOPC_PROTO(movFPR_2ndSingle)
	
	#define mulFPROPC(regname, opcode) FPROPC_PROTO(mul, regname, opcode)
	#define mulFPRs(opcode) ALL_FPRs_PROTO(mulFPROPC, opcode)
	#define mulFPR_2ndSingle(regname) FPR2ndOPCSingle_PROTO(mul, regname)
	#define mulFPRs2nd() ALL_FPRs2ndOPC_PROTO(mulFPR_2ndSingle)

	#define divFPROPC(regname, opcode) FPROPC_PROTO(div, regname, opcode)
	#define divFPRs(opcode) ALL_FPRs_PROTO(divFPROPC, opcode)
	#define divFPR_2ndSingle(regname) FPR2ndOPCSingle_PROTO(div, regname)
	#define divFPRs2nd() ALL_FPRs2ndOPC_PROTO(divFPR_2ndSingle)

	#define addFPROPC(regname, opcode) FPROPC_PROTO(add, regname, opcode)
	#define addFPRs(opcode) ALL_FPRs_PROTO(addFPROPC, opcode)
	#define addFPR_2ndSingle(regname) FPR2ndOPCSingle_PROTO(add, regname)
	#define addFPRs2nd() ALL_FPRs2ndOPC_PROTO(addFPR_2ndSingle)

	#define subFPROPC(regname, opcode) FPROPC_PROTO(sub, regname, opcode)
	#define subFPRs(opcode) ALL_FPRs_PROTO(subFPROPC, opcode)
	#define subFPR_2ndSingle(regname) FPR2ndOPCSingle_PROTO(sub, regname)
	#define subFPRs2nd() ALL_FPRs2ndOPC_PROTO(subFPR_2ndSingle)
#endif

typedef unsigned char byte;

namespace {
	constexpr byte setFPRs_opcode = 0xA0;
	constexpr byte movFPRs_opcode = 0xA1;

	constexpr byte mulFPRs_opcode = 0xA2;
	constexpr byte divFPRs_opcode = 0xA3;
	constexpr byte addFPRs_opcode = 0xA4;
	constexpr byte subFPRs_opcode = 0xA5;
}

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

	{virtual_actions::setAX,  0x0D},
	{virtual_actions::setBX,  0x0E},
	{virtual_actions::setCX,  0x0F},
	{virtual_actions::setDX,  0x10},
	{virtual_actions::setEAX, 0x11},
	{virtual_actions::setEBX, 0x12},
	{virtual_actions::setECX, 0x13},
	{virtual_actions::setEDX, 0x14},
	{virtual_actions::setRAX, 0x15},
	{virtual_actions::setRBX, 0x16},
	{virtual_actions::setRCX, 0x17},
	{virtual_actions::setRDX, 0x18},

	{virtual_actions::setSR,  0x19},

	{virtual_actions::setCR,  0x1B},

	{virtual_actions::movAX,  0x1F},
	{virtual_actions::movBX,  0x20},
	{virtual_actions::movCX,  0x21},
	{virtual_actions::movDX,  0x22},
	{virtual_actions::movEAX, 0x23},
	{virtual_actions::movEBX, 0x24},
	{virtual_actions::movECX, 0x25},
	{virtual_actions::movEDX, 0x26},
	{virtual_actions::movRAX, 0x27},
	{virtual_actions::movRBX, 0x28},
	{virtual_actions::movRCX, 0x29},
	{virtual_actions::movRDX, 0x2A},
	{virtual_actions::movRBP, 0x2B},
	{virtual_actions::movRSP, 0x2C},

	{virtual_actions::inc,	  0x2D},
	{virtual_actions::dec,	  0x2E},

	{virtual_actions::mulAX,  0x31},
	{virtual_actions::mulBX,  0x32},
	{virtual_actions::mulCX,  0x33},
	{virtual_actions::mulDX,  0x34},
	{virtual_actions::mulEAX, 0x35},
	{virtual_actions::mulEBX, 0x36},
	{virtual_actions::mulECX, 0x37},
	{virtual_actions::mulEDX, 0x38},
	{virtual_actions::mulRAX, 0x39},
	{virtual_actions::mulRBX, 0x3A},
	{virtual_actions::mulRCX, 0x3B},
	{virtual_actions::mulRDX, 0x3C},
	
	{virtual_actions::divAX,  0x3D},
	{virtual_actions::divBX,  0x3E},
	{virtual_actions::divCX,  0x3F},
	{virtual_actions::divDX,  0x40},
	{virtual_actions::divEAX, 0x41},
	{virtual_actions::divEBX, 0x42},
	{virtual_actions::divECX, 0x43},
	{virtual_actions::divEDX, 0x44},
	{virtual_actions::divRAX, 0x45},
	{virtual_actions::divRBX, 0x46},
	{virtual_actions::divRCX, 0x47},
	{virtual_actions::divRDX, 0x48},

	{virtual_actions::addAX,  0x49},
	{virtual_actions::addBX,  0x4A},
	{virtual_actions::addCX,  0x4B},
	{virtual_actions::addDX,  0x4C},
	{virtual_actions::addEAX, 0x4D},
	{virtual_actions::addEBX, 0x4E},
	{virtual_actions::addECX, 0x4F},
	{virtual_actions::addEDX, 0x50},
	{virtual_actions::addRAX, 0x51},
	{virtual_actions::addRBX, 0x52},
	{virtual_actions::addRCX, 0x53},
	{virtual_actions::addRDX, 0x54},
	{virtual_actions::addRBP, 0x55},
	{virtual_actions::addRSP, 0x56},

	{virtual_actions::subAX,  0x57},
	{virtual_actions::subBX,  0x58},
	{virtual_actions::subCX,  0x59},
	{virtual_actions::subDX,  0x5A},
	{virtual_actions::subEAX, 0x5B},
	{virtual_actions::subEBX, 0x5C},
	{virtual_actions::subECX, 0x5D},
	{virtual_actions::subEDX, 0x5E},
	{virtual_actions::subRAX, 0x5F},
	{virtual_actions::subRBX, 0x60},
	{virtual_actions::subRCX, 0x61},
	{virtual_actions::subRDX, 0x62},
	{virtual_actions::subRBP, 0x63},
	{virtual_actions::subRSP, 0x64},

	{virtual_actions::toString,	0x65},
	{virtual_actions::castreg,  0x66},

	{virtual_actions::push, 0x67},
	{virtual_actions::pop,  0x68},

	{virtual_actions::pushSR, 0x69},
	{virtual_actions::popSR,  0x6A},

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
	{virtual_actions::cmpstr, 0x81},
	
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

	{virtual_actions::movsmSR, 0x8F},
	{virtual_actions::movgmSR, 0x90},

	{virtual_actions::movsmCR, 0x91},
	{virtual_actions::movgmCR, 0x92},

	{virtual_actions::nopen,   0x95},
	{virtual_actions::nclose,  0x96},
	{virtual_actions::nget,	   0x97},
	{virtual_actions::nsend,   0x98},
	{virtual_actions::nhrecv,  0x99},
	{virtual_actions::ncrtep,  0x9A},
	{virtual_actions::nselep,  0x9B},

	{virtual_actions::pcrtthread, 0x9C},
	{virtual_actions::prstthread, 0x9D},
	{virtual_actions::pendthread, 0x9E},

	{virtual_actions::sdzs, 0x9F},

	setFPRs(setFPRs_opcode),
	movFPRs(movFPRs_opcode),

	mulFPRs(mulFPRs_opcode),
	divFPRs(divFPRs_opcode),
	addFPRs(addFPRs_opcode),
	subFPRs(subFPRs_opcode),

	{virtual_actions::pushFP, subFPRs_opcode + 1},
	{virtual_actions::popFP, subFPRs_opcode + 2},

	{virtual_actions::movsmFP, subFPRs_opcode + 3},
	{virtual_actions::movgmFP, subFPRs_opcode + 4},

	{virtual_actions::cmpdbl, subFPRs_opcode + 5},

	{virtual_actions::incFP, subFPRs_opcode + 6},
	{virtual_actions::decFP, subFPRs_opcode + 7},

	{virtual_actions::rfread, subFPRs_opcode + 8},
	{virtual_actions::rfwrite, subFPRs_opcode + 9},
	{virtual_actions::rflen, subFPRs_opcode + 10}
};

std::map<virtual_actions, byte> map_FPR_set_2nd_opc = {
	setFPRs2nd()
};
std::map<virtual_actions, byte> map_FPR_mov_2nd_opc = {
	movFPRs2nd()
};

std::map<virtual_actions, byte> map_FPR_mul_2nd_opc = {
	mulFPRs2nd()
};
std::map<virtual_actions, byte> map_FPR_div_2nd_opc = {
	divFPRs2nd()
};
std::map<virtual_actions, byte> map_FPR_add_2nd_opc = {
	addFPRs2nd()
};
std::map<virtual_actions, byte> map_FPR_sub_2nd_opc = {
	subFPRs2nd()
};

std::map<virtual_actions, byte>& ops = instructions_set;

std::unordered_set<byte> zero_args_opcodes = {
	ops[virtual_actions::pushSR],
	ops[virtual_actions::popSR],
	ops[virtual_actions::pushCR],
	ops[virtual_actions::popCR],


	ops[virtual_actions::cmp],
	ops[virtual_actions::cmpstr],
	ops[virtual_actions::cmpdbl],

	ops[virtual_actions::gca],
	ops[virtual_actions::hlt],
	ops[virtual_actions::ret],
	ops[virtual_actions::svcall],
	ops[virtual_actions::rscall],

	ops[virtual_actions::rflen]
};
std::unordered_set<byte> uint64_args_opcodes = {
	ops[virtual_actions::_int],

	ops[virtual_actions::setAX],
	ops[virtual_actions::setBX],
	ops[virtual_actions::setCX],
	ops[virtual_actions::setDX],
	ops[virtual_actions::setEAX],
	ops[virtual_actions::setEBX],
	ops[virtual_actions::setECX],
	ops[virtual_actions::setEDX],
	ops[virtual_actions::setRAX],
	ops[virtual_actions::setRBX],
	ops[virtual_actions::setRCX],
	ops[virtual_actions::setRDX],

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

	ops[virtual_actions::pcrtthread],
	ops[virtual_actions::prstthread],
	ops[virtual_actions::pendthread],

	ops[virtual_actions::rfread],
	ops[virtual_actions::rfwrite]
};
std::unordered_set<byte> reg_args_opcodes = {
	ops[virtual_actions::movAX],
	ops[virtual_actions::movBX],
	ops[virtual_actions::movCX],
	ops[virtual_actions::movDX],
	ops[virtual_actions::movEAX],
	ops[virtual_actions::movEBX],
	ops[virtual_actions::movECX],
	ops[virtual_actions::movEDX],
	ops[virtual_actions::movRAX],
	ops[virtual_actions::movRBX],
	ops[virtual_actions::movRCX],
	ops[virtual_actions::movRDX],
	ops[virtual_actions::movRBP],
	ops[virtual_actions::movRSP],

	ops[virtual_actions::inc],
	ops[virtual_actions::dec],

	ops[virtual_actions::incFP],
	ops[virtual_actions::decFP],

	ops[virtual_actions::mulAX],
	ops[virtual_actions::mulBX],
	ops[virtual_actions::mulCX],
	ops[virtual_actions::mulDX],
	ops[virtual_actions::mulEAX],
	ops[virtual_actions::mulEBX],
	ops[virtual_actions::mulECX],
	ops[virtual_actions::mulEDX],
	ops[virtual_actions::mulRAX],
	ops[virtual_actions::mulRBX],
	ops[virtual_actions::mulRCX],
	ops[virtual_actions::mulRDX],

	ops[virtual_actions::divAX],
	ops[virtual_actions::divBX],
	ops[virtual_actions::divCX],
	ops[virtual_actions::divDX],
	ops[virtual_actions::divEAX],
	ops[virtual_actions::divEBX],
	ops[virtual_actions::divECX],
	ops[virtual_actions::divEDX],
	ops[virtual_actions::divRAX],
	ops[virtual_actions::divRBX],
	ops[virtual_actions::divRCX],
	ops[virtual_actions::divRDX],

	ops[virtual_actions::addAX],
	ops[virtual_actions::addBX],
	ops[virtual_actions::addCX],
	ops[virtual_actions::addDX],
	ops[virtual_actions::addEAX],
	ops[virtual_actions::addEBX],
	ops[virtual_actions::addECX],
	ops[virtual_actions::addEDX],
	ops[virtual_actions::addRAX],
	ops[virtual_actions::addRBX],
	ops[virtual_actions::addRCX],
	ops[virtual_actions::addRDX],
	ops[virtual_actions::addRBP],
	ops[virtual_actions::addRSP],

	ops[virtual_actions::subAX],
	ops[virtual_actions::subBX],
	ops[virtual_actions::subCX],
	ops[virtual_actions::subDX],
	ops[virtual_actions::subEAX],
	ops[virtual_actions::subEBX],
	ops[virtual_actions::subECX],
	ops[virtual_actions::subEDX],
	ops[virtual_actions::subRAX],
	ops[virtual_actions::subRBX],
	ops[virtual_actions::subRCX],
	ops[virtual_actions::subRDX],
	ops[virtual_actions::subRBP],
	ops[virtual_actions::subRSP],

	ops[virtual_actions::sdzs],

	ops[virtual_actions::push],
	ops[virtual_actions::pop],

	ops[virtual_actions::pushFP],
	ops[virtual_actions::popFP],

	ops[virtual_actions::movsm],
	ops[virtual_actions::movgm],

	ops[virtual_actions::movsmSR],
	ops[virtual_actions::movgmSR],

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
std::unordered_set<byte> parted_opcodes = {
	setFPRs_opcode,
	movFPRs_opcode,
	mulFPRs_opcode,
	divFPRs_opcode,
	addFPRs_opcode,
	subFPRs_opcode
};

std::unordered_set<byte> opt_arg_ops = {
	ops[virtual_actions::push],
	ops[virtual_actions::pop],

	ops[virtual_actions::movsm],
	ops[virtual_actions::movgm]
};