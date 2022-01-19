#ifndef LOX_FUNCTION_HPP
#define LOX_FUNCTION_HPP
#include "env_ptr.hpp"
#include "lox_callable.hpp"
#include "return.hpp"
#include "token.cpp"
#include "delete_pointer_vector.hpp"
#include "environment.hpp"
#include "stmt.hpp"


template<typename T>
struct lox_function final : public lox_callable<T> {
	Function<T> &declaration;
	environment *closure = nullptr;
	lox_function(Function<T> &declaration, environment &closure) : declaration(declaration), closure(new environment(closure)) {}
	int arity() override
	{
		return declaration.params.size();
	}
	T call(interpreter<T> &interpreter, std::vector<T> &arguments) override
	{
		auto Environment = new environment(closure);
		for (int i = 0; i < (int)declaration.params.size(); i++)
			Environment->define(declaration.params[i].lexeme, arguments[i]);
		finally (

			delete Environment;
			)
		try {
			interpreter.execute_block(declaration.body, Environment);
		}
		catch (Return_value &ret) {
			return ret.value;
		}
		
		return {};
	}
	~lox_function() override
	{
		delete this->closure;
	}
	      
};

#endif
