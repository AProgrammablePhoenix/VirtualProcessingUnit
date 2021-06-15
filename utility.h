#pragma once

#include <iostream>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#define ISWIN 1
#endif

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
	memcpy(*output, &value, sizeof(double));
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
	memcpy(&ret, _array, sizeof(double));
#endif

	return ret;
}