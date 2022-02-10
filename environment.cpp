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
		return; 
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

void environment::operator=(const environment &other)
{
	this->values = other.values;
	this->enclosing = other.enclosing;
}
std::any environment::get_at(int distance, std::string name)
{
	return ancestor(distance).values[name];
}
environment &environment::ancestor(int distance)
{
	auto node = this;
	for (auto i = 0; i < distance; i++)
		node = node->enclosing.data;
	return *node;
}
void environment::assign_at(int distance, std::string name, std::any value)
{
	ancestor(distance).values[name] = value;
}
