#ifndef LOX
#define LOX
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "runtime_error.cpp"
#include "scanner.hpp"
#include "token_type.hpp"
template<typename T>
class interpreter;

class lox {
public:
	static interpreter<std::any> Interpreter;
	static bool had_error;
	static bool repl_mode;
	static bool had_runtime_error;
	static void run(std::string);
	static void run_file(const char *);
	static void run_prompt();
	static void report(int , std::string , std::string &);
	static void error(int, std::string);
	static void error(token, std::string);
	static void runtime_error(runtime_exception);
};
#endif
