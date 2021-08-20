#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "compiler.h"

// used to force exit if std::exit(int ec) failed
#define __CEXIT(...) throw std::runtime_error("Program exited due to compilation error(s)\n");

// General asserts

[[noreturn]] inline void assert_err(const std::string& s, int ec) noexcept(false) {
	std::cerr << s << std::endl;
	std::exit(ec);
	__CEXIT();
}
[[noreturn]] inline void assert_err(const std::string& prefix, const std::string& inst, const std::vector<std::string>& args, int ec) noexcept(false) {
	std::cerr << prefix << inst << " ";
	for (size_t i = 0; i < args.size(); ++i) {
		if (!i)
			std::cerr << args[i];
		else
			std::cerr << "," << args[i];
	}
	std::cerr << std::endl;
	__CEXIT();
}
[[noreturn]] inline void assert_err(const std::string& prefix, const std::string& inst, const std::vector<token>& args, int ec) noexcept(false) {
	std::cerr << prefix << inst << " ";
	for (size_t i = 0; i < args.size(); ++i) {
		if (!i)
			std::cerr << args[i].element;
		else
			std::cerr << "," << args[i].element;
	}
	std::cout << std::endl;
	__CEXIT();
}

// Specialized asserts

[[noreturn]] inline void assert_err_wrnargs(const uint64_t& nargs, const std::string& inst, const std::vector<token>& args) noexcept(false) {
	assert_err("Wrong numbers of arguments (only " + std::to_string(nargs) + " expected): ", inst, args, WRONGNARGS);
}
[[noreturn]] inline void assert_err_argv(const std::vector<std::string>& args_t, const std::string& inst, const std::vector<token>& args) noexcept(false) {
	std::string prefix = "Incorrect argument(s) type(s) (should be ";
	for (size_t i = 0; i < args_t.size(); ++i) {
		if (!i)
			prefix += "<" + args_t[i] + ">";
		else
			prefix += ",<" + args_t[i] + ">";
	}
	prefix += "): ";
	assert_err(prefix, inst, args, ARGV_ERROR);
}
[[noreturn]] inline void assert_err_reguse(const std::string& inst, const std::vector<token>& args) noexcept(false) {
	assert_err("Illegal usage of register: ", inst, args, ARGV_ERROR);
}
[[noreturn]] inline void assert_err_nonimpl(const std::string& inst, const std::vector<token>& args) noexcept(false) {
	assert_err("Usage of non implemented operation: ", inst, args, ARGV_ERROR);
}