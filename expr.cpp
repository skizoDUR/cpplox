#ifndef EXPR_CPP
#define EXPR_CPP

#include "token.cpp"
#include <any>
#include <memory>

template <typename T>
class visitor;

template <typename T>
struct Expr{
	Expr() {};
	virtual T accept(visitor<T> &visitor) {return visitor.visit(*this);}
	virtual ~Expr() {};
};

template <typename T>
class Binary  : public Expr<T>{
public:
	std::unique_ptr<Expr<T>> left;
	token Operator;
	std::unique_ptr<Expr<T>> right;
	T accept(visitor<T> &visitor) override
	{
		return visitor.visit(*this);
	}
	Binary(std::unique_ptr<Expr<T>> &left, token Operator, std::unique_ptr<Expr<T>> &right) :
		left(std::move(left)), Operator(Operator), right(std::move(right)) {};
	Binary() {};
	virtual ~Binary() override = default;
};

template <typename T>
class Grouping: public Expr<T>{
public:
	std::unique_ptr<Expr<T>> expression;
	T accept(visitor<T> &visitor) override
	{
		return visitor.visit(*this);
	}
	Grouping(std::unique_ptr<Expr<T>> expression) : expression(std::move(expression)) {};
	Grouping() {};
	virtual ~Grouping() override = default;
};

template <typename T>
class Literal : public Expr<T>{
public:
	std::any value;
	T accept(visitor<T> &visitor) override
	{
		return visitor.visit(*this);
	}
	Literal(std::any value) : value(value) {};
	Literal() {};
	virtual ~Literal() override = default;
};

template <typename T>
class Unary : public Expr<T> {
public:
	token Operator;
	std::unique_ptr<Expr<T>> right;
	T accept(visitor<T> &visitor) override
	{
		return visitor.visit(*this);
	}
	Unary(token &Operator, std::unique_ptr<Expr<T>> &right) :
		Operator(Operator), right(std::move(right)) {};
	Unary() {};
	virtual ~Unary() override = default;
};

template <typename T>
class Variable : public Expr<T> {
public:
	token name;
	Variable(token name) : name(name) {};
	T accept(visitor<T> &visitor) override
	{
		return visitor.visit(*this);
	}
};

template <typename T>
class Assign : public Expr<T> {
public:
	token name;
	std::unique_ptr<Expr<T>> value;
	Assign(token name, std::unique_ptr<Expr<T>> &value) :
		name(name), value(std::move(value)) {};
	T accept(visitor <T> &visitor) override
	{
		return visitor.visit(*this);
	}
};

template <typename T>
class Logical : public Expr<T> {
public:
	std::unique_ptr<Expr<T>> left;
	token Operator;
	std::unique_ptr<Expr<T>> right;
	T accept(visitor<T> &visitor) override
	{
		return visitor.visit(*this);
	}
	Logical(std::unique_ptr<Expr<T>> &left, token Operator, std::unique_ptr<Expr<T>> &right) :
		left(std::move(left)), Operator(Operator), right(std::move(right)) {};
	Logical() {};
	virtual ~Logical() override = default;

};

#endif
