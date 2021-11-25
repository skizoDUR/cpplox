#ifndef LOX_FUNCTION_HPP
#define LOX_FUNCTION_HPP
#include "environment.hpp"
#include "stmt.hpp"
#include "lox.hpp"

template <typename T>
class interpreter;

template <typename T>
class lox_function {
public:
	Function<T> declaration;
	environment closure;
	int arity();

	lox_function(Function<T> &declaration, environment *closure) :
		declaration(declaration), closure(*closure) {}
	lox_function<T> operator=(lox_function<T> &other)
	{
		this->declaration = other.declaration;
	}
	
	T call(interpreter<T> &interpret, std::vector<T> &arguments);
};
#endif
