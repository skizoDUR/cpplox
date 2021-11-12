#ifndef SCANNER_CPP
#define SCANNER_CPP
#include "scanner.hpp"
#include "lox.hpp"
#include "token_type.hpp"

//const std::unordered_map<std::string, token_type> keywords {"this", token_type::THIS};

bool scanner::is_at_end()
{
	return current >= (int)source.size();
}

char scanner::peek()
{
	return source[current];
}

char scanner::peek_next()
{
	if (current + 1 >= (int)source.size())
		return '\0';
	return source[current + 1];
}
	
char scanner::advance()
{
	if (is_at_end())
		return '\0';
	return source[current++];
}
	
bool scanner::match(char c)
{
	if (peek() == c) {
		advance();
		return 1;
	}
	return 0;
}

bool scanner::isDigit(char c)
{
	return c >= '0' && c <= '9';
}

bool scanner::isAlpha(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_');
}
	
bool scanner::isAlphanumeric(char c)
{
		
	return isAlpha(c) || isDigit(c);
}

void scanner::add_token(token_type t)
{
	add_token(t, "");
}

void scanner::add_token(token_type t, std::string lit)
{
	std::string text = source.substr(start, current - start);
	tokens.emplace_back(t, text, lit, line);
}

void scanner::add_token(token_type t, double lit)
{
	std::string text = source.substr(start, current - start);
	tokens.emplace_back(t, text, lit, line);
}

void scanner::string()
{
	while (!is_at_end() && peek() != '"') {
		if (peek() == '\n')
			line++;
		advance();
	}
	if (is_at_end()) {
		lox::error(line, "Unterminated String.\n");
		return;
	}
	//"abc" -> abc
	std::string lit = source.substr(start + 1, current - start - 1); 
	advance(); //discards ending "
	add_token(token_type::STRING, lit);
		
	//std::string text = source.substr(start, current);
}

void scanner::identifier()
{
	while (isAlphanumeric(peek()) && !is_at_end()) advance();
	std::string text = source.substr(start, current - start);
	token_type type;
	if (text == "pi") {
		type = token_type::NUMBER;
		add_token(type, 3.141592654);
		return;
	}
	if (text == "e") {
		type = token_type::NUMBER;
		add_token(type, 2.71828182);
		return;
	}
	auto find = keywords.find(text);
	if (find == keywords.end())
		type = token_type::IDENTIFIER;
	else type = find->second;
	add_token(type);
}
	
void scanner::number()
{
	while (!is_at_end() && isdigit(peek()))
		advance();
	if (peek() == '.') {
		advance();
		while (!is_at_end() && isdigit(peek())) //peek_next()
			advance();
	}
	
	double lit = std::stod(source.substr(start, current - start));
	add_token(token_type::NUMBER, lit);
}
		
void scanner::scan_token()
{
	char c = advance();
	switch (c) {

	case '(': add_token(token_type::LEFT_PAREN); break;
	case ')': add_token(token_type::RIGHT_PAREN); break;
	case '{': add_token(token_type::LEFT_BRACE); break;
	case '}': add_token(token_type::RIGHT_BRACE); break;
	case ',': add_token(token_type::COMMA); break;
	case '.': add_token(token_type::DOT); break;
	case '-': add_token(token_type::MINUS); break;
	case '+': add_token(token_type::PLUS); break;
	case ';': add_token(token_type::SEMICOLON); break;
	case '*': add_token(token_type::STAR); break;
	case '?': add_token(token_type::QUESTION); break;
	case ':': add_token(token_type::COLON); break;
	case '!':
		add_token(match('=') ? token_type::BANG_EQUAL : token_type::BANG);
		break;
	case '=':
		add_token(match('=') ? token_type::EQUAL_EQUAL : token_type::EQUAL);
		break;
	case '<':
		add_token(match('=') ? token_type::LESS_EQUAL : token_type::LESS);
		break;
	case '>':
		add_token(match('=') ? token_type::GREATER_EQUAL : token_type::GREATER);
		break;
	case '/':
		if (match('/')) {
			while (peek() != '\n' && !is_at_end())
				advance();
		}
		else if (match('*')) {
			while (!is_at_end()) {
				if (peek() == '*' && peek_next() == '/')
					break;
				if (peek() == '\n')
					line++;
				advance();
			}
			if (is_at_end()) {
				lox::error(line, "Unterminated multi-line comment\n");
				return;
			}
			if (match('*')) {
				advance();
				advance();
				//discard two tokens */
			}
		}
		else add_token(token_type::SLASH);
		break;
	case ' ':
	case '\r':
	case '\t':
		break;
	case '\n':
		line++;
		break;
	case '"':
		string();
		break;
	default:
		if (isDigit(c))
			number();
		else if (isAlpha(c))
			identifier();
		else
			lox::error(line, "Unexpected character");
	}
}

std::vector<token>scanner::scan_tokens()
{
	while (!is_at_end()) {
		start = current;
		scan_token();
	}
	tokens.emplace_back(token_type::eof, "EOF", "", line);
	return tokens;
}

const std::unordered_map<std::string, token_type> scanner::keywords =
{
	{"and",	    token_type::AND   },
	{"class",   token_type::CLASS },	
	{"else",    token_type::ELSE },	
	{"false",   token_type::FALSE },	
	{"for",	    token_type::FOR  } ,	
	{"fun",	    token_type::FUN  } ,	
	{"if",	    token_type::IF  } ,	
	{"nil",	    token_type::NIL  } ,	
	{"or",	    token_type::OR  } ,	
	{"print",   token_type::PRINT },	
	{"return",  token_type::RETURN },
	{"super",   token_type::SUPER },	
	{"this",    token_type::THIS } ,	
	{"true",    token_type::TRUE } ,	
	{"var",	    token_type::VAR },	
	{"while",   token_type::WHILE },
	{"break",   token_type::BREAK}
};

#endif
