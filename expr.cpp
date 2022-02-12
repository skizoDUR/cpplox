#ifndef EXPR_CPP
#define EXPR_CPP

#include "token.cpp"
#include <any>
#include <memory>
#include <vector>

template <typename T>
class visitor;

template <typename T>
struct Expr {
	Expr() {};
	virtual T accept(visitor<T> *visitor)
	{
		return visitor->visit(this);
	}
	virtual ~Expr() {};
};

template <typename T>
class Binary : public Expr<T>{
public:
	std::unique_ptr<Expr<T>>left;
	token Operator;
	std::unique_ptr<Expr<T>>right;

	T accept(visitor<T> *visitor) override
	{
		return visitor->visit(this);
	}

	Binary(std::unique_ptr<Expr<T>> &left, token Operator, std::unique_ptr<Expr<T>> &right)
		: left(std::move(left)), Operator(Operator), right(std::move(right)) {}
	virtual ~Binary() = default;
};

template <typename T>
class Grouping: public Expr<T>{
public:
	std::unique_ptr<Expr<T>>expression;

	T accept(visitor<T> *visitor) override
	{
		return visitor->visit(this);
	}
	Grouping(std::unique_ptr<Expr<T>>expression) : expression(std::move(expression)) {}
	~Grouping() override = default;
};

template <typename T>
class Literal : public Expr<T>{
public:
	std::any value;
	T accept(visitor<T> *visitor) override
	{
		return visitor->visit(this);
	}
	Literal(std::any value) : value(value) {}
	Literal() {}
	virtual ~Literal() override = default;
};

template <typename T>
class Unary : public Expr<T> {
public:
	token Operator;
	std::unique_ptr<Expr<T>>right;
	T accept(visitor<T> *visitor) override
	{
		return visitor->visit(this);
	}
	Unary(token &Operator, std::unique_ptr<Expr<T>> &right) :
		Operator(Operator), right(std::move(right)) {}
	~Unary() override = default;
};

template <typename T>
class Variable : public Expr<T> {
public:
	token name;
	Variable(token name) : name(name) {};
	T accept(visitor<T> *visitor) override
	{
		return visitor->visit(this);
	}
	~Variable() override = default;
};

template <typename T>
class Assign : public Expr<T> {
public:
	token name;
	std::unique_ptr<Expr<T>> value;
	Assign(token name, std::unique_ptr<Expr<T>> &value) :
		name(name), value(std::move(value)) {}
	T accept(visitor <T> *visitor) override
	{
		return visitor->visit(this);
	}
	~Assign() override = default;
};

template <typename T>
class Logical : public Expr<T> {
public:
	std::unique_ptr<Expr<T>>left;
	token Operator;
	std::unique_ptr<Expr<T>> right ;
	T accept(visitor<T> *visitor) override
	{
		return visitor->visit(this);
	}
	Logical(std::unique_ptr<Expr<T>> &left, token Operator, std::unique_ptr<Expr<T>> &right) :
		left(std::move(left)), Operator(Operator), right(std::move(right)) {}

	virtual ~Logical() override = default;
};

template <typename T>
struct Ternary : public Expr<T> {

	std::unique_ptr<Expr<T>>condition ;
	std::unique_ptr<Expr<T>>then ;
	std::unique_ptr<Expr<T>>_else ;
	
	T accept(visitor<T> *visitor) override
	{
		return visitor->visit(this);
	}
	Ternary(std::unique_ptr<Expr<T>> &condition, std::unique_ptr<Expr<T>> &then, std::unique_ptr<Expr<T>> &_else) : condition(std::move(condition)), then(std::move(then)), _else(std::move(_else)) {}

	virtual ~Ternary() override = default;
};
template <typename T>
struct Increment : public Expr<T> {
	token target_name;
	token Operator;
	bool postfix;
	T accept(visitor<T> *visitor) override {return visitor->visit(this); }
	std::unique_ptr<Expr<T>>clone()
	{
		return new Increment<T>(*this);
	}
	Increment(const token &target_name, const token &Operator, const bool &postfix) : target_name(target_name), Operator(Operator), postfix(postfix) {}
	~Increment() override = default;
};

template <typename T>
struct Call : public Expr<T> {
	std::unique_ptr<Expr<T>>callee ;
	token paren;
	std::vector<std::unique_ptr<Expr<T>>> arguments;
	T accept(visitor<T> *visitor) override
	{
		return visitor->visit(this);
	}
	Call(std::unique_ptr<Expr<T>> &callee, token paren, std::vector<std::unique_ptr<Expr<T>>> &arguments) : callee(std::move(callee)), paren(paren), arguments(std::move(arguments)) {}
	virtual ~Call() override = default;
};

template<typename T>
struct Function;
template <typename T>
struct Lambda : public Expr<T> {
	std::unique_ptr<Function<T>> declaration;
	Lambda(std::unique_ptr<Function<T>> &declaration) : declaration(std::move(declaration)) {}
	T accept(visitor<T> *visitor) override
	{
		return visitor->visit(this);
	}

};
#endif
