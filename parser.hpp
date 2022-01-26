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
#include <memory>


struct parser_exception : public std::exception {token_type token; const char *msg;};

template <typename T>
class parser {
private:
	std::vector<token> tokens;
	int current = 0;
	bool is_at_loop = 0;
	std::unique_ptr<Stmt<T>>var_declaration()
	{
		auto name = consume(token_type::IDENTIFIER, "Expected identifier name");
		std::unique_ptr<Expr<T>>initializer; //empty initializer
		if (match(token_type::EQUAL))
			initializer = expression();
		consume(token_type::SEMICOLON, "Expected ';' after variable declaration.");
		return std::make_unique<Var<T>>(name, initializer);
	}
	std::unique_ptr<Stmt<T>>declaration()
	{
		
		try {
			if (match(token_type::FUN))
				return std::move(function_declaration("function"));
			if (match(token_type::VAR))
				return std::move(var_declaration());

			return std::move(statement());
		} catch (parser_exception &error){
			synchronize();
			return {};
		}
	}
	std::unique_ptr<Stmt<T>>function_declaration(std::string type) //TODO: type
	{
		auto identifier = consume(token_type::IDENTIFIER, "Expected identifier after function declaration");
		std::vector<token> parameters;
		consume(token_type::LEFT_PAREN, "Ex-pected '(' after function declaration");
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
		return std::make_unique<Function<T>>(identifier, parameters, body);		
	}
	std::unique_ptr<Stmt<T>>expression_statement()
	{
		auto expr = expression();
		if (!lox::repl_mode)
			consume(token_type::SEMICOLON, "Expected ';'");
		else
			match(token_type::SEMICOLON);
		
		return std::make_unique<Expression<T>>(expr);

	}
	std::unique_ptr<Stmt<T>>print_statement()
	{
		auto expr = expression();
		if (!lox::repl_mode)
			consume(token_type::SEMICOLON, "Expected ';'");
		else {
			match(token_type::SEMICOLON);
		}
		return std::make_unique<Print<T>>(expr);
	}
 	std::vector<std::unique_ptr<Stmt<T>>> block()
 	{
		std::vector<std::unique_ptr<Stmt<T>>> statements;
 		while (!check(token_type::RIGHT_BRACE) && !is_at_end())
			statements.push_back(declaration());
 		consume(token_type::RIGHT_BRACE, "Expected }");
		return statements;
 	}
	std::unique_ptr<Stmt<T>>if_statement()
	{
		consume(token_type::LEFT_PAREN, "'(' Expected after if.");
		std::unique_ptr<Expr<T>>condition = expression();
		consume(token_type::RIGHT_PAREN, "')' Expected after condition.");
		std::unique_ptr<Stmt<T>>then = statement();
		std::unique_ptr<Stmt<T>>else_ = nullptr;
		if (match(token_type::ELSE))
			else_ = statement();
		return std::make_unique<If<T>>(condition, then, else_);
	}
 	std::unique_ptr<Stmt<T>>while_statement()
 	{
 		is_at_loop = 1;
 		consume(token_type::LEFT_PAREN, "'(' Expected after while.");
 		std::unique_ptr<Expr<T>>condition = expression();
 		consume(token_type::RIGHT_PAREN, "')' Expected after condition.");
 		std::unique_ptr<Stmt<T>>then = statement();
 		finally (
			is_at_loop = 0;
			)
	        return std::make_unique<While<T>>(condition, then);
 	}
	std::unique_ptr<Stmt<T>>for_statement()
	{
		is_at_loop = 1;
		consume(token_type::LEFT_PAREN, "'(' Expected after for.");
		std::unique_ptr<Stmt<T>>initializer ;
		if (!match(token_type::SEMICOLON)) { //basically consumes for (; ...)
			if (match(token_type::VAR))
				initializer = var_declaration();
			else initializer = expression_statement();
		}
		std::unique_ptr<Expr<T>>condition ;
		if (!match(token_type::SEMICOLON)) { //gets rid of for (something;  ; ...)
			condition = expression();
			consume(token_type::SEMICOLON, "Expected ';' after loop condition.");
		}
		
		std::unique_ptr<Expr<T>>increase ; //i++ ..
		if (!match(token_type::RIGHT_PAREN)) {
			increase = expression();
			consume(token_type::RIGHT_PAREN, "Expected ')' after for clauses.");
		}
		
		std::unique_ptr<Stmt<T>>body = statement();
		if (increase) {
			std::vector<std::unique_ptr<Stmt<T>>> tmp;
			tmp.push_back(std::move(body));
			tmp.push_back(std::make_unique<Expression<T>>(increase));
			body = std::make_unique<Block<T>>(tmp);
		}
		if (!condition)
			condition = std::make_unique<Literal<T>>(true);
 
		body = std::make_unique<While<T>>(condition, body);
		
		if (initializer.get()) { //there's an initializer
			std::vector<std::unique_ptr<Stmt<T>>> tmp;
			tmp.push_back(std::move(initializer));
			tmp.push_back(std::move(body));
			body = std::make_unique<Block<T>>(tmp);
		}
		
		//we add the initializer to the beginning of the loop
		finally (
			is_at_loop = 0;
			)
		return body;
	}
	std::unique_ptr<Stmt<T>>statement()
	{
		if (match(token_type::RETURN))
			return std::move(return_statement());
		if (match(token_type::BREAK))
			return std::move(break_statement());
		
		if (match(token_type::WHILE))
			return std::move(while_statement());
		if (match(token_type::FOR))
			return std::move(for_statement());
		if (match(token_type::IF))
			return std::move(if_statement());
		
		if (match(token_type::PRINT))
			return std::move(print_statement());
		
 		if (match(token_type::LEFT_BRACE)) {
 			auto get = block();
 			return std::make_unique<Block<T>>(get);
		}
		
		return std::move(expression_statement());
	}
	std::unique_ptr<Stmt<T>>return_statement()
	{
		auto keyword = previous();
		std::unique_ptr<Expr<T>>value ;
		if (!check(token_type::SEMICOLON))
			value = expression();
		consume(token_type::SEMICOLON, "Expected ';' after return");
		return std::make_unique<Return<T>>(keyword, value);
	}
	std::unique_ptr<Stmt<T>>break_statement()
	{
		if (!is_at_loop)
			throw (error(previous(), "Break used outside of loop"));
		consume(token_type::SEMICOLON, "Expected ';'");
		return std::make_unique<Break<T>>();
	}
	bool is_increment()
	{
		return match(token_type::INCREMENT, token_type::DECREMENT);
	}
	std::unique_ptr<Expr<T>> lambda()
	{
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
		auto function = std::make_unique<Function<T>>(parameters, body);
		return std::make_unique<Lambda<T>>(function);
	}
	std::unique_ptr<Expr<T>>primary()
	{
		if (match(token_type::TRUE)) return std::make_unique<Literal<T>>(true);
		if (match(token_type::FALSE)) return std::make_unique<Literal<T>>(false);
		if (match(token_type::NIL)) return std::make_unique<Literal<T>>();
		if (match(token_type::STRING, token_type::NUMBER)) return std::make_unique<Literal<T>>(previous().literal);
		if (match(token_type::LEFT_PAREN)) {
			auto ret = expression(); //this is grouping<T>
			consume(token_type::RIGHT_PAREN, "Expected ')' after expression.");
			//you want to match the ')' token and consume it, else throw an exception
			return ret;
		}
		if (match(token_type::IDENTIFIER))
			return std::make_unique<Variable<T>>(previous());
		if (match(token_type::LAMBDA))
			return std::move(lambda());
		
		throw(error(peek(), "Expected expression "));
		//above throws a std::exception, calling a function that calls
		//lox::error and returns the exception
	}
	std::unique_ptr<Expr<T>>increment_expr(token Operator, std::unique_ptr<Expr<T>> &target, bool postfix)
	{
		auto ret = dynamic_cast<Variable<T>*>(target.get());
		if (!ret)
			throw error(Operator, "Invalid increment expression");
		return std::make_unique<Increment<T>>(ret->name, Operator, postfix);
	}
	std::unique_ptr<Expr<T>>finish_call(std::unique_ptr<Expr<T>> &callee)
	{
		std::vector<std::unique_ptr<Expr<T>>> arguments;
		if (!check(token_type::RIGHT_PAREN)) {
			do {
				arguments.push_back(expression());
			} while (match(token_type::COMMA));
		}
		consume(token_type::RIGHT_PAREN, "Expected ')' after function call");
		return std::make_unique<Call<T>>(callee, previous(), arguments);
	}
	std::unique_ptr<Expr<T>>postfix()
	{
		std::unique_ptr<Expr<T>> expr = primary();
		if (match(token_type::INCREMENT, token_type::DECREMENT))
			return increment_expr(previous(), expr, true);
		while (match(token_type::LEFT_PAREN))
			expr = finish_call(expr);
		
		return expr;
	}

	std::unique_ptr<Expr<T>>unary()
	{
		if (match(token_type::BANG, token_type::MINUS)) {
			token Operator = previous();
			auto expr = unary();
			return std::make_unique<Unary<T>>(Operator, expr);
		}
		if (match(token_type::INCREMENT, token_type::DECREMENT)) {
			auto Operator = previous();
			auto expr = primary();
			return increment_expr(Operator, expr, false);
		}
		
		return postfix();
	}
	std::unique_ptr<Expr<T>>factor()
	{
		std::unique_ptr<Expr<T>>expr = unary();
	
		while (match(token_type::SLASH, token_type::STAR)) {
			token Operator = previous();
			auto right = unary();
			expr = std::make_unique<Binary<T>>(expr, Operator, right);
		}
		return expr;

	}
	std::unique_ptr<Expr<T>>term()
	{
		auto expr = factor();
		while (match(token_type::MINUS, token_type::PLUS)) {
			token Operator = previous();
			auto right = factor();
			expr = std::make_unique<Binary<T>>(expr, Operator, right);
		}
		return expr;

	}
	std::unique_ptr<Expr<T>>comparison()
	{
		auto expr = term();
		while (match(token_type::GREATER, token_type::GREATER_EQUAL, token_type::LESS, token_type::LESS_EQUAL)) {
			token Operator = previous();
			auto right = term();
			expr = std::make_unique<Binary<T>>(expr, Operator, right);
		}
		return expr;
	}
	std::unique_ptr<Expr<T>>equality()
	{
		auto expr = comparison();
		while (match(token_type::EQUAL_EQUAL, token_type::BANG_EQUAL)) {
			auto Operator = previous();
			auto right = comparison();
			expr = std::make_unique<Binary<T>>(expr, Operator, right);
		}
	
		return expr;

	}
	std::unique_ptr<Expr<T>>and_()
	{
		auto expr = equality();
		if (match(token_type::AND)) {
			auto Operator = previous();
			auto right = expression();
			expr = std::make_unique<Logical<T>>(expr, Operator, right);
		}
		return expr;
	}
	std::unique_ptr<Expr<T>>or_()
	{
		auto expr = and_();
		if (match(token_type::OR)) {
			auto Operator = previous();
			auto right = expression();
			expr = std::make_unique<Logical<T>>(expr, Operator, right);
		}
		return expr;
	}
	std::unique_ptr<Expr<T>>ternary()
	{
		//the ternary operator '?'
		auto condition = or_();
		if (match(token_type::QUESTION)) {
			auto Then = expression();
			consume(token_type::COLON, "Expected ':' after ternary expression");
			auto Else = ternary();
			return std::make_unique<Ternary<T>>(condition, Then, Else);
		}
		return condition;
	}
	std::unique_ptr<Expr<T>>assignment()
	{
		std::unique_ptr<Expr<T>>expr = ternary();
		if (match(token_type::EQUAL)) {
			token equals = previous();
			auto value = assignment();
			try {
				auto target = dynamic_cast<Variable<T>*>(expr.get());
				token name = target->name;
				return std::make_unique<Assign<T>>(name, value);
			}
			catch (...) {
				error(equals, "Invalid assignment target.");
			}
		}
		return expr;
	}
	std::unique_ptr<Expr<T>>expression()
	{
		std::unique_ptr<Expr<T>>expr = assignment();
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
	std::vector<std::unique_ptr<Stmt<T>>> parse()
	{
		std::vector<std::unique_ptr<Stmt<T>>> ret;
		while (!is_at_end()) {
			try {
				ret.push_back(std::move(declaration()));
			}
			catch (...) { //catch parser_exception
				return {};
			}
		}
		return ret;
	}
};
#endif
