#pragma once

#include <iostream>
#include <memory>
#include <tuple>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#define ISWIN 1
#else
	#if defined(__linux__)
		#include <cstring>
		#include <stdio.h>
	#endif
#endif

typedef std::tuple<size_t, size_t> arg_tuple;

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

template<typename Tin, typename Tout> inline void mp_memcpy(Tin& _in, Tout& _out) {
#if defined(ISWIN)
	memcpy_s(&_out, sizeof(Tout), &_in, sizeof(Tin));
#else
	std::memcpy(&_out, &_in, sizeof(&_out));
#endif
}
template<typename Tin, typename Tout> inline void mp_memcpy(Tin& _in, Tout& _out, size_t count) {
#if defined(ISWIN)
	memcpy_s(&_out, count, &_in, count);
#else
	std::memcpy(&_out, &_in, count);
#endif
}
template<> inline void mp_memcpy<size_t, unsigned char*>(size_t& _in, unsigned char*& _out) {
#if defined(ISWIN)
	memcpy_s(_out, sizeof(size_t), &_in, sizeof(size_t));
#else
	std::memcpy(_out, _in, sizeof(size_t));
#endif
}
template<> inline void mp_memcpy<unsigned char*, size_t>(unsigned char*& _in, size_t& _out) {
#if defined(ISWIN)
	memcpy_s(&_out, sizeof(size_t), _in, sizeof(size_t));
#else
	std::memcpy(&_out, &_in, sizeof(size_t));
#endif
}
template<typename Tin> inline void mp_memcpy(Tin& _in, unsigned char* _out, size_t count = sizeof(Tin)) {
#if defined(ISWIN)
	memcpy_s(_out, count, &_in, count);
#else
	std::memcpy(_out, &_in, count);
#endif
}
template<typename Tout> inline void mp_memcpy(unsigned char* _in, Tout& _out, size_t count = sizeof(Tout)) {
#if defined(ISWIN)
	memcpy_s(&_out, count, _in, count);
#else
	std::memcpy(&_out, _in, count);
#endif
}