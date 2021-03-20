#pragma once

#include <iostream>
#include <string>
#include <vector>

#pragma warning (push)
#pragma warning (disable : 4293)
#pragma warning (disable : 4554)

template<typename T>
struct reg_int {
public:
	virtual void set(T value) {
		return;
	}
	virtual T get() {
		if (typeid (T) == typeid (std::string)) {
#pragma warning (push)
#pragma warning (disable : 4311)
#pragma warning (disable : 4302)
			return (T)"";
		}
		else {
#pragma warning (disable : 6387)
			return (T)NULL;
#pragma warning (pop)
		}
	}
};

struct baseReg : public reg_int<unsigned char> {
public:
	unsigned char* v;

	baseReg() {
		v = NULL;
	}
	baseReg(unsigned char* value_ptr) {
		this->v = value_ptr;
	}

	void set(unsigned char value) {
		*(this->v) = value;
	}

	unsigned char get() {
		return *(this->v);
	}
};

struct extendedReg : public reg_int<unsigned short> {
public:
	unsigned char *l, *h;

	extendedReg() {
		l = NULL;
		h = NULL;
	}
	extendedReg(unsigned char* low, unsigned char* high) {
		this->l = low;
		this->h = high;
	}

	void set(unsigned short value) {
		unsigned char low = 0;
		unsigned char high = 0;

		low = (value >> 0) & ~(~0 << (7 - 0 + 1));
		high = (value >> 8) & ~(~0 << (15 - 8 + 1));

		*l = low;
		*h = high;
	}

	unsigned short get() {
		unsigned short value;

		value = *h;
		value <<= 8;
		value += *l;

		return value;
	}
};

struct extendedEXReg : public reg_int<unsigned int> {
public:
	extendedReg* l;
	unsigned short* h;

	extendedEXReg() {
		l = NULL;
		h = NULL;
	}
	extendedEXReg(extendedReg* low, unsigned short* high) {
		this->l = low;
		this->h = high;
	}

	void set(unsigned int value) {
		unsigned short low = 0;
		unsigned short high = 0;

		low = (value >> 0) & ~(~0 << (15 - 0 + 1));
		high = (value >> 16) & ~(~0 << 31 - 16 + 1);

		l->set(low);
		*h = high;
	}

	unsigned int get() {
		unsigned int value;

		value = *h;
		value <<= 16;
		value += l->get();

		return value;
	}
};

struct extendedEEXReg : public reg_int<unsigned long long> {
	extendedEXReg* l;
	unsigned int* h;

	extendedEEXReg() {
		l = NULL;
		h = NULL;
	}
	extendedEEXReg(extendedEXReg* low, unsigned int* high) {
		l = low;
		h = high;
	}

	void set(unsigned long long value) {
		unsigned int low;
		unsigned int high;

		low = (value >> 0) & ~(~0 << (31 - 0 + 1));
		high = (value >> 32) & ~(~0 << (63 - 32 + 1));

		l->set(low);
		*h = high;
	}

	unsigned long long get() {
		unsigned long long value;

		value = *h;
		value <<= 32;
		value += l->get();

		return value;
	}
};

struct stringReg : reg_int<std::string> {
	stringReg() {
		value = "";
	}
	stringReg(std::string str) {
		this->value = str;
	}

	void set(std::string str) {
		this->value = str;
	}
	std::string get() {
		return this->value;
	}
private:
	std::string value;
};

struct regs {
public:
	baseReg
		*al = &_al,
		*ah = &_ah,
		*bl = &_bl,
		*bh = &_bh,
		*cl = &_cl,
		*ch = &_ch,
		*dl = &_dl,
		*dh = &_dh;

	extendedReg
		*ax = &_ax,
		*bx = &_bx,
		*cx = &_cx,
		*dx = &_dx;

	extendedEXReg
		*eax = &_eax,
		*ebx = &_ebx,
		*ecx = &_ecx,
		*edx = &_edx;

	extendedEEXReg
		*rax = &_rax,
		*rbx = &_rbx,
		*rcx = &_rcx,
		*rdx = &_rdx;

	unsigned long long* process_step = &_process_step;
	unsigned char* cmp_out = &_cmp_out;
	short* jmp_sign = &_jmp_sign;
	bool* stopRequested = &_stopRequested;
	unsigned long long *process_call_address = &_process_call_address;

	stringReg* sr = &_sr;
	stringReg* structPtr = &_structPtr;
private:
	unsigned char c_al, c_ah, c_bl, c_bh, c_cl, c_ch, c_dl, c_dh;

	baseReg
		_al = baseReg(&c_al),
		_ah = baseReg(&c_ah),
		_bl = baseReg(&c_bl),
		_bh = baseReg(&c_bh),
		_cl = baseReg(&c_cl),
		_ch = baseReg(&c_ch),
		_dl = baseReg(&c_dl),
		_dh = baseReg(&c_dh);

	extendedReg
		_ax = extendedReg(&c_al, &c_ah),
		_bx = extendedReg(&c_bl, &c_bh),
		_cx = extendedReg(&c_cl, &c_ch), 
		_dx = extendedReg(&c_dl, &c_dh);

	unsigned short eax_h, ebx_h, ecx_h, edx_h;

	extendedEXReg
		_eax = extendedEXReg(&_ax, &eax_h),
		_ebx = extendedEXReg(&_bx, &ebx_h),
		_ecx = extendedEXReg(&_cx, &ecx_h),
		_edx = extendedEXReg(&_dx, &edx_h);

	unsigned int rax_h, rbx_h, rcx_h, rdx_h;

	extendedEEXReg
		_rax = extendedEEXReg(&_eax, &rax_h),
		_rbx = extendedEEXReg(&_ebx, &rbx_h),
		_rcx = extendedEEXReg(&_ecx, &rcx_h),
		_rdx = extendedEEXReg(&_edx, &rdx_h);

	unsigned long long _process_step = 0;
	unsigned char _cmp_out = 0xFF;
	short _jmp_sign = 1;
	bool _stopRequested = false;
	unsigned long long _process_call_address = 0;

	stringReg _sr = stringReg();
	stringReg _structPtr = stringReg();
};

#pragma warning (pop)