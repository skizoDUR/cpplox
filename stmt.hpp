#ifndef LOX_STMT
#define LOX_STMT
#include "expr.cpp"
#include <memory>
#include <vector>

template <typename T>
class visitor;

template <typename T>
class Stmt {
public:
	virtual ~Stmt() = default;
	virtual void accept(visitor<T> &visitor) {return visitor.visit(*this);}
	std::unique_ptr<Expr<T>> expression;
};

template <typename T>
class Expression : public Stmt<T> {
public:
	Expression(std::unique_ptr<Expr<T>> &expression) : expression(std::move(expression)) {};
	void accept(visitor<T> &visitor) {return visitor.visit(*this);};
	std::unique_ptr<Expr<T>> expression;
};

template <typename T>
class Print : public Stmt<T> {
public:
	Print(std::unique_ptr<Expr<T>> &expression) : expression(std::move(expression)) {};
	void accept(visitor<T> &visitor) {return visitor.visit(*this);}
	std::unique_ptr<Expr<T>> expression;
};

template <typename T>
class Var : public Stmt<T> {
public:
	Var(token name, std::unique_ptr<Expr<T>> &initializer) : initializer(std::move(initializer)), name(name) {};
	void accept(visitor<T> &visitor) {return visitor.visit(*this);}
	std::unique_ptr<Expr<T>> initializer;
	token name;

};

template <typename T>
class Block : public Stmt<T> {
public:
	Block(std::vector<std::unique_ptr<Stmt<T>>> &statements)
	{
		for (auto i = statements.begin(); i != statements.end(); i++)
			this->statements.push_back(std::move(*i));
	}
	void accept(visitor<T> &visitor) {return visitor.visit(*this);}
	std::vector<std::unique_ptr<Stmt<T>>> statements;
};

template <typename T>
class If : public Stmt<T> {
public:
	If(std::unique_ptr<Expr<T>> &condition, std::unique_ptr<Stmt<T>> &Then, std::unique_ptr<Stmt<T>> &Else) : condition(std::move(condition)), Else(std::move(Else)), Then(std::move(Then)) {};
	void accept(visitor<T> &visitor) {return visitor.visit(*this);}
	std::unique_ptr<Expr<T>> condition;
	std::unique_ptr<Stmt<T>> Else;
	std::unique_ptr<Stmt<T>> Then;

	
};

#endif
