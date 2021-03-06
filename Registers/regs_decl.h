#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

inline constexpr bool is_maxtype_valid() {
	size_t maxtype_size = sizeof(size_t);

	//check whether greater than or equal to 4 or not
	if (maxtype_size < 4)
		return false;

	//check whether power of two or not
	while (maxtype_size != 1) {
		if (maxtype_size % 2)
			return false;
		maxtype_size /= 2;
	}
	return true;
}

#define MAXTYPECHK() \
	static_assert(is_maxtype_valid(), "Your platform uses an unsupported size for type: size_t")

template<typename T> struct reg_int {
public:
	virtual void set(T value) {
		return;
	}
	virtual T get() {
		return (T)NULL;
	}
};
template<> struct reg_int<std::string> {
public:
	virtual void set(std::string value) {
		return;
	}
	virtual std::string get() {
		return "";
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

		low = (value >> 0U) & ~(~0 << 8U);
		high = (value >> 8U) & ~(~0 << 8U);

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

		low = (value >> 0UL) & ~(~0 << 16UL);
		high = (value >> 16UL) & ~(~0 << 16UL);

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

struct extendedEEXReg : public reg_int<size_t> {
public:
	extendedEXReg* l;
	unsigned int* h;

	extendedEEXReg() {
		MAXTYPECHK();

		l = NULL;
		h = NULL;
	}
	extendedEEXReg(extendedEXReg* low, unsigned int* high) {
		MAXTYPECHK();

		l = low;
		h = high;
	}

	void set(size_t value) {
		unsigned int low;
		unsigned int high;

		if constexpr (sizeof(size_t) >= 8) {
			low = ((uint64_t)value >> 0ULL) & ~(~0ULL << 32ULL);
			high = ((uint64_t)value >> 32ULL) & ~(~0ULL << 32ULL);
		} else if constexpr (sizeof(size_t) <= 4) {
			low = (unsigned int)value;
			high = 0;
		}

		l->set(low);
		*h = high;
	}
	size_t get() {
		size_t value;

		if constexpr (sizeof(size_t) >= 8) {
			value = *h;
			value <<= 32;
			value += l->get();
		} else if constexpr (sizeof(size_t) <= 4) {
			value = l->get();
		}

		return value;
	}
};

struct stringReg : reg_int<std::string> {
public:
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

struct charReg : reg_int<char> {
public:
	charReg() {
		this->value = '\0';
	}
	charReg(char c) {
		this->value = c;
	}

	void set(char c) {
		this->value = c;
	}
	char get() {
		return this->value;
	}
private:
	char value;
};

template<typename datatype>
struct OrphanReg : public reg_int<datatype> {
public:
	OrphanReg() = delete;
	OrphanReg(datatype dt) {
		this->value = dt;
	}

	void set(datatype dt) {
		this->value = dt;
	}
	datatype get() {
		return this->value;
	}

	datatype& data() { return this->value; }

	operator datatype& () { return this->value; }
	operator datatype() const { return this->value; }
private:
	datatype value;
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

	OrphanReg<size_t>
		*rbp = &_rbp,
		*rsp = &_rsp,
		*rsbgn = &_rsbgn,
		*rsend = &_rsend,
		*rdi = &_rdi,
		*rsi = &_rsi;

	OrphanReg<float>
		*fpr0 = &_fpr0,
		*fpr1 = &_fpr1,
		*fpr2 = &_fpr2,
		*fpr3 = &_fpr3;

	OrphanReg<double>
		*efpr0 = &_efpr0,
		*efpr1 = &_efpr1,
		*efpr2 = &_efpr2,
		*efpr3 = &_efpr3;

	OrphanReg<long double>
		*rfpr0 = &_rfpr0,
		*rfpr1 = &_rfpr1,
		*rfpr2 = &_rfpr2,
		*rfpr3 = &_rfpr3;

	size_t* process_step = &_program_counter;
	unsigned char* cmp_out = &_cmp_out;
	bool* stopRequested = &_stopRequested;
	size_t *process_call_address = &_process_call_address;
	uint8_t exec_level : 2 = 0; // 0: kernel mode | 1-2: intermediate | 3: user mode
	bool IF = true; // Interrupts flag

	charReg* cr = &_cr;
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

	OrphanReg<size_t>
		_rbp = 0,
		_rsp = 0,
		_rsbgn = 0,
		_rsend = 0,
		_rdi = 0,
		_rsi = 0;

	OrphanReg<float>
		_fpr0 = 0,
		_fpr1 = 0,
		_fpr2 = 0,
		_fpr3 = 0;

	OrphanReg<double>
		_efpr0 = 0,
		_efpr1 = 0,
		_efpr2 = 0,
		_efpr3 = 0;

	OrphanReg<long double>
		_rfpr0 = 0,
		_rfpr1 = 0,
		_rfpr2 = 0,
		_rfpr3 = 0;

	size_t _program_counter = 0;
	unsigned char _cmp_out = 0xFF;
	bool _stopRequested = false;
	size_t _process_call_address = 0;

	charReg _cr = charReg();
	stringReg _structPtr = stringReg();
};