#ifndef ARGUMENT_T_CPP
#define ARGUMENT_T_CPP
#include <vector>
template <class T>
class Expr;
//argument list for function calls
template <class T>
class argument_t : public std::vector<Expr<T>> {
	argument_t() {}
	argument_t(const argument_t &other)
	{
		for (auto i : other) {
			if (i)
				this->push_back(i->clone());
		}
	}
	~argument_t()
	{
		for (auto i : *this)
			delete i;
	}
};
#endif
