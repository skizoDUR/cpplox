#include "lox.hpp"
#include "interpreter.hpp"
#include "parser.hpp"
#include "resolver.hpp"
#include <memory>

bool lox::had_error = 0;
bool lox::had_runtime_error = 0;
bool lox::repl_mode = 0;
interpreter<std::any> lox::Interpreter;

lox::lox()
{
	std::ios_base::sync_with_stdio(false);
}

void lox::run(std::string source)
{
	scanner scan(source);
	std::vector<token> tokens = scan.scan_tokens();
	parser<std::any> Parser(tokens);
	std::vector<std::unique_ptr<Stmt<std::any>>> statements;
	statements = Parser.parse();
	if (lox::had_error || lox::had_runtime_error)
		return;
	Resolver<std::any> resolver(Interpreter);
	resolver.resolve(statements);
	if (lox::had_error)
		return;
	lox::Interpreter.interpret(statements);
}

void lox::run_file(const char *path)
{
	std::ifstream read(path);
	std::stringstream buffer;
	buffer << read.rdbuf();
	run(buffer.str());
	had_error = 0;
	had_runtime_error = 0;
}

void lox::run_prompt()
{

	for (;;) {
		std::string input;
		std::cout << "> ";
		if (!(std::getline(std::cin, input)))
			break;
		run(input);
		had_error = 0;
		had_runtime_error = 0;
	}

//	auto s = "1 + 1";
//	lox::run(s);
}

void lox::report(int line, std::string where, std::string &msg)
{
	std::cerr << "[line " << line << "] Error " << where << ": " << msg << '\n';
	had_error = 1;
}

void lox::error(int line, std::string msg)
{
	report(line, " ", msg);
}

void lox::error(token token, std::string msg)
{
	if (token.type == token_type::eof)
		report(token.line, "at end of input", msg);
	else
		report(token.line, "at '" + token.lexeme + "'", msg);
}

void lox::runtime_error(runtime_exception error)
{
	std::cerr << "[line " << error.Token.line << "] " << error.msg << '\n';
	lox::had_runtime_error = 1;
}
