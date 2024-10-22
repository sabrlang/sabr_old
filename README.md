[![GitHub license](https://img.shields.io/github/license/sabrlang/sabr?style=for-the-badge)](https://github.com/sabrlang/sabr/blob/main/LICENSE)

[한국어](korean.md)

# Sabr
Sabr is simple stack-oriented programming language.

# Build from source
```sh
git clone --depth=1 https://github.com/sabrlang/sabr.git
cd sabr
mkdir build
cd build
cmake ..
make
```

# Examples
## Arithmetic
```
2 5 + puti 2.5 3.14 f* putf
```
```
7 7.850000
```

## Print "Hello, world!" with macro
```
$puts macro
	loop dup 0 > while swap putc -- end drop
end

"Hello, world!" puts
```
```
Hello, world!
```

## Simple loop with local variables
```
$cr macro '\n' putc end
$draw func
	$count set
	$i for count to
		$j for i ++ to
			'*' putc
		end
		cr
	end
end

5 draw
10 draw
```
```
*
**
***
****
*****
*
**
***
****
*****
******
*******
********
*********
**********
```

# How to run
## Compile to bytecode
```sh
$ sabrc {source file name} {output file name}
```
## Run bytecode
```sh
$ sabre {bytecode file name}
```

# Specification
Sabr programs must be written in UTF-8.
## Data types
Sabr has no type checking. All stack values are 8 bytes wide.
* `x` : Cell (any)
* `n` : Integer
* `s` : Signed integer
* `u` : Unsigned integer
* `f` : Float-point
* `b` : Boolean
* `addr` : Address
* `id` : Identifier

## Control
### Conditionals
#### if statements
False is 0, and true is non-zero values.

```
(flag) if
	(code)
end
```
`if` pops the flag value from the stack.  
If `(flag)` is true, `(code)` is executed.

```
(flag) if
	(code 1)
else
	(code 2)
end
```
If `(flag)` is true, `(code 1)` is executed.  
If `(flag)` is false, `(code 2)` is executed.


#### switch statements
```
(value) switch
	(case 1) case (code 1) pass
	(case 2) case (code 2) pass
	(case 3) case
	(case 4) case
		(code 3)
	pass
	(code 4)
end
```

`switch` and `case` pop the value from the stack.  
If `(value)` is equal to `(case 1)`, `(code 1)` is executed.  
If `(value)` is equal to `(case 3)` or `(case 4)`, `(code 3)` is executed.  
If there is no matching value, `(code 4)` is executed.

##### if-elif-else style
```
(non-zero) switch
	(flag 1) case (code 1) pass
	(flag 2) case (code 2) pass
	(flag 3) case
	(flag 4) case
		(code 3)
	pass
	(code 4)
end
```

If `(flag 1)` is true, `(code 1)` is executed.  
If `(flag 3)` or `(flag 4)` is true, `(code 3)` is excuted.  
If every flag is false, `(code 4)` is executed.

### Loops
#### loop statements
```
loop
	(code 1)
end
```

This is an endless loop.

```
loop
	(code)
	(flag)
	while
end
```

`(code)` is excuted. `while` pops the flag value from the stack.  
If `(flag)` is true, the loop is restarted.

```
loop
	(code 1)
	(flag)
	while
	(code 2)
end
```

`(code 1)` is executed. `while` pops the flag value from the stack.  
If `(flag)` is true, `(code 2)` is executed and the loop is restarted.


#### for statements
```
$(keyward) for (start) from (end) to (step) step
	(code)
end 
```

`for`, `from`, `to`, `step` pop the values from stack.  
`(keyward)` will be counter variable's name.

`(start)` is starting value.  
if `(start) from` is not present, `(start)` is treated as 0.

The loop ends when the value of the variable exceeds `(end)`.  
if `(end) to` is not present, the loop will be infinite loop.

Each iteration increments the value of the variable by `(step)`.  
if `(step) step` is not present, `(step)` is treated as 1.

`for` only works with signed integers.  
You can use `ufor` for unsigned integers and `ffor` for floating point.


#### continue and break
```
loop
	continue
end
```

`continue` skips the rest of the current iteration.

```
loop
	break
end
```

`break` terminates the loop.

### Subroutines
```
$(keyword) func
	(code)
end
```

```
$(keyword) macro
	(code)
end
```

### struct
```
$(struct keyword) struct
    $(member keyword 1) member
    $(member keyword 2) member
    $(member keyword 3) member
    ...
end
```

### import
```
#(file) import
```

### Control Keywords
* `if`
* `else`
* `loop`
* `while`
* `break`
* `continue`
* `switch`
* `case`
* `pass`
* `func`
* `macro`
* `return`
* `defer`
* `end`
* `import`

## Built-in operators
### Integer arithmetic
* `+` ( n1 n2 -- n )  
n = n1 + n2
* `-` ( n1 n2 -- n )  
n = n1 - n2
* `*` ( n1 n2 -- n )  
n = n1 × n2
* `0-` ( n1 -- n )  
n = －n1
* `1+` ( n1 -- n )  
n = n1 + 1
* `1-` ( n1 -- n )  
n = n1 - 1

### Integer division
* `/` ( s1 s2 -- s )  
s = s1 ÷ s2
* `%` ( s1 s2 -- s )  
s = s1 mod s2
* `u/` ( u1 u2 -- u )  
u = u1 ÷ u2
* `u%` ( u1 u2 -- u )  
u = u1 mod u2

### Numeric comparison
* `=` ( x1 x2 -- b )
* `!=` ( x1 x2 -- b )

### Integer comparison
* `>` ( s1 s2 -- b )
* `>=` ( s1 s2 -- b )
* `<` ( s1 s2 -- b )
* `<=` ( s1 s2 -- b )
* `u>` ( u1 u2 -- b )
* `u>=` ( u1 u2 -- b )
* `u<` ( u1 u2 -- b )
* `u<=` ( u1 u2 -- b )

### Floating-point
* `f+` ( f1 f2 -- f )  
f = f1 + f2
* `f-` ( f1 f2 -- f )  
f = f1 - f2
* `f*` ( f1 f2 -- f )  
f = f1 × f2
* `f/` ( f1 f2 -- f )  
f = f1 ÷ f2
* `f%` ( f1 f2 -- f )  
f = f1 mod f2
* `f0-` ( f1 -- f )  
f = -f1
* `f=` ( f1 f2 -- b )
* `f!=` ( f1 f2 -- b )
* `f>` ( f1 f2 -- b )
* `f>=` ( f1 f2 -- b )
* `f<` ( f1 f2 -- b )
* `f<=` ( f1 f2 -- b )

### Bitwise operation
* `&` ( x1 x2 -- x )
* `|` ( x1 x2 -- x )
* `^` ( x1 x2 -- x )
* `~` ( x1 -- x )
* `<<` ( x1 u -- x )
* `>>` ( x1 u -- x )

### Stack manipulation
* `drop` ( x -- )
* `nip` ( x1 x2 -- x2 )
* `dup` ( x - x x )
* `over` ( x1 x2 -- x1 x2 x1 )
* `tuck` ( x1 x2 -- x2 x1 x2 )
* `swap`( x1 x2 -- x2 x1 )
* `rot` ( x1 x2 x3 -- x2 x3 x1 )
* `2drop` ( x1 x2 --  )
* `2nip` ( x1 x2 x3 x4 -- x3 x4 )
* `2dup` ( x1 x2 -- x1 x2 x1 x2 )
* `2over` ( x1 x2 x3 x4 -- x1 x2 x3 x4 x1 x2 )
* `2tuck` ( x1 x2 x3 x4 -- x3 x4 x1 x2 x3 x4 )
* `2swap` ( x1 x2 x3 x4 -- x3 x4 x1 x2 )
* `2rot` ( x1 x2 x3 x4 x5 x6 -- x3 x4 x5 x6 x1 x2 )

### Callable and Variables
* `set` ( x id -- )  
```
10 $var set \ Define variable 'var' with an initial value of 10.
20 $var set \ Change the value of variable 'var' to 20.
```
* `call` ( id -- ? )
```
10 $var set \ Define variable 'var' with an initial value of 10.
$var call \ Get value of variable 'var'.
var \ Get value of variable 'var'.

$cr macro 10 putc end \ Define macro 'cr'.
$cr call \ Call macro 'cr'.
cr \ Call macro 'cr'.
```
* `addr` ( id -- addr )  
If *id* is a variable, return address of variable *id*. 
* `ref` ( addr id -- )  
Declare a variable *id* that refers to address *addr*.  
If *addr* is 0, an error occurs.

### Memory
#### Dynamic memory allocation
* `alloc` ( u -- addr )  
Allocate *u* bytes and return a pointer to the allocated memory.
* `resize` ( u addr -- addr )  
Change the size of the memory block *addr* to *u* bytes. And return a pointer to the re-allocated memory.
* `free` ( addr -- )  
Free memory block *addr*.

#### Local stack memory allocation
* `allot` ( u -- addr )  
Reserve *u* bytes from memory pool, and return a pointer to the reserved memory.  
The reserved memory is released when the function call is finished.

#### Memory access
* `fetch` ( addr -- x )  
Fetch the value that stored at *addr*.
* `store` ( x addr -- )  
Store the value *x* into the memory cell *addr*.

### Type conversion
* `s>f` ( s -- f )
* `u>f` ( u -- f )
* `f>s` ( f -- s )
* `f>u` ( f -- u )

### I/O
* `geti` ( -- s )
* `getu` ( -- u )
* `getf` ( -- f )
* `getcs` ( -- ... u1 u)  
*u* is length of string, *u1* and the values after it are unicode values of string.
* `putc` ( u -- )
* `puti` ( s -- )
* `putu` ( u -- )
* `putf` ( f -- )
* `show` ( -- )  
Display the stack value.

## Literals
### Number literals
* Integers : `255`, `0255`, `0xff`, `0o377`, `0b11111111`
* Floating-point : `0.25`, `.25`, `00.250`, `0.25e0`, `2.5e-1`, `0.025e1`

### Character literals
* Unicode characters : `'あ'` -> `[ 12354 ]`
* Characters sequence : `'Hello'` -> `[ 111 108 108 101 72 ]`
* Characters sequence with length : `"안녕하세요!"` -> `[ 33 50836 49464 54616 45397 50504 6 ]`

#### Escape sequences
* `\a` -> 7
* `\b` -> 8
* `\e` -> 27
* `\f` -> 12
* `\n` -> 10
* `\r` -> 13
* `\t` -> 9
* `\v` -> 11
* `\\` -> 92
* `\'` -> 39
* `\"` -> 34
* `\nnn` -> The byte whose numerical value is given by nnn interpreted as an octal number.
* `\xhh` -> The byte whose numerical value is given by hh interpreted as a hexadecimal number.
* `\uhhhh` -> Unicode code point below 10000 hexadecimal.
* `\Uhhhhhhhh` -> Unicode code point where h is a hexadecimal digit.

## Identifiers
Control keywords, built-in operators, literals cannot become identifiers.
* Define identifiers  : `$main`, `$a`, `$Pos`, `$x`  
Use for define functions, macros, variables. These are unique unsigned integer value.
* Call identifiers : `main`, `a`, `Pos`  
Get variable values or call functions and macros.
* Get the address of a member variable of a struct : `Pos.x`  
See 'Struct' for detailed usage.

## Comments
* Line comments : `\this is a comment`, `\ this is a comment`, `2 5 + \this is a comment`
* Multiline / Stack-Effect comment : 
```
(this is a comment)
( this is
a comment )
$add func ( n1 n2 -- x ) \ this is a stack-effect comment
	+
end
```

## Struct
A struct is declared like this:
```
$Pos struct
	$x member
	$y member
end
```
Pos is the name of the struct, and it has two member variables, x and y.

To use a struct, allocate memory space equal to the size of the struct. The name of the structure return the size of the struct.
The members of the struct are used in the form `Pos.x`, It must be preceded by the address of a block of memory in that struct type. This will return the address of the member variable.
```
Pos cells allot $p1 set \ After allocating the Pos struct, it is assigned to the p1 variable.
50 p1 Pos.x store \ Storing the value of 50 in the member variable x of p1.
p1 Pos.x fetch puti \ Outputs the value of the member variable x of p1.
```