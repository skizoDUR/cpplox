#include "lox_function.hpp"
#include "interpreter.hpp"

template <>
int lox_function<std::any>::arity()
{
	return declaration.params.size();
}

template <>
std::any lox_function<std::any>::call(interpreter<std::any> &interpret, std::vector<std::any> &arguments)
{
	environment Environment(&closure);

	for (int i = 0; i < (int)declaration.params.size(); i++)
		Environment.define(declaration.params.at(i).lexeme, arguments.at(i));

	try {
		interpret.execute_block(declaration.body, Environment);
	}
	catch (Return &value) {
		return value.value;
	}
	return {};
}
