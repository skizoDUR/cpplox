#ifndef ENVIRONMENT_HPP
#define ENVIRONMENT_HPP
#include "token.cpp"
#include <any>
#include <string>
#include <unordered_map>

class environment {
	std::unordered_map<std::string, std::any> values;
public:
	environment *enclosing = nullptr;
	environment(environment *enclosing) : enclosing(enclosing) {};
	environment() {};
	void assign(token name, std::any value);
	void define(std::string , std::any);
	std::any get(token &);
};
#endif
