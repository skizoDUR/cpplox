#include "alloc.hpp"
void alloc::allocate(size_t size)
{
	this->size = size;
	data = new uint8_t[size];
}
void alloc::deallocate()
{
	if (data)
		delete[] data;
}
void alloc::destroy()
{
	if (dtor && data)
		dtor(data);
}
alloc::alloc(size_t size)
{
	allocate(size);
}
bool alloc::operator==(const alloc &other) const noexcept
{
	return data == other.data;
}
alloc::~alloc()
{
	destroy();
	deallocate();
}
alloc::alloc(alloc &&other) noexcept
{
	if (this == &other)
		return;
	alive = other.alive;
	data = other.data;
	size = other.size;
	dtor = other.dtor;
	other.data = nullptr;
	other.size = 0;
	other.dtor = nullptr;
}
const alloc &alloc::operator=(alloc &&other) noexcept
{
	if (this == &other)
		return *this;
	deallocate(), destroy();
	alive = other.alive;
	data = other.data;
	size = other.size;
	dtor = other.dtor;
	other.data = nullptr;
	other.size = 0;
	other.dtor = nullptr;
	return *this;
}

