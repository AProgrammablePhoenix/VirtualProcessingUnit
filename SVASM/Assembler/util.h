#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "../../utility.h"
#include "compiler.h"

extern std::map<uint64_t, uint64_t> vline_rline;
extern std::string source_file;

// used to force exit if std::exit(int ec) failed
#define __CEXIT(...) throw std::runtime_error("Program exited due to compilation error(s)\n");

// forced to do that since g++ and clang don't support c++ std::format feature yet
#if defined(ISWIN)
	#include <format>
	#define __CFMT(nline) std::format("({})", nline)
#else
	#include <cstdio>
	#define __CFMT(nline) "(" + std::to_string(nline) + ")"

#endif
#define __CASRTHDR(nline) source_file << __CFMT(nline) << " => "

// General asserts

[[noreturn]] inline void assert_err(const std::string& s, uint64_t nline, int ec) noexcept(false) {
	nstd::ncout << __CASRTHDR(nline) << s << nstd::nendl;
	std::exit(ec);
	__CEXIT();
}
[[noreturn]] inline void assert_err(const std::string& prefix, const std::string& inst, const std::vector<std::string>& args, uint64_t nline, int ec) noexcept(false) {
	nstd::ncout << __CASRTHDR(nline) << prefix << inst << " ";
	for (size_t i = 0; i < args.size(); ++i) {
		if (!i)
			nstd::ncout << args[i];
		else
			nstd::ncout << "," << args[i];
	}
	nstd::ncout << nstd::nendl;
	std::exit(ec);
	__CEXIT();
}
[[noreturn]] inline void assert_err(const std::string& prefix, const std::string& inst, const std::vector<token>& args, uint64_t nline, int ec) noexcept(false) {
	nstd::ncout << __CASRTHDR(nline) << prefix << inst << " ";
	for (size_t i = 0; i < args.size(); ++i) {
		if (!i)
			nstd::ncout << args[i].element;
		else
			nstd::ncout << "," << args[i].element;
	}
	nstd::ncout << nstd::nendl;
	std::exit(ec);
	__CEXIT();
}

// Specialized asserts

[[noreturn]] inline void assert_err_wrnargs(const uint64_t& nargs, const std::string& inst, const std::vector<token>& args, uint64_t nline) noexcept(false) {
	assert_err("Wrong numbers of arguments (only " + std::to_string(nargs) + " expected): ", inst, args, nline, WRONGNARGS);
}
[[noreturn]] inline void assert_err_argv(const std::vector<std::string>& args_t, const std::string& inst, const std::vector<token>& args, uint64_t nline) noexcept(false) {
	std::string prefix = "Incorrect argument(s) type(s) (should be ";
	for (size_t i = 0; i < args_t.size(); ++i) {
		if (!i)
			prefix += "<" + args_t[i] + ">";
		else
			prefix += ",<" + args_t[i] + ">";
	}
	prefix += "): ";
	assert_err(prefix, inst, args, nline, ARGV_ERROR);
}
[[noreturn]] inline void assert_err_reguse(const std::string& inst, const std::vector<token>& args, uint64_t nline) noexcept(false) {
	assert_err("Illegal usage of register: ", inst, args, nline, ARGV_ERROR);
}
[[noreturn]] inline void assert_err_nonimpl(const std::string& inst, const std::vector<token>& args, uint64_t nline) noexcept(false) {
	assert_err("Usage of non implemented operation: ", inst, args, nline, ARGV_ERROR);
}
