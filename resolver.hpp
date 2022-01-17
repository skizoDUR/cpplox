#ifndef RESOLVER_HPP
#define RESOLVER_HPP
#include "expr.cpp"
#include "visitor.hpp"
#include "interpreter.hpp"
#include "stmt.hpp"
#include "delete_pointer_vector.hpp"
#include <list>
#include <string>
#include <unordered_map>
#include <algorithm>
template <typename T>
using statement_list = delete_pointer_vector<Stmt<T>>;

template <typename T>

class Resolver : visitor<T> {
public:
	interpreter<T> &Interpreter;
	std::list<std::unordered_map<std::string, bool>> scopes;
	Resolver(interpreter<T> &Interpreter) : Interpreter(Interpreter) {}
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
		if (stmt->initializer)
			resolve(stmt->initializer);
		define(stmt->name);
	}
	void visit(Block<T> *stmt) override
	{
		begin_scope();
		resolve(stmt->statements);
		end_scope();
	}
	T visit(Variable<T> *expr) override
	{
		
		if (!scopes.empty()) {
			if (scopes.front().contains(expr->name.lexeme)) {
				auto v = scopes.front()[expr->name.lexeme];
				if (!v)
					lox::error(expr->name, "Can't read local variable in its own initializer");
			}
		}
		resolve_local(expr, expr->name);
		return {};
	}
	T visit(Assign<T> *expr) override
	{
		resolve(expr->value);
		resolve_local(expr, expr->name);
		return {};
	}
	void visit(Function<T> *stmt) override
	{
		declare(stmt->name);
		define(stmt->name);
		resolve_function(stmt);
	}
	void visit(Expression<T> *stmt) override
	{
		resolve(stmt->expression);
	}
	void visit(If<T> *stmt) override
	{
		resolve(stmt->condition);
		resolve(stmt->Then);
		if (stmt->Else)
			resolve(stmt->Else);
	}
	void visit(Print<T> *stmt) override
	{
		resolve(stmt->expression);
	}
	void visit(Return<T> *stmt) override
	{
		if (stmt->value)
			resolve(stmt->value);
	}
	void visit(While<T> *stmt) override
	{
		resolve(stmt->condition);
		resolve(stmt->Then);
	}
	void visit(Break<T> *stmt) override
	{
		
	}
	T visit(Binary<T> *expr) override
	{
		resolve(expr->left);
		resolve(expr->right);
		return {};
	}
	T visit(Call<T> *expr) override
	{
		resolve(expr->callee);
		for (auto argument : expr->arguments)
			resolve(argument);
		return {};
	}
	T visit(Grouping<T> *expr) override
	{
		resolve(expr->expression);
		return {};
	}
	T visit(Literal<T> *expr) override
	{
		return {};
	}
	T visit(Logical<T> *expr) override
	{
		resolve(expr->left);
		resolve(expr->right);
		return {};
	}
	T visit(Unary<T> *expr) override
	{
		resolve(expr->right);
		return {};
	}
	T visit(Ternary<T> *expr) override
	{
		resolve(expr->condition);
		resolve(expr->then);
		if (expr->_else)
			resolve(expr->_else);
		return {};
	}
	T visit(Increment<T> *expr) override
	{
		return {};
	}
	void resolve(statement_list<T> &statements)
	{
		for (auto &i : statements)
			resolve(i);
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
		scopes.front()[name.lexeme] = false;
	}
	void define(token &name)
	{
		if (scopes.empty())
			return;
		scopes.front()[name.lexeme] = true;
	}
	void resolve_local(Expr<T> *expr, token &name)
	{
		for (auto i = scopes.begin(); i != scopes.end(); i++)
			if (i->contains(name.lexeme)) {
				Interpreter.resolve(expr, std::distance(scopes.begin(), i));
				break;
			}
	}
	void resolve_function(Function<T> *stmt)
	{
		begin_scope();
		for (auto &param : stmt->params) {
			declare(param);
			define(param);
		}
		resolve(stmt->body);
		end_scope();
	}

};
#endif
