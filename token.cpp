#ifndef TOKEN_CPP
#define TOKEN_CPP
#include "token_type.hpp"
#include <ostream>
#include <string>
#include <any>
#include <type_traits>

class token {
public:
	token_type type;
	std::string lexeme;
	std::any literal;
	int line;

	token() {}
	
	token(token_type type, std::string lexeme, double literal, int line)
		: type(type), lexeme(lexeme), literal(literal), line(line) {};
	token(token_type type, std::string lexeme, std::string literal, int line)
		: type(type), lexeme(lexeme), literal(literal), line(line) {};
	//friend std::ostream &operator<<(std::ostream &, const token &);
	
	friend std::ostream &operator<<(std::ostream &os, const token &t)
	{	
		if (t.literal.type() == typeid(double))
			os << static_cast<int>(t.type) << ": " << std::any_cast<double>(t.literal) << " " << t.lexeme << " line[" << t.line << "]";
		else
			os << static_cast<int>(t.type) << ": " << std::any_cast<std::string>(t.literal) << " " << t.lexeme << " line[" << t.line << "]";

			      return os;
	}

};


#endif
