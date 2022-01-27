#ifndef LOX_STMT
#define LOX_STMT
#include "expr.cpp"
#include <stdexcept>
#include <vector>
#include <list>
#include <memory>

template <typename T>
class visitor;

template <typename T>
class Stmt {
public:
	virtual ~Stmt() = default;
	virtual void accept(visitor<T> *visitor) {return visitor->visit(this);}
	std::unique_ptr<Expr<T>> expression;
};


template <typename T>
class Expression : public Stmt<T> {
public:
	void accept(visitor<T> *visitor) override
	{
		return visitor->visit(this);
	}
	Expression(std::unique_ptr<Expr<T>> &expression) : expression(std::move(expression)) {}
	std::unique_ptr<Expr<T>>expression;
	~Expression() override = default;
};

template <typename T>
class Print : public Stmt<T> {
public:
	Print(std::unique_ptr<Expr<T>> &expression) : expression(std::move(expression)) {}
	void accept(visitor<T> *visitor) override
	{
		return visitor->visit(this);
	}
	std::unique_ptr<Expr<T>>expression;
	~Print() override = default;
};

template <typename T>
class Var : public Stmt<T> {
public:
	Var(token name, std::unique_ptr<Expr<T>> &initializer) : name(name), initializer(std::move(initializer)) {}
	void accept(visitor<T> *visitor) {return visitor->visit(this);}
	token name;
	std::unique_ptr<Expr<T>>initializer;
	~Var() override = default;
};

template <typename T>
class Block : public Stmt<T> {
public:
	Block(std::vector<std::unique_ptr<Stmt<T>>> &statements) : statements(std::move(statements)) {}
	Block() {}
	~Block() override = default;
	void accept(visitor<T> *visitor) {return visitor->visit(this);}
	std::vector<std::unique_ptr<Stmt<T>>> statements;
};

template <typename T>
class If : public Stmt<T> {
public:
	~If() override = default;
	If(std::unique_ptr<Expr<T>> &condition, std::unique_ptr<Stmt<T>>&Then, std::unique_ptr<Stmt<T>>&Else) :
		condition(std::move(condition)), Then(std::move(Then)), Else(std::move(Else)) {}
	void accept(visitor<T> *visitor) {return visitor->visit(this);}
	std::unique_ptr<Expr<T>>condition;
	std::unique_ptr<Stmt<T>>Then;
	std::unique_ptr<Stmt<T>>Else;
};

template <typename T>
class While : public Stmt<T> {
public:
	While(std::unique_ptr<Expr<T>> &condition, std::unique_ptr<Stmt<T>> &Then) : condition(std::move(condition)), Then(std::move(Then)) {}
	~While() override = default;
	void accept(visitor<T> *visitor) {return visitor->visit(this);}
	std::unique_ptr<Expr<T>>condition;
	std::unique_ptr<Stmt<T>>Then;
};

template <typename T>
class Break : public Stmt<T>, std::exception {
public:
	token keyword;
	Break(token &keyword) : keyword(keyword) {}
	Break() {}
	void accept(visitor<T> *visitor) {return visitor->visit(this);}
};

template <typename T>
class Function : public Stmt<T> {
public:
	void accept(visitor<T> *visitor) {return visitor->visit(this);}
	token name;
	std::vector<token> params;
	std::vector<std::unique_ptr<Stmt<T>>> body;

	Function(token name, std::vector<token> &params, std::vector<std::unique_ptr<Stmt<T>>>&body) : name(name), params(std::move(params)), body(std::move(body)) {}
	Function(std::vector<token> &params, std::vector<std::unique_ptr<Stmt<T>>>&body) : params(std::move(params)), body(std::move(body)) {}
	Function(Function<T> &&other)
	{
		this->name = other.name;
		this->params = other.params;
		this->body = std::move(other.body);
	}
	~Function() override = default;
};

template <typename T>
class Return : public Stmt<T> {
public:
	void accept(visitor<T> *visitor) {return visitor->visit(this);}
	explicit Return(token &keyword, std::unique_ptr<Expr<T>> &value) : keyword(keyword), value(std::move(value)) {}
	token keyword;
	std::unique_ptr<Expr<T>>value ;
	~Return() override = default;
};
#endif
