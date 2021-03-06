#ifndef ALLOC_HPP
#define ALLOC_HPP
#include <cstddef>
#include <cstdint>
#include <cassert>
#include <utility>
class alloc {
	template <typename T>
	friend class deferred_ptr;
	friend class deferred_heap;
	bool alive;
	uint8_t *data = nullptr;
	size_t size;
	void (*dtor)(void*) = nullptr;
	void allocate(size_t size);
	void deallocate();
	void destroy();
public:
	~alloc();
	template <typename T, typename ...Args>
	T *construct(Args ...args)
	{
		assert(sizeof(T) <= size && "Incorrect data size for type T");
		T *ptr = new (data) T(std::forward<T>(args)...);
		dtor = [](void*x){static_cast<T*>(x)->~T();};
		return ptr;
	}
	alloc(size_t size);
	bool operator==(const alloc &other) const noexcept;
	alloc(const alloc &other) = delete;
	const alloc &operator=(const alloc &other) = delete;
	alloc(alloc &&other) noexcept;
	const alloc &operator=(alloc &&other) noexcept;
};
#endif
