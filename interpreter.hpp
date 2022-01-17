#ifndef LOX_INTERPRETER
#define LOX_INTERPRETER
#include "env_ptr.hpp"
#include "environment.hpp"
#include "expr.cpp"
#include "lox_callable.hpp"
#include "delete_pointer_vector.hpp"
#include "stmt.hpp"
#include "token_type.hpp"
#include "visitor.hpp"
#include "runtime_error.cpp"
#include "finally.hpp"
#include "lox.hpp"
#include "clock.hpp"
#include "lox_function.hpp"
#include "rand.hpp"
#include "return.hpp"
#include <stdexcept>
#include <any>
#include <memory>
class lox;

template <typename T>
using statement_list = delete_pointer_vector<Stmt<T>>;

template <typename T>
class interpreter : public visitor<T> {
private:

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
	
	T evaluate(Expr<T> *expr)
	{
		return expr->accept(this);
	}
	T execute(Stmt<T> *stmt)
	{
		stmt->accept(this);
		return {};
	}
	std::unordered_map<Expr<T> *, int> locals; 
	std::any lookup_variable(token name, Expr<T> *expr)
	{
		//std::cout << expr << " name : " << name.lexeme << '\n';
		if (locals.contains(expr))
			return Environment->get_at(locals[expr], name.lexeme);
		return globals->get(name);
	}
	//void execute(Stmt<T> &);
public:
	//statements
	void execute_block(statement_list<T> &stmts, env_ptr env)
 	{
		auto previous = this->Environment;
		this->Environment = env;
		finally (
			this->Environment = previous;
			)
		for (auto i = stmts.begin(); i != stmts.end(); i++)
			execute(*i);
	}

	env_ptr Environment = std::make_shared<environment>();
	env_ptr globals = Environment;

	void visit(Stmt<T> *stmt) override
	{
		evaluate(stmt->expression);
	}
	void visit(Expression<T> *stmt) override
	{
		auto result = evaluate(stmt->expression);
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
	void visit(Print<T> *stmt) override
	{
		auto result = evaluate(stmt->expression);
		print_any(result);
	}
	void visit(Var<T> *stmt) override
	{
		std::any value = nullptr;
		if (stmt->initializer)
			value = evaluate(stmt->initializer);
		Environment->define(stmt->name.lexeme, value);
	}
 	void visit(Block<T> *stmt) override
 	{
 		execute_block(stmt->statements, std::make_shared<environment>(Environment));
 	}
	void visit(If<T> *stmt) override
	{
		auto condition = evaluate(stmt->condition);
		if (is_truthy(condition))
			execute(stmt->Then);
		else if (stmt->Else)
			execute(stmt->Else);
	}
	void visit(While<T> *stmt) override
	{
		auto condition = evaluate(stmt->condition);
			while (is_truthy(condition)) {
				try {
					execute(stmt->Then);
				}
				catch (Break<T> &) {
					break;
				}
				condition = evaluate(stmt->condition);
			}
	}
	void visit(Break<T> *stmt) override
	{
		throw Break<T>();
	}
	void visit(Function<T> *stmt) override
	{
		auto function = std::make_shared<lox_function<T>>(*stmt, Environment);
		Environment->define(stmt->name.lexeme, std::static_pointer_cast<lox_callable<T>>(function));
	}
	void visit(Return<T> *stmt) override
	{
		std::any value;
		if (stmt->value)
			value = evaluate(stmt->value);
		throw Return_value(value);
	}
	//expressions
	T visit(Expr<T> *expr) override
	{
		return expr->accept(this);
	}
	T visit(Binary<T> *expr) override
	{
		auto left = evaluate(expr->left);
		auto right = evaluate(expr->right);
/*		if (expr->Operator.type == token_type::PLUS) {
			std::cout << std::any_cast<double>(left) << '+' <<
				std::any_cast<double>(right) << '\n';

		}
*/
		switch (expr->Operator.type) {
		case token_type::BANG_EQUAL:
			return !is_equal(left, right);
		case token_type::EQUAL_EQUAL:
			return is_equal(left, right);
		case token_type::GREATER:
			check_number_operand(expr->Operator, left, right);
			return std::any_cast<double>(left) > std::any_cast<double>(right);
		case token_type::GREATER_EQUAL:
			check_number_operand(expr->Operator, left, right);
			return std::any_cast<double>(left) >= std::any_cast<double>(right);
		case token_type::LESS:
			check_number_operand(expr->Operator, left, right);
			return std::any_cast<double>(left) < std::any_cast<double>(right);
		case token_type::LESS_EQUAL:
			check_number_operand(expr->Operator, left, right);
			return std::any_cast<double>(left) <= std::any_cast<double>(right);
		case token_type::MINUS:
			check_number_operand(expr->Operator, left, right);
			return std::any_cast<double>(left) - std::any_cast<double>(right);
		case token_type::PLUS:
			if (left.type() == typeid(double) && right.type() == typeid(double))
				return std::any_cast<double>(left) + std::any_cast<double>(right);
			if (left.type() == typeid(std::string) && right.type() == typeid(std::string))
				return std::any_cast<std::string>(left) + std::any_cast<std::string>(right);
			throw runtime_exception(expr->Operator, "Operands must be two numbers or two strings.");
		case token_type::SLASH:
			check_number_operand(expr->Operator, left, right);
			if (std::any_cast<double>(right) == 0)
				throw runtime_exception(expr->Operator, "Division by zero");
			return std::any_cast<double>(left) / std::any_cast<double>(right);
		case token_type::STAR:
			check_number_operand(expr->Operator, left, right);
			return std::any_cast<double>(left) * std::any_cast<double>(right);
		}
		return {}; //never reached

	}
	T visit(Grouping<T> *expr) override
	{
		return evaluate(expr->expression);
	}
	T visit(Literal<T> *expr) override
	{
		return expr->value;
	}
	T visit(Increment<T> *expr) override
	{
		auto get = Environment->get(expr->target_name);
		double value;
		try {
			value = std::any_cast<double>(get);
		}
		catch (...) {
			throw runtime_exception(expr->target_name, "Variable does not contain arithmetic type");
		}
		if (expr->Operator.type == token_type::INCREMENT) {
			Environment->assign(expr->target_name, value + 1);
			if (!expr->postfix)
				return value + 1;
		}
		else {
			Environment->assign(expr->target_name, value - 1);
			if (!expr->postfix)
				return value - 1;
		}
		return value;
	}
	T visit(Unary<T> *expr) override
	{
		auto right = evaluate(expr->right);
		switch (expr->Operator.type) {
		case token_type::MINUS:
			check_number_operand(expr->Operator, right);
			return -std::any_cast<double>(right);
			break;
		case token_type::BANG:
			return !is_truthy(right);
			break;
		}
		return nullptr;

	}
	T visit(Logical<T> *expr) override
	{
		auto left = evaluate(expr->left);
		if (expr->Operator.type == token_type::OR) {
			if (is_truthy(left))
				return left;
		}
		else {
			if (!is_truthy(left))
				return left;
		}
		return evaluate(expr->right);
	}
	T visit(Ternary<T> *expr) override
	{
		auto condition = evaluate(expr->condition);
		if (is_truthy(condition))
			return evaluate(expr->then);
		return evaluate(expr->_else);
	}
	T visit(Variable<T> *expr) override
	{
		return lookup_variable(expr->name, expr);
	}
	T visit(Assign<T> *expr) override
	{
		auto value = evaluate(expr->value);
		if (locals.contains(expr))
			Environment->assign_at(locals[expr], expr->name.lexeme, value);
		else
			Environment->assign(expr->name, value);
		return value;
	}
	T visit(Call<T> *expr) override
	{
		using lox_callable_ptr = std::shared_ptr<lox_callable<T>>;
		auto function_any = evaluate(expr->callee);
		lox_callable_ptr function_call;
		try {
			function_call = std::any_cast<lox_callable_ptr>(function_any);
		}
		catch (...) {
			throw runtime_exception(expr->paren, "Not callable expression");
		}
		if (function_call->arity() != (int)expr->arguments.size())
			throw runtime_exception(expr->paren, "Mismatched function parameters");
		std::vector<std::any> arg_list;
		for (auto &i : expr->arguments)
			arg_list.push_back(evaluate(i));
		return function_call->call(*this, arg_list);
	}
	void resolve(Expr<T> *expr, int depth)
	{
		std::cout << expr << " depth: " << depth << std::endl;
		locals[expr] = depth;
	}

	interpreter()
	{
		Environment->define("clock", (std::shared_ptr<lox_callable<T>>)std::make_shared<Clock<T>>());
		Environment->define("rand", (std::shared_ptr<lox_callable<T>>)std::make_shared<Rand<T>>());
	}
	~interpreter() = default;

	T interpret(statement_list<T> &stmts)
	{

		try {
			for (auto &i : stmts)
				execute(i);
		} catch(runtime_exception &ex) {
			lox::runtime_error(ex);
		}

		return {};
	}
};

#endif
