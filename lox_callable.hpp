/*#ifndef LOX_CALLABLE_HPP
#define LOX_CALLABLE_HPP
#include <vector>
template <class T>
class interpreter;

template <class T>
class lox_callable {
public:
	virtual int arity() {return 0;};
	virtual T call(interpreter<T> &interpreter, std::vector<T> &arguments) {return {};};
	virtual ~lox_callable() = default;
};
#endif
*/
