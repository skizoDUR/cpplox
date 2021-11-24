#ifndef LOX_CALLABLE_HPP
#define LOX_CALLABLE_HPP
#include <any>
#include <vector>
#include "interpreter.hpp"
template <typename T>
class lox_callable {
public:
	lox_callable() = default;
	virtual ~lox_callable() = default;
	virtual T call(interpreter<T> &interpreter, std::vector<T> &arguments);
	virtual int arity() {};
};

#endif
