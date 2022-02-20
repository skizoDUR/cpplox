#ifndef DEFERRED_HEAP_HPP
#define DEFERRED_HEAP_HPP
#include <forward_list>
#include "alloc.hpp"
#include "deferred_base.hpp"
#include "deferred_ptr.hpp"
class deferred_heap {
	std::forward_list<alloc> heap;
	std::forward_list<deferred_base *> roots;
	void sweep(std::forward_list<alloc>::iterator);
public:
	template <typename T, typename ...Args>
	deferred_ptr<T> make(Args ...args)
	{
		heap.emplace_front(sizeof(T));
		T *data = heap.front().construct<T>(std::forward<T>(args)...);
		deferred_ptr<T> ret(&heap.front().alive, data, &roots);
		return ret;
	}
	void collect();
};
#endif
