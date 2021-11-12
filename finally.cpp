#ifndef FINALLY
#define FINALLY
#include <functional>
#include <iostream>
struct finally {
	std::function<void()> function;
	finally(const std::function<void()> &function) : function(function) {};
	~finally() {function();};
};
#endif
