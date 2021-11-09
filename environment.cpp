#include "environment.hpp"
#include "lox.hpp"
#include "runtime_error.cpp"

void environment::define(std::string name, std::any value)
{
	this->values[name] = value;
}

void environment::assign(token name, std::any value)
{
	if (values.contains(name.lexeme)) {
		values[name.lexeme] = value;
		return;
	}
	if (this->enclosing) {
		this->enclosing->assign(name, value);
		return; //unnecesary but more clear
	}

	throw runtime_exception(name, "Undefined variable '" + name.lexeme + "'.");
}

std::any environment::get(token &t)
{
	if (this->values.contains(t.lexeme))
		return this->values[t.lexeme];

	if (this->enclosing)
		return this->enclosing->get(t);
		
	throw runtime_exception(t, "Undefined reference to " + t.lexeme + ".");
}

