# cpplox
The lox programming language in C++ (WIP)

# Features:

-C style comments /*not nested*/

-Post and prefix increment / decrement operators

-Break statement

-Capturing lambdas

-Ternary operator "?"

-Native C++ functions work, clock() function is included.

# Todo:

-Improve environment capturing

-Split hpp and cpp files

-Add classes and methods

# Limitations:
Functions and lambdas environments are not static, you get a new environment on each call and gets destroyed at the end of the function's life, this is due to the type system using std::any which is copy only.

Functions can capture outer variables but BY COPY only, means they can't update them. This is because C++ doesn't have a garbage collector and the captured environments may go out of scope at any moment.


