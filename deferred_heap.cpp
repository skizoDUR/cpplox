#include "deferred_heap.hpp"
#include "iostream"
void deferred_heap::sweep(std::forward_list<alloc>::iterator start)
{
	if (start == heap.end())
		return;
	sweep(std::next(start));
	if (!start->alive)
		heap.remove(*start);
}

void deferred_heap::collect()
{
	for (auto &mark : heap) {
		mark.alive = false;
	}

	for (auto &remark : roots) {
		remark->mark();
	}

	sweep(heap.begin());
}
