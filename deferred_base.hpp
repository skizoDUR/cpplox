#ifndef DEFERRED_BASE_HPP
#define DEFERRED_BASE_HPP
class deferred_base {
public:
	virtual void mark() = 0;
	virtual ~deferred_base() = default;
};
#endif
