#include <iostream>
#include <map>
#include <string>
#include <unordered_set>
#include <vector>

#include "../Compiler/action_parser.h"

typedef unsigned char byte;

std::map<registries_def, byte> registers_set = {
	{AH,  0x01},
	{AL,  0x02},
	{AX,  0x03},
	{BH,  0x04},
	{BL,  0x05},
	{BX,  0x06},
	{CH,  0x07},
	{CL,  0x08},
	{CX,  0x09},
	{DH,  0x0A},
	{DL,  0x0B},
	{DX,  0x0C},
	{EAX, 0x0D},
	{EBX, 0x0E},
	{ECX, 0x0F},
	{EDX, 0x10},
	{RAX, 0x11},
	{RBX, 0x12},
	{RCX, 0x13},
	{RDX, 0x14}
};
std::map<virtual_actions, byte> instructions_set = {
	{virtual_actions::_int,   0x00},

	{virtual_actions::getAX,  0x01},
	{virtual_actions::getBX,  0x02},
	{virtual_actions::getCX,  0x03},
	{virtual_actions::getDX,  0x04},
	{virtual_actions::getEAX, 0x05},
	{virtual_actions::getEBX, 0x06},
	{virtual_actions::getECX, 0x07},
	{virtual_actions::getEDX, 0x08},
	{virtual_actions::getRAX, 0x09},
	{virtual_actions::getRBX, 0x0A},
	{virtual_actions::getRCX, 0x0B},
	{virtual_actions::getRDX, 0x0C},

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
	{virtual_actions::getSR,  0x1A},

	{virtual_actions::setCR,  0x1B},
	{virtual_actions::getCR,  0x1C},

	{virtual_actions::setDR,  0x1D},
	{virtual_actions::getDR,  0x1E},

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

	{virtual_actions::inc,	  0x2B},
	{virtual_actions::dec,	  0x2C},

	{virtual_actions::incDR,  0x2D},
	{virtual_actions::decDR,  0x2E},

	{virtual_actions::mulAX,  0x2F},
	{virtual_actions::mulBX,  0x30},
	{virtual_actions::mulCX,  0x31},
	{virtual_actions::mulDX,  0x32},
	{virtual_actions::mulEAX, 0x33},
	{virtual_actions::mulEBX, 0x34},
	{virtual_actions::mulECX, 0x35},
	{virtual_actions::mulEDX, 0x36},
	{virtual_actions::mulRAX, 0x37},
	{virtual_actions::mulRBX, 0x38},
	{virtual_actions::mulRCX, 0x39},
	{virtual_actions::mulRDX, 0x3A},
	
	{virtual_actions::divAX,  0x3B},
	{virtual_actions::divBX,  0x3C},
	{virtual_actions::divCX,  0x3D},
	{virtual_actions::divDX,  0x3E},
	{virtual_actions::divEAX, 0x3F},
	{virtual_actions::divEBX, 0x40},
	{virtual_actions::divECX, 0x41},
	{virtual_actions::divEDX, 0x42},
	{virtual_actions::divRAX, 0x43},
	{virtual_actions::divRBX, 0x44},
	{virtual_actions::divRCX, 0x45},
	{virtual_actions::divRDX, 0x46},

	{virtual_actions::addAX,  0x47},
	{virtual_actions::addBX,  0x48},
	{virtual_actions::addCX,  0x49},
	{virtual_actions::addDX,  0x4A},
	{virtual_actions::addEAX, 0x4B},
	{virtual_actions::addEBX, 0x4C},
	{virtual_actions::addECX, 0x4D},
	{virtual_actions::addEDX, 0x4E},
	{virtual_actions::addRAX, 0x4F},
	{virtual_actions::addRBX, 0x50},
	{virtual_actions::addRCX, 0x51},
	{virtual_actions::addRDX, 0x52},

	{virtual_actions::subAX,  0x53},
	{virtual_actions::subBX,  0x54},
	{virtual_actions::subCX,  0x55},
	{virtual_actions::subDX,  0x56},
	{virtual_actions::subEAX, 0x57},
	{virtual_actions::subEBX, 0x58},
	{virtual_actions::subECX, 0x59},
	{virtual_actions::subEDX, 0x5A},
	{virtual_actions::subRAX, 0x5B},
	{virtual_actions::subRBX, 0x5C},
	{virtual_actions::subRCX, 0x5D},
	{virtual_actions::subRDX, 0x5E},

	{virtual_actions::toString,	0x5F},
	{virtual_actions::castreg,  0x60},

	{virtual_actions::push, 0x61},
	{virtual_actions::pop,  0x62},

	{virtual_actions::pushSR, 0x63},
	{virtual_actions::popSR,  0x64},

	{virtual_actions::pushCR, 0x65},
	{virtual_actions::popCR,  0x66},

	{virtual_actions::pushDR, 0x67},
	{virtual_actions::popDR,  0x68},

	{virtual_actions::_not, 0x69},
	{virtual_actions::_and, 0x6A},
	{virtual_actions::_or,  0x6B},
	{virtual_actions::_xor, 0x6C},
	{virtual_actions::_shl, 0x6D},
	{virtual_actions::_shr, 0x6E},
	
	{virtual_actions::_log,   0x6F},
	{virtual_actions::_log2,  0x70},
	{virtual_actions::_log10, 0x71},
	{virtual_actions::_pow,   0x72},

	{virtual_actions::ijmp,   0x73},
	{virtual_actions::jmp,    0x74},
	{virtual_actions::cmp,    0x75},
	{virtual_actions::je,     0x76},
	{virtual_actions::jne,    0x77},
	{virtual_actions::jl,     0x78},
	{virtual_actions::jg,     0x79},
	{virtual_actions::jle,    0x7A},
	{virtual_actions::jge,    0x7B},
	{virtual_actions::cmpstr, 0x7C},
	
	{virtual_actions::gca,    0x7D},
	{virtual_actions::hlt,    0x7E},
	{virtual_actions::call,   0x7F},
	{virtual_actions::lcall,  0x80},
	{virtual_actions::ret,    0x81},
	{virtual_actions::svcall, 0x82},
	{virtual_actions::rscall, 0x83},

	{virtual_actions::_dlog,   0x84},
	{virtual_actions::_dlog2,  0x85},
	{virtual_actions::_dlog10, 0x86},
	{virtual_actions::_dpow,   0x87},

	{virtual_actions::movsm,   0x88},
	{virtual_actions::movgm,   0x89},

	{virtual_actions::movsmSR, 0x8A},
	{virtual_actions::movgmSR, 0x8B},

	{virtual_actions::movsmCR, 0x8C},
	{virtual_actions::movgmCR, 0x8D},

	{virtual_actions::movsmDR, 0x8E},
	{virtual_actions::movgmDR, 0x8F},

	{virtual_actions::nsms,	   0x90},
};

std::map<virtual_actions, byte>& ops = instructions_set;

std::unordered_set<byte> zero_args_opcodes = {
	ops[virtual_actions::getAX],
	ops[virtual_actions::getBX],
	ops[virtual_actions::getCX],
	ops[virtual_actions::getDX],
	ops[virtual_actions::getEAX],
	ops[virtual_actions::getEBX],
	ops[virtual_actions::getECX],
	ops[virtual_actions::getEDX],
	ops[virtual_actions::getRAX],
	ops[virtual_actions::getRBX],
	ops[virtual_actions::getRCX],
	ops[virtual_actions::getRDX],

	ops[virtual_actions::getSR],
	ops[virtual_actions::getCR],
	ops[virtual_actions::getDR],

	ops[virtual_actions::incDR],
	ops[virtual_actions::decDR],

	ops[virtual_actions::pushSR],
	ops[virtual_actions::popSR],
	ops[virtual_actions::pushCR],
	ops[virtual_actions::popCR],
	ops[virtual_actions::pushDR],
	ops[virtual_actions::popDR],

	ops[virtual_actions::ijmp],
	ops[virtual_actions::jmp],
	ops[virtual_actions::cmp],
	ops[virtual_actions::je],
	ops[virtual_actions::jne],
	ops[virtual_actions::jl],
	ops[virtual_actions::jg],
	ops[virtual_actions::jle],
	ops[virtual_actions::jge],
	ops[virtual_actions::cmpstr],

	ops[virtual_actions::gca],
	ops[virtual_actions::hlt],
	ops[virtual_actions::ret],
	ops[virtual_actions::svcall],
	ops[virtual_actions::rscall],

	ops[virtual_actions::_dlog],
	ops[virtual_actions::_dlog2],
	ops[virtual_actions::_dlog10],
	ops[virtual_actions::_dpow]
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

	ops[virtual_actions::call],
	ops[virtual_actions::lcall],

	ops[virtual_actions::nsms]
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

	ops[virtual_actions::inc],
	ops[virtual_actions::dec],

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

	ops[virtual_actions::push],
	ops[virtual_actions::pop],

	ops[virtual_actions::movsm],
	ops[virtual_actions::movgm],

	ops[virtual_actions::movsmSR],
	ops[virtual_actions::movgmSR],

	ops[virtual_actions::movsmCR],
	ops[virtual_actions::movgmCR],

	ops[virtual_actions::movsmDR],
	ops[virtual_actions::movgmDR],

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
	ops[virtual_actions::_pow]
};