#ifndef CLOCK_HPP
#define CLOCK_HPP
#include "lox_callable.hpp"
#include <ctime>
template <typename T>
struct Clock final : public lox_callable<T> {
	int arity() override
	{
		return 0;
	}
	T call(interpreter<T> &interpreter, std::vector<T> &arguments) override
	{
		return (double)time(0);
	}
	~Clock() override = default;
};

#endif
