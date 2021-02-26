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
* `+`
* `-`
* `*`
* `0-`
* `1+`
* `1-`
### Integer division
* `/`
* `%`
* `u/`
* `u%`
### Integer comparison
* `==`
* `!=`
* `>`
* `>=`
* `<`
* `<=`
* `u>`
* `u>=`
* `u<`
* `u<=`
### Floating-point
* `f+`
* `f-`
* `f*`
* `f/`
* `f%`
* `f==`
* `f!=`
* `f>`
* `f>=`
* `f<`
* `f<=`
### Bitwise operation
* `&`
* `|`
* `^`
* `~`
* `<<`
* `>>`
### Stack manipulation
* `drop`
* `nip`
* `dup`
* `over`
* `tuck`
* `swap`
* `rot`
* `2drop`
* `2nip`
* `2dup`
* `2over`
* `2tuck`
* `2swap`
* `2rot`
### Variable
* `var`
* `=`
### Dynamic allocation
* `alloc`
* `resize`
* `free`
* `@`
* `#`
### I/O
* `putc`
* `putd`
* `putu`
* `putf`
* `show`

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
* Identifier name  : `$main`, `$a`  
Use for define functions, macros, variables. These are unsigned integer value.
* Call identifiers : `main`, `a`  
Get variable values or call functions and macros.
## Comments
* Line comments : `\this is a comment`, `\ this is a comment`, `2 5 + \this is a comment`

