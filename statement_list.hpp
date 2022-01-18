#ifndef STATEMENT_LIST_HPP
#define STATEMENT_LIST_HPP
#include <vector>
template <typename T>
class Stmt;

template <typename T>
class statement_list : public std::vector<Stmt<T>*> {
public:
	statement_list()
	{
		
	}

	statement_list(const statement_list &other)
	{
		for (auto &i : other) {
			if (i)
				this->push_back(i->clone());
		}
	}

	~statement_list()
	{
		for (auto &i : *this)
			delete i;
	}

};
#endif
