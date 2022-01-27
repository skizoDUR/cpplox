#ifndef RESOLVER_HPP
#define RESOLVER_HPP
#include "expr.cpp"
#include "visitor.hpp"
#include "interpreter.hpp"
#include "stmt.hpp"
#include <list>
#include <string>
#include <unordered_map>
#include <algorithm>

template <typename T>
class Resolver : visitor<T> {
public:
	enum function_type {
		NONE,
		FUNCTION
	};
	struct variable_traits {
		bool ready;
		bool used;
	};
	bool is_at_loop = false;
	function_type current_function = function_type::NONE;
	interpreter<T> &Interpreter;
	std::list<std::unordered_map<std::string, variable_traits>> scopes;
	Resolver(interpreter<T> &Interpreter) : Interpreter(Interpreter) {}
	void report_unused()
	{
		for (auto &i : scopes.front())
			if (!i.second.used)
				std::cout << "Unused variable: " << i.first << '\n';

	}
	void becomes_used(token &name)
	{
		if (scopes.empty())
			return;
		scopes.front()[name.lexeme].used = true; //variable becomes used
	}
	T visit(Expr<T> *expr) override
	{
		return expr->accept(this);
	}
	void visit(Stmt<T> *stmt) override
	{
		stmt->accept(this);
	}
	void visit(Var<T> *stmt) override
	{
		declare(stmt->name);
		if (stmt->initializer.get())
			resolve(stmt->initializer.get());
		define(stmt->name);
	}
	void visit(Block<T> *stmt) override
	{
		begin_scope();
		resolve(stmt->statements);
		report_unused();
		end_scope();
	}
	T visit(Variable<T> *expr) override
	{
		if (!scopes.empty()) {
			if (scopes.front().contains(expr->name.lexeme)) {
				auto v = scopes.front()[expr->name.lexeme].ready;
				if (!v)
					lox::error(expr->name, "Can't read local variable in its own initializer");
			}
		}
		becomes_used(expr->name); //this variable is used
		resolve_local(expr, expr->name);
		return {};
	}
	T visit(Assign<T> *expr) override
	{
		resolve(expr->value.get());
		becomes_used(expr->name);
		resolve_local(expr, expr->name);
		return {};
	}
	T visit(Lambda<T> *expr) override
	{
		//I don't want the ANONYMOUS function to be taken into account as a variable
		//As opposite to real functions lambdas don't bind a name themselves
		resolve_function(expr->declaration.get(), function_type::FUNCTION);
		return {};
	}
	void visit(Function<T> *stmt) override
	{
		declare(stmt->name);
		define(stmt->name);
		resolve_function(stmt, function_type::FUNCTION);
	}
	void visit(Expression<T> *stmt) override
	{
		resolve(stmt->expression.get());
	}
	void visit(If<T> *stmt) override
	{
		resolve(stmt->condition.get());
		resolve(stmt->Then.get());
		if (stmt->Else.get())
			resolve(stmt->Else.get());
	}
	void visit(Print<T> *stmt) override
	{
		resolve(stmt->expression.get());
	}
	void visit(Return<T> *stmt) override
	{
		if (current_function == function_type::NONE) {
			lox::error(stmt->keyword, "Can't return from top level.");
		}
		if (stmt->value.get())
			resolve(stmt->value.get());
	}
	void visit(While<T> *stmt) override
	{
		resolve(stmt->condition.get());
		is_at_loop = true;
		resolve(stmt->Then.get());
		is_at_loop = false;
	}
	void visit(Break<T> *stmt) override
	{
		if (!is_at_loop)
			lox::error(stmt->keyword, "Break used outside of loop");
	}
	T visit(Binary<T> *expr) override
	{
		resolve(expr->left.get());
		resolve(expr->right.get());
		return {};
	}
	T visit(Call<T> *expr) override
	{
		resolve(expr->callee.get());
		for (auto &argument : expr->arguments)
			resolve(argument.get());
		return {};
	}
	T visit(Grouping<T> *expr) override
	{
		resolve(expr->expression.get());
		return {};
	}
	T visit(Literal<T> *expr) override
	{
		return {};
	}
	T visit(Logical<T> *expr) override
	{
		resolve(expr->left.get());
		resolve(expr->right.get());
		return {};
	}
	T visit(Unary<T> *expr) override
	{
		resolve(expr->right.get());
		return {};
	}
	T visit(Ternary<T> *expr) override
	{
		resolve(expr->condition.get());
		resolve(expr->then.get());
		if (expr->_else.get())
			resolve(expr->_else.get());
		return {};
	}
	T visit(Increment<T> *expr) override
	{
		return {};
	}
	void resolve(std::vector<std::unique_ptr<Stmt<T>>> &statements)
	{
		for (auto &i : statements)
			resolve(i.get());
	}
	void resolve(Stmt<T> *stmt)
	{
		stmt->accept(this);
	}
	void resolve(Expr<T> *expr)
	{
		expr->accept(this);
	}
	void begin_scope()
	{
		scopes.emplace_front();
	}
	void end_scope()
	{
		scopes.pop_front();
	}
	void declare(token &name)
	{
		if (scopes.empty())
			return;
		scopes.front()[name.lexeme].ready = false;
	}
	void define(token &name)
	{
		if (scopes.empty())
			return;
		scopes.front()[name.lexeme].ready = true;
		scopes.front()[name.lexeme].used = false;
	}
	void resolve_local(Expr<T> *expr, token &name)
	{
		for (auto i = scopes.begin(); i != scopes.end(); i++)
			if (i->contains(name.lexeme)) {
				Interpreter.resolve(expr, std::distance(scopes.begin(), i));
				break;
			}
	}
	void resolve_function(Function<T> *stmt, function_type type)
	{
		auto enclosing_function = current_function;
		current_function = type;
		begin_scope();
		for (auto &param : stmt->params) {
			declare(param);
			define(param);
		}
		resolve(stmt->body);
		report_unused();
		end_scope();
		current_function = enclosing_function;
	}

};
#endif
