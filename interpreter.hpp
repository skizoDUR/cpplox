#ifndef LOX_INTERPRETER
#define LOX_INTERPRETER
#include <any>
#include "cast_unique_ptr.cpp"
#include "environment.hpp"
#include "expr.cpp"
#include "stmt.hpp"
#include "token_type.hpp"
#include "visitor.hpp"
#include "runtime_error.cpp"
#include "finally.cpp"
#include "lox.hpp"
#include <any>
#include <vector>
#include <iostream>

class lox;

template <typename T>
class interpreter : public visitor<T> {
private:
	environment Environment;
	void check_number_operand(token &Operator, std::any &operand)
	{
		if (operand.type() == typeid(double))
			return;
		throw runtime_exception(Operator, "Operand must be a number");
	}
	void check_number_operand(token &Operator, std::any &left, std::any &right)
	{
		if (left.type() == typeid(double) && right.type() == typeid(double))
			return;
		throw runtime_exception(Operator, "Binary Operands must be numbers");
	}
	bool is_equal(std::any &left, std::any &right)
	{
		/*	if (left.type() == typeid(nullptr) && right.type() == typeid(nullptr))
			return true;
			if (right.type() == typeid(nullptr))
			return false;
		*/
		if (left.type() != right.type())
			return false;
		if (left.type() == typeid(double))
			return std::any_cast<double>(left) == std::any_cast<double>(right);
		if (left.type() == typeid(bool))
			return std::any_cast<bool>(left) == std::any_cast<bool>(right);
		if (left.type() == typeid(std::string))
			return std::any_cast<std::string>(left) == std::any_cast<std::string>(right);
		return true;

	}
	bool is_truthy(std::any &thing)
	{
		if (thing.type() == typeid(nullptr))
			return false;
		if (thing.type() == typeid(bool))
			return std::any_cast<bool>(thing);
		return true;

	}
	T evaluate(std::unique_ptr<Expr<T>> &expr)
	{
		return expr->accept(*this);
	}
	T execute(std::unique_ptr<Stmt<T>> &stmt)
	{
		stmt->accept(*this);
		return {};
	}
	void execute_block(std::vector<std::unique_ptr<Stmt<T>>> &stmts, environment env)
	{
		/*environment previous = this->Environment; //it's crucial to keep a reference !
		this->Environment = env;
		for (auto i = stmts.begin(); i != stmts.end(); i++)
			execute(*i);
		finally restore_env([&] {
					    this->Environment = previous;
				    });
		*/
		environment previous = this->Environment;
		env.enclosing = &previous;
		this->Environment = env;
		for (auto i = stmts.begin(); i != stmts.end(); i++)
			execute(*i);
		finally restore_env([&] {
					    this->Environment = previous;
				    });
		
	}
	//void execute(Stmt<T> &);
public:
	//statements
	void visit(Stmt<T> &stmt) override
	{
		evaluate(stmt.expression);
	}
	void visit(Expression<T> &stmt)
	{
		auto result = evaluate(stmt.expression);
		if (lox::repl_mode)
			print_any(result);
	}
	void print_any(std::any &result)
	{
		if (result.type() == typeid(double))
			std::cout << std::any_cast<double>(result) << '\n';
 		if (result.type() == typeid(std::string))
			std::cout << '"' << std::any_cast<std::string>(result) << '"' << '\n';
 		if (result.type() == typeid(bool)) {
 			if (std::any_cast<bool>(result))
 				std::cout << "true\n";
 			else std::cout << "false\n";
 		}
	}
	void visit(Print<T> &stmt)
	{
		auto result = evaluate(stmt.expression);
		print_any(result);
	}
	void visit(Var<T> &stmt)
	{
		std::any value = nullptr;
		if (stmt.initializer.get())
			value = evaluate(stmt.initializer);
		Environment.define(stmt.name.lexeme, value);
	}
	void visit(Block<T> &stmt)
	{
		execute_block(stmt.statements, {});
	}
	void visit(If<T> &stmt)
	{
		auto condition = evaluate(stmt.condition);
		if (is_truthy(condition))
			execute(stmt.Then);
		else if (stmt.Else.get())
			execute(stmt.Else);
	}
	void visit(While<T> &stmt)
	{
		auto condition = evaluate(stmt.condition);
		while (is_truthy(condition)) {
			execute(stmt.Then);
			condition = evaluate(stmt.condition);
		}
	}
	//expressions
	T visit(Expr<T> &expr)
	{
		return expr.accept(*this);
	}
	T visit(Binary  <T> &expr)
	{
		auto left = evaluate(expr.left);
		auto right = evaluate(expr.right);
		switch (expr.Operator.type) {
		case token_type::BANG_EQUAL:
			return !is_equal(left, right);
		case token_type::EQUAL_EQUAL:
			return is_equal(left, right);
		case token_type::GREATER:
			check_number_operand(expr.Operator, left, right);
			return std::any_cast<double>(left) > std::any_cast<double>(right);
		case token_type::GREATER_EQUAL:
			check_number_operand(expr.Operator, left, right);
			return std::any_cast<double>(left) >= std::any_cast<double>(right);
		case token_type::LESS:
			check_number_operand(expr.Operator, left, right);
			return std::any_cast<double>(left) < std::any_cast<double>(right);
		case token_type::LESS_EQUAL:
			check_number_operand(expr.Operator, left, right);
			return std::any_cast<double>(left) <= std::any_cast<double>(right);
		case token_type::MINUS:
			check_number_operand(expr.Operator, left, right);
			return std::any_cast<double>(left) - std::any_cast<double>(right);
		case token_type::PLUS:
			if (left.type() == typeid(double) && right.type() == typeid(double))
				return std::any_cast<double>(left) + std::any_cast<double>(right);
			if (left.type() == typeid(std::string) && right.type() == typeid(std::string))
				return std::any_cast<std::string>(left) + std::any_cast<std::string>(right);
			throw runtime_exception(expr.Operator, "Operands must be two numbers or two strings.");
		case token_type::SLASH:
			check_number_operand(expr.Operator, left, right);
			if (std::any_cast<double>(right) == 0)
				throw runtime_exception(expr.Operator, "Division by zero");
			return std::any_cast<double>(left) / std::any_cast<double>(right);
		case token_type::STAR:
			check_number_operand(expr.Operator, left, right);
			return std::any_cast<double>(left) * std::any_cast<double>(right);
		}
		return {}; //never reached

	}
	T visit(Grouping<T> &expr)
	{
		return evaluate(expr.expression);
	}
	T visit(Literal<T> &expr)
	{
		return expr.value;
	}
	T visit(Unary<T> &expr)
	{
		auto right = evaluate(expr.right);
		switch (expr.Operator.type) {
		case token_type::MINUS:
			check_number_operand(expr.Operator, right);
			return -std::any_cast<double>(right);
			break;
		case token_type::BANG:
			return !is_truthy(right);
		}
		return nullptr;

	}
	T visit(Logical<T> &expr)
	{
		auto left = evaluate(expr.left);
		if (expr.Operator.type == token_type::OR) {
			if (is_truthy(left))
				return left;
		}
		else {
			if (!is_truthy(left))
				return left;
		}
		return evaluate(expr.right);
	}

	T visit(Variable<T> &expr)
	{
		return Environment.get(expr.name);
	}
	T visit(Assign<T> &expr)
	{
		auto value = evaluate(expr.value);
		Environment.assign(expr.name, value);
		return value;
	}
	interpreter() {};
	~interpreter()  = default;
	T interpret(std::vector<std::unique_ptr<Stmt<T>>> &stmts)
	{
		try {
			for (auto i = stmts.begin(); i != stmts.end(); i++)
				execute(*i);
		} catch(runtime_exception &ex) {
			lox::runtime_error(ex);
		}

		return {};
	}
};
#endif
