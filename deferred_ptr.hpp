#ifndef DEFERRED_PTR_HPP
#define DEFERRED_PTR_HPP
#include "deferred_base.hpp"
#include <forward_list>

template <typename T>
class deferred_ptr : private deferred_base {
public:
	bool *alive = nullptr;
	T *data = nullptr;
	std::forward_list<deferred_base *> *roots = nullptr;
	void mark() override
	{
		if (alive)
			*alive = true;
	}
	void de_register()
	{
		if (roots)
			roots->remove(this);
	}
	void do_register()
	{
		if (roots)
			roots->push_front(this);
	}
	deferred_ptr(){}
	deferred_ptr(bool *alive, T*data, std::forward_list<deferred_base*> *roots) : alive(alive) , data(data), roots(roots) {roots->push_front(this);}
	deferred_ptr(const deferred_ptr &other)
	{
		if (this == &other) return;
		alive = other.alive;
		data = other.data;
		roots = other.roots;
		do_register();
	}
	const deferred_ptr &operator=(const deferred_ptr &other)
	{
		if (this == &other) return *this;
		de_register();
		alive = other.alive;
		data = other.data;
		roots = other.roots;
		do_register();
		return *this;
	}
	deferred_ptr(deferred_ptr &&other)
	{
		alive = other.alive;
		data = other.data;
		roots = other.roots;
		do_register();
		other.de_register();
	}
	deferred_ptr &operator=(deferred_ptr &&other)
	{
		if (this == &other) return *this;
		de_register();
		alive = other.alive;
		data = other.data;
		roots = other.roots;
		do_register();
		other.de_register();
		return *this;
	}
	~deferred_ptr()
	{
		de_register();
	}
	T *operator->()
	{
		return data;
	}
	T &operator*()
	{
		return *data;
	}
	operator bool()
	{
		return data;
	}
};
#endif
