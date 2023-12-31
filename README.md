# Popcorn How To

## About

Popcorn is a language created in c++ for fun.

Eventually files will be ended with the extension `.pop` to run.

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
| / | divide | 4 |

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
| double( value ) | casts a double to a double |
| bool( value ) | casts a bool value to a double |

### How to declare variables

* currently, all variables declared are global

```c++
x = 0
y = "Hello, world!"
z = true
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