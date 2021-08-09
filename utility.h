#pragma once

#include <iostream>
#include <memory>
#include <tuple>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#define ISWIN 1
#else
	#if defined(__linux__) 
		#define ISLINUX 1
		#include <cstring>
		#include <stdio.h>
	#endif
	#if defined(__unix__)
		#define ISUNIX 1
	#endif
#endif

typedef std::tuple<size_t, size_t> arg_tuple;

#define CUSTOM_STD_ARGS(A, B, C) std::shared_ptr<void> A, regs* B, memory* C

inline void ULLTOA(size_t value, unsigned char** output) {
	*output = new unsigned char[sizeof(size_t)];

	for (long long j = 0, i = sizeof(size_t) - 1; i >= 0; i--, j++) {
		(*output)[j] = (value >> (i * 8)) & 0xff;
	}
}
inline void DTOA(double value, unsigned char** output) {
	*output = new unsigned char[sizeof(double)];
#if defined(ISWIN)
	memcpy_s(*output, sizeof(double), &value, sizeof(double));
#else
	std::memcpy(*output, &value, sizeof(double));
#endif
}
inline void LDTOA(long double value, unsigned char** output) {
	*output = new unsigned char[sizeof(long double)];
#if defined(ISWIN)
	memcpy_s(*output, sizeof(long double), &value, sizeof(long double));
#else
	std::memcpy(*output, &value, sizeof(long double));
#endif
}
inline size_t ATOULL(unsigned char* _array) {
	size_t ret = 0;

	for (long long i = sizeof(size_t) - 1, j = 0; i >= 0; i--, j++) {
		ret |= ((size_t)_array[j] << (i * 8));
	}

	return ret;
}
inline double ATOD(unsigned char* _array) {
	double ret = 0;

#if defined(ISWIN)
	memcpy_s(&ret, sizeof(double), _array, sizeof(double));
#else
	std::memcpy(&ret, _array, sizeof(double));
#endif

	return ret;
}
inline long double ATOLD(unsigned char* _array) {
	long double ret = 0;

#if defined(ISWIN)
	memcpy_s(&ret, sizeof(long double), _array, sizeof(long double));
#else
	std::memcpy(&ret, _array, sizeof(long double));
#endif

	return ret;
}

template<typename Tin, typename Tout> inline void mp_memcpy(Tin* _in, Tout* _out) {
#if defined(ISWIN)
	memcpy_s(_out, sizeof(Tout), _in, sizeof(Tin));
#else
	std::memcpy(_out, _in, sizeof(&_out));
#endif
}
template<typename Tin, typename Tout> inline void mp_memcpy(Tin* _in, Tout* _out, size_t count) {
#if defined(ISWIN)
	memcpy_s(_out, count, _in, count);
#else
	std::memcpy(_out, _in, count);
#endif
}
template<typename Tin> inline void mp_memcpy(Tin* _in, unsigned char* _out, size_t count = sizeof(Tin)) {
#if defined(ISWIN)
	memcpy_s(_out, count, _in, count);
#else
	std::memcpy(_out, _in, count);
#endif
}
template<typename Tout> inline void mp_memcpy(unsigned char* _in, Tout* _out, size_t count = sizeof(Tout)) {
#if defined(ISWIN)
	memcpy_s(_out, count, _in, count);
#else
	std::memcpy(_out, _in, count);
#endif
}
template<> inline void mp_memcpy<size_t, unsigned char>(size_t* _in, unsigned char* _out) {
#if defined(ISWIN)
	memcpy_s(_out, sizeof(size_t), _in, sizeof(size_t));
#else
	std::memcpy(_out, _in, sizeof(size_t));
#endif
}
template<> inline void mp_memcpy<unsigned char, size_t>(unsigned char* _in, size_t* _out) {
#if defined(ISWIN)
	memcpy_s(_out, sizeof(size_t), _in, sizeof(size_t));
#else
	std::memcpy(_out, _in, sizeof(size_t));
#endif
}

// Compress byte array, returns size of new array, with new array set to ba (passed by ref)
inline size_t COMPBA(unsigned char*& ba, unsigned char length) {
	unsigned char nZeros = 0;

	for (size_t i = 0; i < length; i++) {
		if (!ba[i])
			nZeros++;
		else
			break;
	}

	if (nZeros < length) {
		size_t newlen = length - (size_t)nZeros + 1;

		unsigned char* temp = new unsigned char[newlen];
		temp[0] = nZeros;

		std::copy(ba + nZeros, ba + length, temp + 1);
		delete[] ba;
		ba = temp;

		return newlen;
	}
	else {
		size_t newlen = 2;

		unsigned char* temp = new unsigned char[newlen];
		temp[0] = nZeros;
		temp[1] = 0;

		delete[] ba;
		ba = temp;
		return newlen;
	}
}
// Extends compressed byte array (compressed with COMPBA), with new size returned and extended array set to cba (passed by ref)
inline size_t EXTDBA(unsigned char*& cba, unsigned char length) {
	unsigned char nZeros = cba[0];

	if (cba[1] != 0) {
		size_t baselen = (size_t)nZeros + length - 1;

		unsigned char* temp = new unsigned char[baselen];
		std::fill(temp, temp + baselen, 0);

		for (size_t i = 0; i < nZeros && i < baselen; i++)
			temp[i] = 0;
		for (size_t i = 0; i + 1 < length && i + nZeros < baselen; i++)
			temp[i + nZeros] = cba[i + 1];

		delete[] cba;
		cba = temp;

		return baselen;
	}
	else {
		size_t baselen = (size_t)nZeros;

		unsigned char* temp = new unsigned char[baselen];

		for (size_t i = 0; i < nZeros; i++)
			temp[i] = 0;

		delete[] cba;
		cba = temp;

		return baselen;
	}
}