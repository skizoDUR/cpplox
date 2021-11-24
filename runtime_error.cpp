#ifndef LOX_RUNTIME_ERROR
#define LOX_RUNTIME_ERROR
#include <exception>
#include "token.cpp"

struct runtime_exception : public std::exception {
	token Token;
	std::string msg;
public:
	runtime_exception(token &Token, std::string msg) :
		Token(Token), msg(msg) {};
	runtime_exception() {};

};


#endif
