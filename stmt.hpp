#ifndef LOX_STMT
#define LOX_STMT
#include "expr.cpp"
#include <stdexcept>

template <typename T>
class visitor;

template <typename T>
class Stmt {
public:
	virtual ~Stmt() = default;
	virtual void accept(visitor<T> *visitor) {return visitor->visit(this);}
	virtual Stmt<T> *clone() {return new Stmt<T>(*this);}
	Expr<T> *expression = nullptr;
};

template <typename T>
using statement_list = delete_pointer_vector<Stmt<T>>;

template <typename T>
class Expression : public Stmt<T> {
public:
	virtual Stmt<T> *clone() override
	{
		return new Expression(*this);
	}
	Expression(const Expression<T> &other)
	{
		if (this == &other)
			throw std::runtime_error("Error: Self assignment");
		this->expression = other.expression->clone();
	}
	Expression(const Expression<T> *other)
	{
		if (this == other)
			throw std::runtime_error("Error: Self assignment");
		this->expression = other->expression;
	}
	Expression(Expr<T> *expression) : expression(expression) {}
	Expression(Expr<T> &expression) : expression(expression.clone()) {}
	void accept(visitor<T> *visitor) override
	{
		return visitor->visit(this);
	}
	Expr<T> *expression = nullptr;
	~Expression()
	{
		delete this->expression;
	}
};

template <typename T>
class Print : public Stmt<T> {
public:
	virtual Stmt<T> *clone() override
	{
		return new Print(*this);
	}
	Print(const Print<T> &other)
	{
		if (this == &other)
			throw std::runtime_error("Error: Self assignment");
		this->expression = other.expression->clone();
	}
	Print(const Print<T> *other)
	{
		if (this == other)
			throw std::runtime_error("Error: Self assignment");
		this->expression = other->expression;
	}
	Print(Expr<T> *expression) : expression(expression) {}
	Print(Expr<T> &expression) : expression(expression.clone()) {}
	void accept(visitor<T> *visitor) override
	{
		return visitor->visit(this);
	}
	Expr<T> *expression = nullptr;
	~Print()
	{
		delete this->expression;
	}
};

template <typename T>
class Var : public Stmt<T> {
public:
	virtual Stmt<T> *clone() override
	{
		return new Var(*this);
	}
	Var(const Var<T> &other)
	{
		if (this == &other)
			throw std::runtime_error("Error: Self assignment");
		this->name = other.name;
		this->initializer = other.initializer ? other.initializer->clone() : nullptr;
		//it may not have an initializer we can clone
	}
	Var(const Var<T> *other)
	{
		if (this == other)
			throw std::runtime_error("Error: Self assignment");
		this->name = other->name;
		this->initializer = other->initializer;
	}
	Var(token name, Expr<T> *initializer) : initializer(initializer), name(name) {}
	Var(token name, Expr<T> &initializer) : initializer(initializer.clone()), name(name) {}
	void accept(visitor<T> *visitor) {return visitor->visit(this);}
	Expr<T> *initializer = nullptr;
	~Var()
	{
		delete this->initializer;
	}
	token name;
};

template <typename T>
class Block : public Stmt<T> {
public:
	Block(const Block &other)
	{
		this->statements = other.statements;
	}
	virtual Stmt<T> *clone() override
	{
		return new Block(*this);
	}
	Block(statement_list<T> &statements) : statements(statements) {}
	Block() {}
	~Block() override = default;
	void accept(visitor<T> *visitor) {return visitor->visit(this);}
	statement_list<T> statements;
};

template <typename T>
class If : public Stmt<T> {
public:
	virtual Stmt<T> *clone() override
	{
		return new If(*this);
	}
	If(const If<T> &other)
	{
		if (this == &other)
			throw std::runtime_error("Error: Self assignment");
		this->condition = other.condition->clone();
		this->Then = other.Then->clone();
		if (other.Else)
			this->Else = other.Else->clone();
	}
	If(const If<T> *other)
	{
		if (this == other)
			throw std::runtime_error("Error: Self assignment");
		this->condition = other->condition;
		this->Then = other->Then;
		this->Else = other->Else;
	}
	~If()
	{
		delete this->condition;
		delete this->Else;
		delete this->Then;
	}
	If(Expr<T> *condition, Stmt<T> *Then, Stmt<T> *Else) :
		condition(condition), Else(Else), Then(Then) {}
	If(Expr<T> &condition, Stmt<T> &Then, Stmt<T> &Else) :
		condition(condition.clone()), Else(Else.clone()), Then(Then.clone()) {}
	void accept(visitor<T> *visitor) {return visitor->visit(this);}
	Expr<T> *condition = nullptr;
	Stmt<T> *Else = nullptr;
	Stmt<T> *Then = nullptr;
};

template <typename T>
class While : public Stmt<T> {
public:
	virtual Stmt<T> *clone() override
	{
		return new While(*this);
	}
	While(const While<T> &other)
	{
		if (this == &other)
			throw std::runtime_error("Error: Self assignment");
		this->condition = other.condition->clone();
		this->Then = other.Then->clone();
	}
	While(const While<T> *other)
	{
		if (this == other)
			throw std::runtime_error("Error: Self assignment");
		this->condition = other->condition;
		this->Then = other->Then;
	}
	While(Expr<T> &condition, Stmt<T> &Then) : condition(condition.clone()), Then(Then.clone()) {}
	While(Expr<T> *condition, Stmt<T> *Then) : condition(condition), Then(Then) {}
	~While()
	{
		delete this->condition;
		delete this->Then;
	}
	void accept(visitor<T> *visitor) {return visitor->visit(this);}
	Expr<T> *condition = nullptr;
	Stmt<T> *Then = nullptr;
};

template <typename T>
class Break : public Stmt<T>, std::exception {
	virtual Stmt<T> *clone()
	{
		return new Break<T>();
	}
	void accept(visitor<T> *visitor) {return visitor->visit(this);}
};

template <typename T>
class Function : public Stmt<T> {
public:
	virtual Stmt<T> *clone() override
	{
		return new Function<T>(*this);
	}
	void accept(visitor<T> *visitor) {return visitor->visit(this);}
	Function(token name, std::vector<token> &params, statement_list<T> &body) : name(name), params(params), body(body) {}
	Function(const Function &other)
	{
		if (this == &other)
			throw std::runtime_error("Error: Self assignment");
		this->name = other.name;
		this->params = other.params;
		this->body = other.body;
	}
	token name;
	std::vector<token> params;
	statement_list<T> body;
	~Function() override = default;
};

template <typename T>
class Return : public Stmt<T> {
public:
	virtual Stmt<T> *clone() override
	{
		return new Return<T>(*this);
	}
	void accept(visitor<T> *visitor) {return visitor->visit(this);}
	Return(const Return &other)
	{
		if (this == &other)
			throw std::runtime_error("Error: Self assignment");
		if (other.value)
			this->value = other.value->clone();
		this->keyword = other.keyword;
	}
	explicit Return(token &keyword, Expr<T> *value) : keyword(keyword), value(value) {}
	//Return(token &keyword, Expr<T> &value) : keyword(keyword), value(value.clone()) {}
	token keyword;
	Expr<T> *value = nullptr;
	~Return() override
	{
		delete this->value;
	}
};
#endif
