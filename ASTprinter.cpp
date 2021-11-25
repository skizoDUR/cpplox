/*
#include "expr.cpp"
#include "token_type.hpp"
#include <any>
#include <string>
#include <iostream>

struct ASTprinter : visitor<std::string> {
public:
	template <typename ...Args>
	std::string parenthesize(std::unique_ptr<Expr<std::string>> &expr)
	{
		return expr->accept(*this);
	}
	
	template <typename ...Args>
	std::string parenthesize(std::unique_ptr<Expr<std::string>> &expr, Args &...args)
	{
		return " " + expr->accept(*this) + " " + parenthesize(args...);
	}
	
	template <typename ...Args>
	std::string parenthesize(std::string name, Args &...args) //lexeme, arg_count, exprs
	{
		std::string ret;
		ret += "(" + name; 
		ret += parenthesize(args...);
		return ret + ")";
	}
	
	virtual std::string print(std::unique_ptr<Expr<std::string>> &expr)
	{
		return expr->accept(*this);
	}

	virtual std::string visit(Expr<std::string> &expr)
	{
		return expr.accept(*this);
	}
	
	virtual std::string visit(Binary<std::string> &expr)
	{
		return parenthesize(expr.Operator.lexeme, expr.left, expr.right);
	}

	virtual std::string visit(Grouping<std::string> &expr)
	{
		return parenthesize("group", expr.expression);
	}

	virtual std::string visit(Literal<std::string> &expr)
	{
		if (!expr.value.has_value())
			return "nil";

		if (expr.value.type() == typeid(double))
			return std::to_string(std::any_cast<double>(expr.value));
		if (expr.value.type() == typeid(bool))
			return std::to_string(std::any_cast<bool>(expr.value));
		if (expr.value.type() == typeid(nullptr))
			return "NULL";
		
		return std::any_cast<std::string>(expr.value);
	}

	virtual std::string visit(Unary<std::string> &expr)
	{
		return parenthesize(expr.Operator.lexeme, expr.right);
	}

};

class RPN: public ASTprinter { //reverse polish notation (reverse lisp)
public:
	template <typename ...Args>
	std::string parenthesize(Expr<std::string> &expr)
	{
		return expr.accept(*this);
	}
	
	template <typename ...Args>
	std::string parenthesize(Expr<std::string> &expr, Args &...args)
	{
		return expr.accept(*this) + " " + parenthesize(args...);
	}
	
	template <typename ...Args>
	std::string parenthesize(std::string name, Args &...args) //lexeme, arg_count, exprs
	{
		std::string ret;
		ret += parenthesize(args...);
		ret += " " + name; 
		return ret;
	}

	virtual std::string print(Expr<std::string> &expr)
	{
		return expr.accept(*this);
	}

	virtual std::string visit(Expr<std::string> &expr)
	{
		return expr.accept(*this);
	}
	
	virtual std::string visit(Binary<std::string> &expr)
	{
		return parenthesize(expr.Operator.lexeme, expr.left, expr.right);
	}

	virtual std::string visit(Grouping<std::string> &expr)
	{
		return parenthesize("group", expr.expression);
	}

	virtual std::string visit(Literal<std::string> &expr)
	{
		if (!expr.value.has_value())
			return "nil";
		if (expr.value.type() == typeid(double))
			return std::to_string(std::any_cast<double>(expr.value));
		
		return std::any_cast<std::string>(expr.value);
	}

	virtual std::string visit(Unary<std::string> &expr)
	{
		return parenthesize(expr.Operator.lexeme, expr.right);
	}
	
};


*/
