#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP
#include "token.cpp"
#include <any>
#include <string>
#include <unordered_map>
#include <memory>

class environment {
	std::unordered_map<std::string, std::any> values;
public:
	environment *enclosing = nullptr;
	environment(environment *enclosing) : enclosing(enclosing) {}
	environment(const environment &other)
	{
		this->values = other.values;
		this->enclosing = other.enclosing;
	}
	environment() {};
	void assign(token name, std::any value);
	void define(std::string , std::any);
	std::any get(token &);
	std::any get_at(int distance, std::string name);
	void assign_at(int distance, std::string name, std::any value);
	environment &ancestor(int distance);
	void operator=(const environment &);
};
#endif
