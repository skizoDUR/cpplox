#ifndef CAST_UNIQUE_PTR
#define CAST_UNIQUE_PTR
#include <memory>
template <typename T, typename R>
std::unique_ptr<T> cast_unique_ptr(std::unique_ptr<R> &source)
{
	T *test = dynamic_cast<T*>(source.get()); //try cast
	if (!test)
		return {};
	std::unique_ptr<T> ret = std::make_unique<T>(*test); //copy object
	source.reset(); //free old ptr
	return ret;
}
template <typename T, typename R>
bool ptr_is_instance_of(std::unique_ptr<R> &source)
{
	T *test = dynamic_cast<T*>(source.get());
	return test;
}
#endif
