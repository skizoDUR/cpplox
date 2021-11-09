#ifndef SCANNER_H
#define SCANNER_H
#include "token_type.hpp"
#include "token.cpp"
#include <string>
#include <vector>
#include <unordered_map>

class scanner
{
public:
	static const std::unordered_map<std::string, token_type> keywords;
	int start = 0, current = 0, line = 1;
	std::string source;
	std::vector<token> tokens;
	bool is_at_end();
	char peek();
	char peek_next();
	char advance();
	bool match(char c);
	bool isDigit(char c);
	bool isAlpha(char c);
	bool isAlphanumeric(char c);
	void add_token(token_type t);
	void add_token(token_type t, std::string lit);
	void add_token(token_type t, double lit);
	void string();
	void identifier();
	void number();
	void scan_token();
public:
	scanner(std::string source) : source(source) {};
	std::vector<token> scan_tokens();
};

#endif
