#ifndef RETURN_HPP
#define RETURN_HPP
#include <exception>
#include <any>
struct Return_value : std::exception {
public:
	std::any value;
	Return_value(std::any &value) :
		value(value) {}
};
#endif
