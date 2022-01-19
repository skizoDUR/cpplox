#ifndef EXPR_CPP
#define EXPR_CPP

#include "token.cpp"
#include "delete_pointer_vector.hpp"
#include <any>

template <typename T>
class visitor;

template <typename T>
struct Expr {
	Expr() {};
	virtual T accept(visitor<T> *visitor)
	{
		return visitor->visit(this);
	}
	virtual Expr<T> *clone()
	{
		return new Expr<T>(*this);
	}
	virtual ~Expr() {};
};
template <typename T>
using argument = delete_pointer_vector<Expr<T>>;

template <typename T>
class Binary : public Expr<T>{
public:
	Expr<T> *left = nullptr;
	token Operator;
	Expr<T> *right = nullptr;
	virtual Expr<T> *clone()
	{
		return new Binary<T>(*this);
	}
	T accept(visitor<T> *visitor) override
	{
		return visitor->visit(this);
	}
	Binary(const Binary<T> &other)
	{
		if (this == &other)
			throw std::runtime_error("Error: Self assignment");
		this->left = other.left->clone();
		this->Operator = other.Operator;
		this->right = other.right->clone();
	}
	Binary(const Binary<T> *other)
	{
		if (this == other)
			throw std::runtime_error("Error: Self assignment");
		this->left = other->left;
		this->Operator = other->Operator;
		this->right = other->right;
	}
	Binary(Expr<T> &left, token Operator, Expr<T> &right)
		: left(new Expr<T>(left)), Operator(Operator), right(new Expr<T>(right)) {}
	Binary(Expr<T> *left, token Operator, Expr<T> *right)
		: left(left), Operator(Operator), right(right) {}

	/*void operator=(Binary<T> *other)
	{
		if (this == other)
			return;
		free();
		this->left = new Expr<T>(other->left[0]);
		this->right = new Expr<T>(other->right[0]);
		this->Operator = other->Operator;
	}
	*/
	void free()
	{
		delete left;
		delete right;
	}
	virtual ~Binary()
	{
		free();
	}
};

template <typename T>
class Grouping: public Expr<T>{
public:
	Expr<T> *expression = nullptr;
	virtual Expr<T> *clone()
	{
		return new Grouping<T>(*this);
	}
	T accept(visitor<T> *visitor) override
	{
		return visitor->visit(this);
	}
	Grouping(const Grouping<T> &other)
	{
		if (this == &other)
			throw std::runtime_error("Error: Self assignment");
		this->expression = other.expression->clone();
	}
	Grouping(const Grouping<T> *other)
	{
		if (this == other)
			throw std::runtime_error("Error: Self assignment");
		this->expression = other->expression;
	}
	Grouping(Expr<T> &expression = nullptr) : expression(new Expr<T>(expression)) {}
	Grouping(Expr<T> *expression = nullptr) : expression(expression) {}
/*	void operator=(Grouping<T> *other)
	{
		if (this == other)
			return;
		free();
		this->expression = new Expr<T>(other->expression[0]);
	}
*/
	void free()
	{
		delete this->expression;
	}
	~Grouping() override
	{
		free();
	} 
};

template <typename T>
class Literal : public Expr<T>{
public:
	std::any value;
	virtual Expr<T> *clone()
	{
		return new Literal<T>(this);
	}
	T accept(visitor<T> *visitor) override
	{
		return visitor->visit(this);
	}
	Literal(const Literal<T> &other)
	{
		if (this == &other)
			throw std::runtime_error("Error: Self assignment");
		this->value = other.value;
	}
	Literal(const Literal<T> *other)
	{
		if (this == other)
			throw std::runtime_error("Error: Self assignment");
		this->value = other->value;
	}
	Literal(std::any value) : value(value) {}
	Literal() {}
	virtual ~Literal() override = default;
};

template <typename T>
class Unary : public Expr<T> {
public:
	token Operator;
	Expr<T> *right = nullptr;
	virtual Expr<T> *clone()
	{
		return new Unary<T>(*this);
	}
	T accept(visitor<T> *visitor) override
	{
		return visitor->visit(this);
	}
	Unary(const Unary<T> &other)
	{
		if (this == &other)
			throw std::runtime_error("Error: Self assignment");
		this->Operator = other.Operator;
		this->right = other.right->clone();
	}
	Unary(const Unary<T> *other)
	{
		if (this == other)
			throw std::runtime_error("Error: Self assignment");
		this->Operator = other->Operator;
		this->right = other->right;
	}
	Unary(token &Operator, Expr<T> &right) :
		Operator(Operator), right(new Expr<T>(right)) {}
	Unary(token &Operator, Expr<T> *right) :
		Operator(Operator), right(right) {}
	void free()
	{
		delete this->right;
	}
	~Unary() override
	{
		free();
	}
};

template <typename T>
class Variable : public Expr<T> {
public:
	token name;
	Variable(token name) : name(name) {};
	virtual Expr<T> *clone()
	{
		return new Variable<T>(*this);
	}
	Variable(const Variable<T> &other)
	{
		if (this == &other)
			throw std::runtime_error("Error: Self assignment");
		this->name = other.name;
	}
	Variable(const Variable<T> *other)
	{
		if (this == other)
			throw std::runtime_error("Error: Self assignment");
		this->name = other->name;
	}
	T accept(visitor<T> *visitor) override
	{
		return visitor->visit(this);
	}
	~Variable() = default;
};

template <typename T>
class Assign : public Expr<T> {
public:
	token name;
	Expr<T> *value = nullptr;
	virtual Expr<T> *clone()
	{
		return new Assign<T>(*this);
	}
	Assign(const Assign<T> &other)
	{
		if (this == &other)
			throw std::runtime_error("Error: Self assignment");
		this->name = other.name;
		this->value = other.value->clone();
	}
	Assign(const Assign<T> *other)
	{
		if (this == other)
			throw std::runtime_error("Error: Self assignment");
		this->name = other->name;
		this->value = other->value;
	}
	Assign(token name, Expr<T> &value) :
		name(name), value(new Expr<T>(value)) {}
	Assign(token name, Expr<T> *value) :
		name(name), value(value) {}
	T accept(visitor <T> *visitor) override
	{
		return visitor->visit(this);
	}
	~Assign()
	{
		delete this->value;
	}
};

template <typename T>
class Logical : public Expr<T> {
public:
	Expr<T> *left = nullptr;
	token Operator;
	Expr<T> * right = nullptr;
	virtual Expr<T> *clone()
	{
		return new Logical<T>(*this);
	}
	Logical(const Logical<T> &other)
	{
		if (this == &other)
			throw std::runtime_error("Error: Self assignment");
		this->left = other.left->clone();
		this->Operator = other.Operator;
		this->right = other.left->clone();
	}
	Logical(const Logical<T> *other)
	{
		if (this == other)
			throw std::runtime_error("Error: Self assignment");
		this->left = other->left;
		this->Operator = other->Operator;
		this->right = other->right;
	}
	T accept(visitor<T> *visitor) override
	{
		return visitor->visit(this);
	}
	Logical(Expr<T> &left, token Operator, Expr<T> &right) :
		left(left.clone()), Operator(Operator), right(right.clone()) {}
	Logical(Expr<T> *left, token Operator, Expr<T> *right) :
		left(left), Operator(Operator), right(right) {}

	virtual ~Logical() override
	{
		delete this->left;
		delete this->right;
	}
};

template <typename T>
struct Ternary : public Expr<T> {

	Expr<T> *condition = nullptr;
	Expr<T> *then = nullptr;
	Expr<T> *_else = nullptr;
	
	T accept(visitor<T> *visitor) override
	{
		return visitor->visit(this);
	}
	Expr<T> *clone() override
	{
		return new Ternary<T>(*this);
	}
	Ternary(const Ternary<T> &other)
	{
		this->condition = other.condition->clone();
		this->then = other.then->clone();
		this->_else = other._else->clone();
	}
	Ternary(const Ternary<T> *other)
	{
		if (this == other)
			throw std::runtime_error("Error: Self assignment");
		this->condition = other->condition;
		this->then = other->then;
		this->_else = other->_else;
	}
	Ternary(Expr<T> *condition, Expr<T> *then, Expr<T> *_else) : condition(condition), then(then), _else(_else) {}

	virtual ~Ternary() override
	{
		delete this->condition;
		delete this->then;
		delete this->_else;
	}
};
template <typename T>
struct Increment : public Expr<T> {
	token target_name;
	token Operator;
	bool postfix;
	T accept(visitor<T> *visitor) override {return visitor->visit(this); }
	Expr<T> *clone()
	{
		return new Increment<T>(*this);
	}
	Increment(const Increment &other) : target_name(other.target_name), Operator(other.Operator), postfix(other.postfix) {}
	Increment(const token &target_name, const token &Operator, const bool &postfix) : target_name(target_name), Operator(Operator), postfix(postfix) {}
	~Increment() override = default;
};


template <typename T>
struct Call : public Expr<T> {

	Expr<T> *callee = nullptr;
	token paren;
	argument<T> arguments;
	
	T accept(visitor<T> *visitor) override
	{
		return visitor->visit(this);
	}
	Expr<T> *clone() override
	{
		return new Call<T>(*this);
	}
	Call (const Call<T> &other) : callee(other.callee->clone()), paren(other.paren), arguments(other.arguments) {}
	Call(Expr<T> *callee, token paren, argument<T> &arguments) : callee(callee), paren(paren), arguments(arguments) {}

	virtual ~Call() override
	{
		delete this->callee;
	}
};
#endif
