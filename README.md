# cpplox
The lox programming language in C++ a work in progress.
Lox is a high level garbage-collected scripting language designed for the book Crafting Interpreters by Robert Nystrom.

# Features:

-C style comments /*not nested*/

-Post and prefix increment / decrement operators

-Break statement

-Capturing lambdas

-Ternary operator "?"

-Native C++ functions work, clock() function is included.

# Todo:


-Split hpp and cpp files

-Add classes and methods

# Limitations:
Mostly the same as the original Lox, C++ is non garbage collected thus the funarg problem becomes evident, I worked around it by writing a small arena that acts like a gc and permits reference cycles.

# Examples:
```python
fun f(a, b) 
{
  return a + b;
}
print f(1, 2); //3
```
```python
lambda() {print "Hello world!";}(); //inline lambda call
```
```python
fun f()
{
  var i = 0;
  return lambda() {print i++;}; //lambda captures environment and increases value
}

var a = f();
a(); //prints 0
a(); //prints 1

```
