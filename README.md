# Popcorn How To

## About

Popcorn is a language created in C++ for fun.<br>
Compile with `make`.

### Operators

* as rows increase precedence increases

| Operator | Name | Order Number |
|----------|------|--------------|
| && | AND | 1 |
| \|\| | OR | 1 |
| == | Equals | 2 |
| != | Not Equals | 2 |
| > | Greater Than | 2 |
| < | Less Than | 2 |
| >= | Greater Than or Equal to | 2 |
| <= | Less Than or Equal to | 2 |
| + | Add | 3 |
| - | Subtract | 3 |
| * | Multiply | 4 |
| % | Modulus | 4 |
| / | Divide | 4 |
| ^ | Power (only with integer exponents) | 5 |
| ++ | Increments a variable by 1 | not really an operator |
| -- | Decrements a variable by 1 | not really an operator |

### Terms

| Example | Type | 
|---------|------|
| 10 | integer |
| 10.0 | double |
| "Hello, world!" | string |
| ( ... ) | Expression |
| 1 + 1 | Expression |
| true | boolean |
| false | boolean |

### Built in functions

* currently, casts do not stick

| Function Name | Does What |
|---------------|-----------|
| print( value ) | prints a line of text to console |
| printl( value ) | prints a line of text with a new line to console |
| input() | gets a line of user input from console |
| int( value ) | casts a value to an integer |
| double( value ) | casts a value to a double |
| bool( value ) | casts a value to a boolean |
| string( value ) | casts a value to a string |

### How to declare variables

```c++
x = 0
y = "Hello, world!"
z = true
```

Variables have local and global scopes.

If a variable not declared globally is declared within a child's body. It will be scoped locally. Local variables cannot have the same name as a global variable. In other words, a child's scope cannot have variables with the same name as variables within the parent's scope. Unless, of course, you are trying to access the global variables. That is totally fine.

Example:

```c++
y = 0

{
    x = 0 // declares a local variable x
    y = 1 // sets the global y variable to 1
}
```

### How to declare if statments

```c++
if [Expression]
{
    ...
}
else if [Expression]
{
    ...
}
else
{

}
```

### How to declare while statements

```c++
while [Expression]
{

}
```

### How to declare functions

Functions are declared as follows:

```c++
func test()
{
    printl("Hello, world!")
}
```

```c++
func my_func()
{
    x = 1

    return x
}
```

The example above returns a value of 1.

Parameters are passed simply:

```c++
func add(x, y)
{
    return x + y
}
```

Functions do not have to be defined on lines prior to being called. The interpreter caches all of the declared functions at runtime so it doesn't matter where you call the function.

Calling a function is as normal:

```c++
add(1, 1) // returns 2
```

You can also catch the returned value:

```c++
x = add(1, 1)
printl("The value of x is: " + x)
```

## Notes

Since this is not a strongly typed language. Many automatic casts are done depending on what variables you are using in combination.

For example:

```c++
x = 0
y = "Hello, world!"

printl(y + x)
```

In the example above, x is implicitly an integer and y implicitly a string. When the interpreter sees that you want to add y to x, it automatically casts x to a string so that it can perform a concatination. Variables placed on the left side of operands determine what the right variable will be cast to.

i.e.,

```c++
x1 = 2.0 + 1
// is not the same as
x2 = 2 + 1.0
```

x1 is ultimately a decimal while x2 becomes an integer.