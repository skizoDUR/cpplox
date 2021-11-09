#include <iostream>
#include "lox.hpp"
#include "scanner.hpp"

int main(int argc, char **argv)
{
	if (argc > 2) {
		std::cerr << "Usage: " << argv[0] << " [script]\n";
		return -1;
	}
	else if (argc == 2) {
		lox::run_file(argv[1]);
	}
	else {
		lox::repl_mode = 1;
		lox::run_prompt();
	}
		
}
