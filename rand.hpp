#ifndef RAND_HPP
#define RAND_HPP
#include <cstdlib>
#include <ctime>
#include "lox_callable.hpp"
template <typename T>
struct Rand final : public lox_callable<T> {
	Rand()
	{
		srand(time(0));
	}
	int arity() override
	{
		return 0;
	}
	T call(interpreter<T> &interpreter, std::vector<T> &arguments) override
	{
		return (double)rand();
	}
	~Rand() override = default;
};
#endif
