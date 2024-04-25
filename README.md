# Popcorn --- a really bad language

Compile with `make`. Or do it manually if you liek pain.

Run with `./pop` the name of the file you want to run (i.e., `./pop main.pop`).

* One command line argument is available for debugging `-d`.

The pop executable must have its working directory set to the directory of the file you want to run.

---
The language is very primitive and buggy so far. But it works. Kind of. Well, at least it should. Maybe. Anyways, here is a look so far at the barbarian style language that is popcorn.

As you would expect, data types don't change that often.
You have your basics:
```go
int32
float32
char
bool
string
```

Not strictly typed but type inforced. (Really not fun to use)
```cpp
a = 0.0 // float32
a = 0 // int32
a = "" // string
a = true | false // bool
a = '!' // char

// yeah, you can't do this
a = 1 + 1.0 // <- will crash ... must cast

// you can however do this
a = 1 + int(1.0) // <- should work ... unsure

// anyone wanna help me figure out why I can't do this though?
a = int(-1.5) // <- for some reason this fails \_(*_*)_/
// never mind ^ works now!
```
The different casting functions are: (bad, maybe not though idk)
```cpp
int(x)
float(x)
char(x)
bool(x)
str(x)
```
Functions are parsed but not running yet.
```go
func myFunc 
{

}
// yeah, that's a valid function definition ^. Fight me.

// ok ok, here are some parameters
func myFunc(a, b, c)
{
    ret a + b + c
}

// eventually you will be able to call
myFunc(1, 2, 3)
```
You got your basic operations:
```go
+
-
*
/
%
==
!=
>=
<=
>
<

// should also be able to negate number... i say should
a = -1
```
Also got your basic statements: (if it crashes with these i can't help, I'm probably doing things too important to listen to you ramble on about my trash bin of a language)
```cpp
if condition {

} else if condition { // ive got a sneaky suspicion that this one will fail

} else {

}

while condition {

}
// while loops don't get a break by the way (lol.. puns)
```
I think that's about it for now. Thanks for taking a look!







OH! WAIT!
```
print("You can do this as well")
```
