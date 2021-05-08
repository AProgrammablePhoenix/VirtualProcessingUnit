#pragma once

#include <iostream>
#include <string>
#include <functional>
#include <vector>

#include "../Memory/memory_decl.h"
#include "regs_decl.h"
#pragma warning (push)
#pragma warning (disable : 4091)
#pragma warning (disable : 26812)

/*
enum regs_actions {
	getAX = 0x000100,
	getBX = 0x000200,
	getCX = 0x000300,
	getDX = 0x000400,

	setAX = 0x000500,
	setBX = 0x000600,
	setCX = 0x000700,
	setDX = 0x000800,

	getEAX = 0x000900,
	getEBX = 0x000A00,
	getECX = 0x000B00,
	getEDX = 0x000C00,

	setEAX = 0x000D00,
	setEBX = 0x000E00,
	setECX = 0x000F00,
	setEDX = 0x001000,

	getRAX = 0x001100,
	getRBX = 0x001200,
	getRCX = 0x001300,
	getRDX = 0x001400,

	setRAX = 0x001500,
	setRBX = 0x001600,
	setRCX = 0x001700,
	setRDX = 0x001800,

	setSR = 0x001801,
	getSR = 0x001802,

	movAX = 0x001900,
	movBX = 0x001A00,
	movCX = 0x001B00,
	movDX = 0x001C00,

	movEAX = 0x001D00,
	movEBX = 0x001E00,
	movECX = 0x001F00,
	movEDX = 0x002000,

	movRAX = 0x002100,
	movRBX = 0x002200,
	movRCX = 0x002300,
	movRDX = 0x002400,

	inc = 0x002500,
	dec = 0x002600,

	mulAX = 0x002700,
	mulBX = 0x002800,
	mulCX = 0x002900,
	mulDX = 0x002A00,

	mulEAX = 0x002B00,
	mulEBX = 0x002C00,
	mulECX = 0x002D00,
	mulEDX = 0x002E00,

	mulRAX = 0x002F00,
	mulRBX = 0x003000,
	mulRCX = 0x003100,
	mulRDX = 0x003200,

	divAX = 0x003300,
	divBX = 0x003400,
	divCX = 0x003500,
	divDX = 0x003600,

	divEAX = 0x003700,
	divEBX = 0x003800,
	divECX = 0x003900,
	divEDX = 0x003A00,

	divRAX = 0x003B00,
	divRBX = 0x003C00,
	divRCX = 0x003D00,
	divRDX = 0x003E00
};
*/

enum registries_def {
	AL = 0x000001,
	AH = 0x000010,

	BL = 0x000002,
	BH = 0x000020,

	CL = 0x000003,
	CH = 0x000030,

	DL = 0x000004,
	DH = 0x000040,


	AX = 0x000005,
	BX = 0x000006,
	CX = 0x000007,
	DX = 0x000008,

	EAX = 0x000050,
	EBX = 0x000060,
	ECX = 0x000070,
	EDX = 0x000080,

	RAX = 0x000055,
	RBX = 0x000066,
	RCX = 0x000077,
	RDX = 0x000088
};

enum extra_registries {
	SR = 0x000009
};

#pragma warning (pop)

// Native registers ops
#pragma region native_regs_ops
void b_set16AX(std::shared_ptr<void> a, regs* regsiters, memory* unused_m);
void b_set16BX(std::shared_ptr<void> a, regs* registers, memory* unused_m);
void b_set16CX(std::shared_ptr<void> a, regs* registers, memory* unused_m);
void b_set16DX(std::shared_ptr<void> a, regs* registers, memory* unused_m);

void b_set32EAX(std::shared_ptr<void> a, regs* registers, memory* unused_m);
void b_set32EBX(std::shared_ptr<void> a, regs* registers, memory* unused_m);
void b_set32ECX(std::shared_ptr<void> a, regs* registers, memory* unused_m);
void b_set32EDX(std::shared_ptr<void> a, regs* registers, memory* unused_m);

void b_set64RAX(std::shared_ptr<void> a, regs* registers, memory* unused_m);
void b_set64RBX(std::shared_ptr<void> a, regs* registers, memory* unused_m);
void b_set64RCX(std::shared_ptr<void> a, regs* registers, memory* unused_m);
void b_set64RDX(std::shared_ptr<void> a, regs* registers, memory* unused_m);

void b_setSR(std::shared_ptr<void> a, regs* registers, memory* unused_m);


void b_get16AX(std::shared_ptr<void> receiver, regs* registers, memory* unused_m);
void b_get16BX(std::shared_ptr<void> receiver, regs* registers, memory* unused_m);
void b_get16CX(std::shared_ptr<void> receiver, regs* registers, memory* unused_m);
void b_get16DX(std::shared_ptr<void> receiver, regs* registers, memory* unused_m);

void b_get32EAX(std::shared_ptr<void> receiver, regs* registers, memory* unused_m);
void b_get32EBX(std::shared_ptr<void> receiver, regs* registers, memory* unused_m);
void b_get32ECX(std::shared_ptr<void> receiver, regs* registers, memory* unused_m);
void b_get32EDX(std::shared_ptr<void> receiver, regs* registers, memory* unused_m);

void b_get64RAX(std::shared_ptr<void> receiver, regs* registers, memory* unused_m);
void b_get64RBX(std::shared_ptr<void> receiver, regs* registers, memory* unused_m);
void b_get64RCX(std::shared_ptr<void> receiver, regs* registers, memory* unused_m);
void b_get64RDX(std::shared_ptr<void> receiver, regs* registers, memory* unused_m);

void b_getSR(std::shared_ptr<void> receiver, regs* registers, memory* unused_m);

#pragma endregion

// Mov ops
#pragma region mov_ops
void b_mov16AX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_mov16BX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_mov16CX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_mov16DX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);

void b_mov32EAX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_mov32EBX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_mov32ECX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_mov32EDX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);

void b_mov64RAX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_mov64RBX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_mov64RCX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_mov64RDX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
#pragma endregion

// Native maths ops
#pragma region native_maths_ops
void b_inc(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_dec(std::shared_ptr<void> reg, regs* registers, memory* unused_m);

void b_mul16AX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_mul16BX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_mul16CX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_mul16DX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);

void b_mul32EAX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_mul32EBX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_mul32ECX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_mul32EDX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);

void b_mul64RAX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_mul64RBX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_mul64RCX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_mul64RDX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);

void b_div16AX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_div16BX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_div16CX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_div16DX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);

void b_div32EAX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_div32EBX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_div32ECX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_div32EDX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);

void b_div64RAX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_div64RBX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_div64RCX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_div64RDX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);

void b_add16AX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_add16BX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_add16CX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_add16DX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);

void b_add32EAX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_add32EBX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_add32ECX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_add32EDX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);

void b_add64RAX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_add64RBX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_add64RCX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_add64RDX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);

void b_sub16AX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_sub16BX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_sub16CX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_sub16DX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);

void b_sub32EAX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_sub32EBX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_sub32ECX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_sub32EDX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);

void b_sub64RAX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_sub64RBX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_sub64RCX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_sub64RDX(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
#pragma endregion

//extended ops
#pragma region extended_ops
void b_getInput(std::shared_ptr<void> unused_p, regs* registers, memory* mem);
void b_toString(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_mergeString(std::shared_ptr<void> unused_p, regs* registers, memory* mem);
void b_substring(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m);
void b_strlen(std::shared_ptr<void> unused_p, regs* registers, memory* mem);

void b_print(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m);
void b_println(std::shared_ptr<void> unused_p, regs* registers, memory* unused_m);
void b_printEOL(std::shared_ptr<void> unused_p, regs* unused_r, memory* unused_m);

void b_castreg(std::shared_ptr<void> receiver, regs* registers, memory* mem);
void b_recast(std::shared_ptr<void> unused_p, regs* registers, memory* mem);
void b_fromString(std::shared_ptr<void> unused_p, regs* registers, memory* mem);
#pragma endregion

// Native binary ops
#pragma region native_bin_ops
void b_not(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_log2(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_log10(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_log(std::shared_ptr<void> reg, regs* registers, memory* unused_m);
void b_and(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_or(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_xor(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_shl(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_shr(std::shared_ptr<void> reg, regs* registers, memory* mem);
void b_pow(std::shared_ptr<void> reg, regs* registers, memory* mem);
#pragma endregion

template<typename ret, typename argT, typename argT2>
struct function {
	function(ret(*ptr)(argT, argT2)) {
		this->stored = ptr;
	}

	void execute(argT a, argT2 b) {
		this->stored(a, b);
	}
private:
	ret(*stored)(argT, argT2);
};

struct registries_ptr_table {
public:
	registries_ptr_table(regs* _registers) {
		this->registers = _registers;
		this->init();
	}

#pragma warning (push)
#pragma warning (disable:26812)
	void* access(registries_def reg_id) {
		return this->table[reg_id];
	}
#pragma warning (pop)

private:
	regs* registers;
	void* table[0x000088 + 1];

	void init() {
		table[registries_def::AX] = registers->ax;
		table[registries_def::BX] = registers->bx;
		table[registries_def::CX] = registers->cx;
		table[registries_def::DX] = registers->dx;

		table[registries_def::EAX] = registers->eax;
		table[registries_def::EBX] = registers->ebx;
		table[registries_def::ECX] = registers->ecx;
		table[registries_def::EDX] = registers->edx;

		table[registries_def::RAX] = registers->rax;
		table[registries_def::RBX] = registers->rbx;
		table[registries_def::RCX] = registers->rcx;
		table[registries_def::RDX] = registers->rdx;
	}
};

struct extra_registries_ptr_table {
public:
	extra_registries_ptr_table(regs* _registers) {
		this->registers = _registers;
		this->init();
	}
#pragma warning (push)
#pragma warning (disable : 26812)
	void* access(extra_registries reg_id) {
		return this->table[reg_id];
	}
#pragma warning (pop)
private:
	regs* registers;
	void* table[0x000009 + 1];

	void init() {
		table[extra_registries::SR] = registers->sr;
	}
};

/*
struct registry_symbols {
public:
	registry_symbols(regs* _registry) {
		this->registry = _registry;
		this->init();
	}

#pragma warning (push)
#pragma warning (disable:26812)
	void execute(regs_actions action, void* value_ptr) {
		((void*(*)(void*, regs*))this->b_db[action])(value_ptr, this->registry);

		//function<void, unsigned short*, regs*> test = function<void, unsigned short*, regs*>(b_set16AX);
		//test.execute((unsigned short*)value_ptr, this->registry);
	}
#pragma warning (pop)

private:
	regs* registry;
	void* b_db[0x003E00 + 0x1];

	void init() {
#pragma region b_set
		b_db[regs_actions::setAX] = b_set16AX;
		b_db[regs_actions::setBX] = b_set16BX;
		b_db[regs_actions::setCX] = b_set16CX;
		b_db[regs_actions::setDX] = b_set16DX;

		b_db[regs_actions::setEAX] = b_set32EAX;
		b_db[regs_actions::setEBX] = b_set32EBX;
		b_db[regs_actions::setECX] = b_set32ECX;
		b_db[regs_actions::setEDX] = b_set32EDX;

		b_db[regs_actions::setRAX] = b_set64RAX;
		b_db[regs_actions::setRBX] = b_set64RBX;
		b_db[regs_actions::setRCX] = b_set64RCX;
		b_db[regs_actions::setRDX] = b_set64RDX;

		b_db[regs_actions::setSR] = b_setSR;
#pragma endregion
#pragma region b_get
		b_db[regs_actions::getAX] = b_get16AX;
		b_db[regs_actions::getBX] = b_get16BX;
		b_db[regs_actions::getCX] = b_get16CX;
		b_db[regs_actions::getDX] = b_get16DX;

		b_db[regs_actions::getEAX] = b_get32EAX;
		b_db[regs_actions::getEBX] = b_get32EBX;
		b_db[regs_actions::getECX] = b_get32ECX;
		b_db[regs_actions::getEDX] = b_get32EDX;

		b_db[regs_actions::getRAX] = b_get64RAX;
		b_db[regs_actions::getRBX] = b_get64RBX;
		b_db[regs_actions::getRCX] = b_get64RCX;
		b_db[regs_actions::getRDX] = b_get64RDX;

		b_db[regs_actions::getSR] = b_getSR;
#pragma endregion
#pragma region b_mov
		b_db[regs_actions::movAX] = b_mov16AX;
		b_db[regs_actions::movBX] = b_mov16BX;
		b_db[regs_actions::movCX] = b_mov16CX;
		b_db[regs_actions::movDX] = b_mov16DX;

		b_db[regs_actions::movEAX] = b_mov32EAX;
		b_db[regs_actions::movEBX] = b_mov32EBX;
		b_db[regs_actions::movECX] = b_mov32ECX;
		b_db[regs_actions::movEDX] = b_mov32EDX;

		b_db[regs_actions::movRAX] = b_mov64RAX;
		b_db[regs_actions::movRBX] = b_mov64RBX;
		b_db[regs_actions::movRCX] = b_mov64RCX;
		b_db[regs_actions::movRDX] = b_mov64RDX;
#pragma endregion
		b_db[regs_actions::inc] = b_inc;
		b_db[regs_actions::dec] = b_dec;
#pragma region b_mul
		b_db[regs_actions::mulAX] = b_mul16AX;
		b_db[regs_actions::mulBX] = b_mul16BX;
		b_db[regs_actions::mulCX] = b_mul16CX;
		b_db[regs_actions::mulDX] = b_mul16DX;

		b_db[regs_actions::mulEAX] = b_mul32EAX;
		b_db[regs_actions::mulEBX] = b_mul32EBX;
		b_db[regs_actions::mulECX] = b_mul32ECX;
		b_db[regs_actions::mulEDX] = b_mul32EDX;

		b_db[regs_actions::mulRAX] = b_mul64RAX;
		b_db[regs_actions::mulRBX] = b_mul64RBX;
		b_db[regs_actions::mulRCX] = b_mul64RCX;
		b_db[regs_actions::mulRDX] = b_mul64RDX;
#pragma endregion
#pragma region b_div
		b_db[regs_actions::divAX] = b_div16AX;
		b_db[regs_actions::divBX] = b_div16BX;
		b_db[regs_actions::divCX] = b_div16CX;
		b_db[regs_actions::divDX] = b_div16DX;

		b_db[regs_actions::divEAX] = b_div32EAX;
		b_db[regs_actions::divEBX] = b_div32EBX;
		b_db[regs_actions::divECX] = b_div32ECX;
		b_db[regs_actions::divEDX] = b_div32EDX;

		b_db[regs_actions::divRAX] = b_div64RAX;
		b_db[regs_actions::divRBX] = b_div64RBX;
		b_db[regs_actions::divRCX] = b_div64RCX;
		b_db[regs_actions::divRDX] = b_div64RDX;
#pragma endregion
	}
};
*/