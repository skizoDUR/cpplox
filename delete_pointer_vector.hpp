#ifndef DELETE_POINTER_VECTOR_HPP
#define DELETE_POINTER_VECTOR_HPP
#include <vector>
template <typename T>
struct delete_pointer_vector : public std::vector<T*> {

	delete_pointer_vector() {}
	void free()
	{
//		for (auto &i : *this)
//			delete i;
		
		for (size_t i = 0; i < this->size(); i++)
			delete this->at(i);
		this->clear();
	}
	~delete_pointer_vector()
	{
		free();
	}
	delete_pointer_vector(const delete_pointer_vector<T> &other)
	{
		operator=(other);
	}
	void operator=(const delete_pointer_vector<T> &other)
	{
		free();
		for (auto &i : other)
			if (i)
				this->push_back(i->clone());
	}
	
};
#endif
