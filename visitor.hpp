#ifndef VISITOR_HPP
#define VISITOR_HPP
#include "expr.cpp"
#include "stmt.hpp"

template <typename T>
class visitor {
public:
	virtual T visit(Expr<T> *) = 0;
	virtual T visit(Binary  <T> *) = 0;
	virtual T visit(Grouping<T> *) = 0;
	virtual T visit(Literal <T> *) = 0;
	virtual T visit(Unary   <T> *) = 0;
	virtual T visit(Variable<T> *) = 0;
	virtual T visit(Assign<T> *) = 0;
	virtual T visit(Logical<T> *) = 0;
	virtual T visit(Ternary<T> *) = 0;
	virtual T visit(Increment<T> *) = 0;
	virtual T visit(Call<T> *) = 0;
	virtual T visit(Lambda<T> *) = 0;
	
	//statements
	virtual void visit(Stmt<T> *) = 0;
	virtual void visit(Expression<T> *) = 0;
	virtual void visit(Print<T> *) = 0;
	virtual void visit(Var<T> *) = 0;
	virtual void visit(Block<T> *) = 0;
	virtual void visit(If<T> *) = 0;
	virtual void visit(While<T> *) = 0;
	virtual void visit(Break<T> *) = 0;
	virtual void visit(Function<T> *) = 0;
	virtual void visit(Return<T> *) = 0;
	virtual ~visitor()  = default;
};
#endif
