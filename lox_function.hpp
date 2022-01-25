#ifndef LOX_FUNCTION_HPP
#define LOX_FUNCTION_HPP
#include "environment.hpp"
#include "stmt.hpp"
#include <functional>
#include <vector>
#include <iostream>

template <typename T>
class interpreter;

template<typename T>
class lox_function {
public:
	

	std::function<T(lox_function &, interpreter<T> &, std::vector<T> &)> call_f;
	std::function<int(lox_function &)> arity_f;
	environment *closure = nullptr;
	Function<T> *function_decl;

	int arity()
	{
		return arity_f(*this);
	}
	
	T call(interpreter<T> &Interpreter, std::vector<T> &arguments)
	{
		return call_f(*this, Interpreter, arguments);
	}

	lox_function<T>(std::function<T(lox_function &, interpreter<T> &, std::vector<T> &)> call_f,
			std::function<int(lox_function &)> arity_f,
			environment *closure,
			Function<T> *function_decl) :
		call_f(call_f),
		arity_f(arity_f),
		closure(new environment(*closure)),
		function_decl(function_decl) {}
	lox_function() {}
	lox_function &operator=(const lox_function &other)
	{
		if (this == &other)
			return *this;
		this->call_f = other.call_f;
		this->arity_f = other.arity_f;
		this->closure = new environment(*other.closure);
		this->function_decl = other.function_decl;
		return *this;
	}
	lox_function(const lox_function &other)
	{
		operator=(other);
	}
	~lox_function()
	{
		delete this->closure;
	}
};

#endif
