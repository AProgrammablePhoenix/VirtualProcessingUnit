#pragma once

#include "../CursesWrapper/wrapper.hpp"

#include <chrono>
#include <iostream>
#include <string>

template<auto f_ptr>
class TimeWrapper {};

template<class Ret, class... Args, auto (*f_ptr)(Args...)->Ret>
class TimeWrapper<f_ptr> {
public:
	auto operator()(Args... args) {
		this->timer_begin = std::chrono::high_resolution_clock::now();
		auto ret = f_ptr(args...);
		this->timer_end = std::chrono::high_resolution_clock::now();

		return ret;
	};
	long long getTimeDiff() const {
		auto begin = std::chrono::time_point_cast<std::chrono::microseconds>(this->timer_begin).time_since_epoch().count();
		auto end = std::chrono::time_point_cast<std::chrono::microseconds>(this->timer_end).time_since_epoch().count();

		auto time_diff = end - begin;
		return time_diff;
	}

	operator long long() const {
		return this->getTimeDiff();
	};
	operator std::string() const {
		return std::to_string((long long)this);
	}
private:
	std::chrono::high_resolution_clock::time_point timer_begin;
	std::chrono::high_resolution_clock::time_point timer_end;
};

class ScopeTimer {
public:
	ScopeTimer() {
		this->timer_begin = std::chrono::high_resolution_clock::now();
	}
	ScopeTimer(const std::string& _end_msg) {
		this->end_msg = _end_msg;
		this->timer_begin = std::chrono::high_resolution_clock::now();
	}

	~ScopeTimer() {
		this->timer_end = std::chrono::high_resolution_clock::now();

		auto begin = std::chrono::time_point_cast<std::chrono::microseconds>(this->timer_begin).time_since_epoch().count();
		auto end = std::chrono::time_point_cast<std::chrono::microseconds>(this->timer_end).time_since_epoch().count();

		auto time_diff = end - begin;

		nstd::ncout << this->end_msg << time_diff << nstd::nendl;
	}
private:
	std::string end_msg = "Time to execute scope (microseconds): ";

	std::chrono::high_resolution_clock::time_point timer_begin;
	std::chrono::high_resolution_clock::time_point timer_end;
};