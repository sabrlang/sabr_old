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
+ Sabr programs must be written in UTF-8.

## Tokens
### Control keywords
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
### Literals
#### Number literals
* Integers : `255` `0255` `0xff` `0o377` `0b11111111`
* Floating-point : `0.25` `.25` `00.250` `0.25e0` `2.5e-1` `0.025e1`
#### Character literals

### Identifiers
