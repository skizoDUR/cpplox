#ifndef PARSER_HPP
#define PARSER_HPP
#include "lox.hpp"
#include "expr.cpp"
#include "token.cpp"
#include "token_type.hpp"
#include "stmt.hpp"
#include "finally.hpp"
#include <array>
#include <exception>
#include <vector>
template <typename T>
using statement_list = delete_pointer_vector<Stmt<T>>;
template <typename T>
using argument_t = delete_pointer_vector<Expr<T>>;
struct parser_exception : public std::exception {token_type token; const char *msg;};

template <typename T>
class parser {
private:
	std::vector<token> tokens;
	int current = 0;
	bool is_at_loop = 0;
	Stmt<T> *var_declaration()
	{
		auto name = consume(token_type::IDENTIFIER, "Expected identifier name");
		Expr<T> *initializer = nullptr; //empty initializer
		if (match(token_type::EQUAL))
			initializer = expression();
		consume(token_type::SEMICOLON, "Expected ';' after variable declaration.");
		return new Var<T>(name, initializer);
	}
	Stmt<T> *declaration()
	{
		try {
			if (match(token_type::FUN))
				return function_declaration("function");
			if (match(token_type::VAR))
				return var_declaration();
			return statement();
		} catch (parser_exception &error){
			synchronize();
			return nullptr;
		}
	}
	Stmt<T> *function_declaration(std::string type) //TODO: type
	{
		auto identifier = consume(token_type::IDENTIFIER, "Expected identifier after function declaration");
		std::vector<token> parameters;
		consume(token_type::LEFT_PAREN, "Expected '(' after function declaration");
		if (peek().type != token_type::RIGHT_PAREN) {
			do {
				if (parameters.size() >= 255)
					error(peek(), "Can't have more than 255 parameters");
				parameters.push_back(consume(token_type::IDENTIFIER, "Expected identifier"));
			} while (match(token_type::COMMA));
		}
		consume(token_type::RIGHT_PAREN, "Expected ')' after function parameters");
		consume(token_type::LEFT_BRACE, "Expected '{' before function body");
		auto body = block();
		return new Function<T>(identifier, parameters, body);
		
	}
	Stmt<T> *expression_statement()
	{
		auto expr = expression();
		if (!lox::repl_mode)
			consume(token_type::SEMICOLON, "Expected ';'");
		else
			match(token_type::SEMICOLON);
		
		return new Expression<T>(expr);

	}
	Stmt<T> *print_statement()
	{
		auto expr = expression();
		if (!lox::repl_mode)
			consume(token_type::SEMICOLON, "Expected ';'");
		else {
			match(token_type::SEMICOLON);
		}
		return new Print<T>(expr);
	}
 	statement_list<T> block()
 	{
 		statement_list<T> statements;
 		while (!check(token_type::RIGHT_BRACE) && !is_at_end())
			statements.push_back(declaration());
 		consume(token_type::RIGHT_BRACE, "Expected }");
		return statements;
 	}
	Stmt<T> *if_statement()
	{
		consume(token_type::LEFT_PAREN, "'(' Expected after if.");
		Expr<T> *condition = expression();
		consume(token_type::RIGHT_PAREN, "')' Expected after condition.");
		Stmt<T> *then = statement();
		Stmt<T> *else_ = nullptr;
		if (match(token_type::ELSE))
			else_ = statement();
		return new If<T>(condition, then, else_);
	}
 	Stmt<T> *while_statement()
 	{
 		is_at_loop = 1;
 		consume(token_type::LEFT_PAREN, "'(' Expected after while.");
 		Expr<T> *condition = expression();
 		consume(token_type::RIGHT_PAREN, "')' Expected after condition.");
 		Stmt<T> *then = statement();
 		finally (
			is_at_loop = 0;
			)
 		return new While<T>(condition, then);
 	}
	Stmt<T> *for_statement()
	{
		is_at_loop = 1;
		consume(token_type::LEFT_PAREN, "'(' Expected after for.");
		Stmt<T> *initializer = nullptr;
		if (!match(token_type::SEMICOLON)) { //basically consumes for (; ...)
			if (match(token_type::VAR))
				initializer = var_declaration();
			else initializer = expression_statement();
		}
		Expr<T> *condition = nullptr;
		if (!match(token_type::SEMICOLON)) { //gets rid of for (something;  ; ...)
			condition = expression();
			consume(token_type::SEMICOLON, "Expected ';' after loop condition.");
		}
		
		Expr<T> *increase = nullptr; //i++ ..
		if (!match(token_type::RIGHT_PAREN)) {
			increase = expression();
			consume(token_type::RIGHT_PAREN, "Expected ')' after for clauses.");
		}
		
		Stmt<T> *body = statement();
		if (increase) {
			statement_list<T> tmp;
			tmp.push_back(body);
			tmp.push_back(new Expression<T>(increase));
			body = new Block<T>(tmp);
		}
		if (!condition)
			condition = new Literal<T>(true);
 
		body = new While<T>(condition, body);
 
		if (initializer) { //there's an initializer
			statement_list<T> tmp;
			tmp.push_back(initializer);
			tmp.push_back(body);
			body = new Block<T>(tmp);
		}
		//we add the initializer to the beginning of the loop
		finally (
			is_at_loop = 0;
			)
		return body;
	}
	Stmt<T> *statement()
	{
		if (match(token_type::RETURN))
			return return_statement();
		if (match(token_type::BREAK))
			return break_statement();
		if (match(token_type::WHILE))
			return while_statement();
		if (match(token_type::FOR))
			return for_statement();
		if (match(token_type::IF))
			return if_statement();
		if (match(token_type::PRINT))
			return print_statement();
 		if (match(token_type::LEFT_BRACE)) {
 			statement_list<T> get = block();
 			return new Block<T>(get);
		}
		return expression_statement();
	}
	Stmt<T> *return_statement()
	{
		auto keyword = previous();
		Expr<T> *value = nullptr;
		if (!check(token_type::SEMICOLON))
			value = expression();
		consume(token_type::SEMICOLON, "Expected ';' after return");
		return new Return<T>(keyword, value);
	}
	Stmt<T> *break_statement()
	{
		if (!is_at_loop)
			throw (error(previous(), "Break used outside of loop"));
		consume(token_type::SEMICOLON, "Expected ';'");
		return new Break<T>();
	}
	bool is_increment()
	{
		return match(token_type::INCREMENT, token_type::DECREMENT);
	}
	Expr<T> *primary()
	{
		if (match(token_type::TRUE)) return new Literal<T>(true);
		if (match(token_type::FALSE)) return new Literal<T>(false);
		if (match(token_type::NIL)) return new Literal<T>();
		if (match(token_type::STRING, token_type::NUMBER)) return new Literal<T>(previous().literal);
		if (match(token_type::LEFT_PAREN)) {
			auto ret = expression(); //this is grouping<T>
			consume(token_type::RIGHT_PAREN, "Expected ')' after expression.");
			//you want to match the ')' token and consume it, else throw an exception
			return ret;
		}
		if (match(token_type::IDENTIFIER))
			return new Variable<T>(previous());
		
		throw(error(peek(), "Expected expression "));
		//above throws a std::exception, calling a function that calls
		//lox::error and returns the exception
	}
	Expr<T> *increment_expr(token Operator, Expr<T> *target, bool postfix)
	{
		auto ret = dynamic_cast<Variable<T>*>(target);
		if (!ret)
			throw error(Operator, "Invalid increment expression");
		return new Increment<T>(ret->name, Operator, postfix);
	}
	Expr<T> *finish_call(Expr<T> *callee)
	{
		argument_t<T> arguments;
		if (!check(token_type::RIGHT_PAREN)) {
			do {
				arguments.push_back(expression());
			} while (match(token_type::COMMA));
		}
		consume(token_type::RIGHT_PAREN, "Expected ')' after function call");
		return new Call<T>(callee, previous(), arguments);
	}
	Expr<T> *postfix()
	{
		auto expr = primary();
		if (match(token_type::INCREMENT, token_type::DECREMENT))
			return increment_expr(previous(), expr, true);
		while (match(token_type::LEFT_PAREN))
			expr = finish_call(expr);
		
		return expr;
	}

	Expr<T> *unary()
	{
		if (match(token_type::BANG, token_type::MINUS)) {
			token Operator = previous();
			auto expr = unary();
			return new Unary<T>(Operator, expr);
		}
		if (match(token_type::INCREMENT, token_type::DECREMENT)) {
			auto Operator = previous();
			return increment_expr(Operator, primary(), false);
		}
		
		return postfix();
	}
	Expr<T> *factor()
	{
		Expr<T> *expr = unary();
	
		while (match(token_type::SLASH, token_type::STAR)) {
			token Operator = previous();
			auto right = unary();
			expr = new Binary<T>(expr, Operator, right);
		}
		return expr;

	}
	Expr<T> *term()
	{
		auto expr = factor();
		while (match(token_type::MINUS, token_type::PLUS)) {
			token Operator = previous();
			auto right = factor();
			expr = new Binary<T>(expr, Operator, right);
		}
		return expr;

	}
	Expr<T> *comparison()
	{
		auto expr = term();
		while (match(token_type::GREATER, token_type::GREATER_EQUAL, token_type::LESS, token_type::LESS_EQUAL)) {
			token Operator = previous();
			auto right = term();
			expr = new Binary<T>(expr, Operator, right);
		}
		return expr;
	}
	Expr<T> *equality()
	{
		auto expr = comparison();
		while (match(token_type::EQUAL_EQUAL, token_type::BANG_EQUAL)) {
			auto Operator = previous();
			auto right = comparison();
			expr = new Binary<T>(expr, Operator, right);
		}
	
		return expr;

	}
	Expr<T> *and_()
	{
		auto expr = equality();
		if (match(token_type::AND)) {
			auto Operator = previous();
			auto right = expression();
			expr = new Logical<T>(expr, Operator, right);
		}
		return expr;
	}
	Expr<T> *or_()
	{
		auto expr = and_();
		if (match(token_type::OR)) {
			auto Operator = previous();
			auto right = expression();
			expr = new Logical<T>(expr, Operator, right);
		}
		return expr;
	}
	Expr<T> *ternary()
	{
		//the ternary operator '?'
		auto condition = or_();
		if (match(token_type::QUESTION)) {
			auto Then = expression();
			consume(token_type::COLON, "Expected ':' after ternary expression");
			auto Else = ternary();
			return new Ternary<T>(condition, Then, Else);
		}
		return condition;
	}
	Expr<T> *assignment()
	{
		Expr<T> *expr = ternary();
		if (match(token_type::EQUAL)) {
			token equals = previous();
			auto value = assignment();
			try {
				auto target = dynamic_cast<Variable<T>*>(expr);
				token name = target->name;
				return new Assign<T>(name, value);
			}
			catch (...) {
				error(equals, "Invalid assignment target.");
			}
		}
		return expr;
	}
	Expr<T> *expression()
	{
		Expr<T> *expr = assignment();
		return expr;
	}
	parser_exception error(token token, const char *msg)
	{
		lox::error(token, msg);
		return parser_exception();
	}
	void synchronize()
	{
		while (!is_at_end()) {
			switch (advance().type) { //fixed for some reason
			case token_type::SEMICOLON:
			case token_type::CLASS:
			case token_type::FUN:
			case token_type::VAR:
			case token_type::FOR:
			case token_type::IF:
			case token_type::WHILE:
			case token_type::PRINT:
			case token_type::RETURN:
				return;
			}
			advance();
		}
	}
	bool check(token_type token)
	{
		return tokens.at(current).type == token;
	}
	token advance()
	{
		return tokens.at(current++);
	}
	token consume(token_type token, const char *msg)
	{
		if (check(token))
			return advance();
		throw(error(peek(), msg));
	}
	bool is_at_end()
	{
		return peek().type == token_type::eof;
	}
	token peek()
	{
		return tokens.at(current);
	}
	token previous()
	{
		return tokens.at(current - 1);
	}
	bool match(token_type t)
	{
		if (check(t)) {
			advance();
			return 1;
		}
		return 0;
	}
	template <class ...Args>
	bool match(token_type t, Args ...args)
	{
		if (check(t)) {
			advance();
			return 1;
		}
		return match(args...);
	}
		
public:
	parser(std::vector<token> &tokens) : tokens(tokens) {};
	parser() {};
	statement_list<T> parse()
	{
		statement_list<T> ret;
		
		while (!is_at_end()) {
			try {
				ret.push_back(declaration());
			}
			catch (...) { //catch parser_exception
				return {};
			}
		}
		return ret;
	}
};
#endif
