#ifndef FINALLY_HPP
#define FINALLY_HPP
template <typename T>
struct finally {
	T function;
	finally(const T &function) : function(function) {}
	~finally() {function();};
};
#define CONCAT_INNER(a, b) a ## b
#define CONCAT(a, b) CONCAT_INNER(a, b) //requires one level of indirection to expand __LINE__
#define UNIQUE_NAME(base) CONCAT(base, __LINE__)
#define finally(ARGS)						\
	auto UNIQUE_NAME(finally_lambda) = [&]()->void{ARGS;};		\
	auto UNIQUE_NAME(finally_obj) =	finally<decltype(UNIQUE_NAME(finally_lambda))>(UNIQUE_NAME(finally_lambda)); \

//the only limit is only one finally "statement" per line, this doesn't use std::function so no unnecessary overhead
#endif
