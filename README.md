[![GitHub license](https://img.shields.io/github/license/mhcoma/sabr?style=for-the-badge)](https://github.com/mhcoma/sabr/blob/main/LICENSE)

# Sabr
Sabr is simple stack-oriented programming language.

# Build from source
```
git clone --depth=1 https://github.com/mhcoma/sabr.git
cd sabr
mkdir build
cd build
cmake ..
make
```

# Example
## Arithmetic
```
2 5 + putd 2.5 3.14 f* putf
```
```
7 7.850000
```

## Print "Hello, world!" with macro
```
$putcs macro
    loop dup 0 != while putc end drop
end

'Hello, world!\0' putcs
```
```
Hello, world!
```

## Simple loop with local variables
```
$var= macro dup var = end
$cr macro '\n' putc end
$draw func
	$count var= 
	0 $i var=
	$j var
	loop
		i count <
	while
		0 $j =
		loop
			j i <=
		while
			'*' putc
			j 1 + $j =
		end
		cr
		i 1 + $i =
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
```
sabrc {source file name} {output file name}
```
## Interprete bytecode
```
sabre {bytecode file name}
```

# Specification
Sabr programs must be written in UTF-8.
## Stack-effect comments
**Not implemented**
* `x` : Cell (any)
* `n` : Integer
* `s` : Signed integer
* `u` : Unsigned integer
* `f` : Float-point
* `b` : Boolean
* `addr` : Address
* `id` : Identifier
## Control keywords
* `if`
* `else`
* `loop`
* `while`
* `break`
* `continue`
* `func`
* `return`
* `macro`
* `end`
## Built-in operators
### Integer arithmetic
These also work for unsigned integers.
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
* `==` ( x1 x2 -- b )
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
* `f+` ( f1 f2 -- f ) : f = f1 + f2
* `f-` ( f1 f2 -- f ) : f = f1 - f2
* `f*` ( f1 f2 -- f ) : f = f1 × f2
* `f/` ( f1 f2 -- f ) : f = f1 ÷ f2
* `f%` ( f1 f2 -- f ) : f = f1 mod f2
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
### Variable
* `var` ( id -- )
* `=` ( x id -- )
### Dynamic allocation
* `alloc` ( u -- addr )
* `resize` ( u addr -- addr )
* `free` ( addr -- )
* `@` ( addr -- x )
* `#` ( x addr -- )
### I/O
* `getd` ( -- s )
* `getu` ( -- u )
* `getf` ( -- f )
* `getcs` ( -- u ... )
* `putc` ( u -- )
* `putd` ( s -- )
* `putu` ( u -- )
* `putf` ( f -- )
* `show` ( -- )

## Literals
### Number literals
* Integers : `255`, `0255`, `0xff`, `0o377`, `0b11111111`
* Floating-point : `0.25`, `.25`, `00.250`, `0.25e0`, `2.5e-1`, `0.025e1`
### Character literals
* Unicode characters : `'가'` -> `[ 44032 ]`
* Characters sequence : `'Hello\0'` -> `[ 0, 111, 108, 108, 72 ]`
### Escape sequences
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
* Define identifiers  : `$main`, `$a`  
Use for define functions, macros, variables. These are unsigned integer value.
* Call identifiers : `main`, `a`  
Get variable values or call functions and macros.
## Comments
* Line comments : `\this is a comment`, `\ this is a comment`, `2 5 + \this is a comment`

