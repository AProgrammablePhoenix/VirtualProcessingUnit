#include <iostream>
#include <string>

#include "../CursesWrapper/wrapper.hpp"

template<typename T>
inline void dump_array_raw(T* begin, T* end) {
	for (T* i = begin; i < end; ++i)
		nstd::ncout << *i;
	nstd::ncout << nstd::nendl;
}