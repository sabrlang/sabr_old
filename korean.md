[![GitHub license](https://img.shields.io/github/license/sabrlang/sabr?style=for-the-badge)](https://github.com/sabrlang/sabr/blob/main/LICENSE)

[English](README.md)

# Sabr
Sabr 는 단순한 스택 기반 프로그래밍 언어입니다.

# 소스에서 빌드하기
```sh
git clone --depth=1 https://github.com/sabrlang/sabr.git
cd sabr
mkdir build
cd build
cmake ..
make
```

# 예제
## 산술 연산
```
2 5 + puti 2.5 3.14 f* putf
```
```
7 7.850000
```

## 매크로로 "Hello, world!" 출력하기
```
$puts macro
	loop dup 0 > while swap putc 1- end drop
end

"Hello, world!" puts
```
```
Hello, world!
```

## 간단한 반복문과 지역변수
```
$cr macro '\n' putc end
$draw func
	$count set
	0 $i set
	loop
		i count <
	while
		0 $j set
		loop
			j i <=
		while
			'*' putc
			j 1 + $j set
		end
		cr
		i 1 + $i set
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

# 실행 방법
## 바이트코드로 컴파일하기
```sh
$ sabrc {source file name} {output file name}
```
## 바이트코드 실행하기
```sh
$ sabre {bytecode file name}
```

# 명세
Sabr 프로그램은 UTF-8로 쓰여져야 합니다.
## 자료형
Sabr는 타입 검사가 없습니다. 모든 스택 값들은 8 바이트입니다.
* `x` : 셀 (아무 값)
* `n` : 정수
* `s` : 부호 있는 정수
* `u` : 부호 없는 정수
* `f` : 부동소수점 실수
* `b` : 불리언
* `addr` : 주소
* `id` : 식별자
## 제어문
### if 문
0은 거짓이고, 0이 아닌 값은 참입니다.

```
(flag) if
	(code)
end
```
`if`는 스택으로부터 플래그 값을 뽑습니다.  
만약 `(flag)`가 참이면, `(code)`가 실행됩니다.

```
(flag) if
	(code 1)
else
	(code 2)
end
```
만약 `(flag)`가 참이면, `(code 1)`이 실행됩니다.  
만약 `(flag)`가 거짓이면, `(code 2)`가 실행됩니다.

### loop 문
```
loop
	(code 1)
end
```

무한 루프입니다.

```
loop
	(code)
	(flag)
	while
end
```

우선 `(code)`가 실행됩니다. `while`은 스택으로부터 플래그 값을 뽑습니다.  
만약 `(flag)`가 참이면, 반복문이 재시작됩니다.

```
loop
	(code 1)
	(flag)
	while
	(code 2)
end
```

우선 `(code 1)`이 실행됩니다. `while`은 스택으로부터 플래그 값을 뽑습니다.  
만약 `(flag)`가 참이면, `(code 2)`가 실행되고 반복문이 재시작됩니다.

### switch 문
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

`switch`와 `case`는 스택으로부터 값을 뽑습니다.  
만약 `(value)`의 값이 `(case 1)`와 같다면, `(code 1)`이 실행됩니다.  
만약 `(value)`의 값이 `(case 3)`또는 `(case 4)`와 같다면, `(code 3)`가 실행됩니다.  
일치하는 값이 없다면, `(code 4)`가 실행됩니다.

#### if-elif-else 스타일
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

만약 `(flag 1)`이 참이면, `(code 1)`이 실행됩니다.  
만약 `(flag 3)` 또는 `(flag 4)`가 참이면, `(code 3)`이 실행됩니다.  
만약 모든 플래그가 거짓이라면, `(code 4)`가 실행됩니다.

### func 와 macro
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

### 제어 키워드
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

## 내장 연산자
### 정수 산술
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

### 정수 나눗셈
* `/` ( s1 s2 -- s )  
s = s1 ÷ s2
* `%` ( s1 s2 -- s )  
s = s1 mod s2
* `u/` ( u1 u2 -- u )  
u = u1 ÷ u2
* `u%` ( u1 u2 -- u )  
u = u1 mod u2

### 수 비교
* `=` ( x1 x2 -- b )
* `!=` ( x1 x2 -- b )

### 정수 비교
* `>` ( s1 s2 -- b )
* `>=` ( s1 s2 -- b )
* `<` ( s1 s2 -- b )
* `<=` ( s1 s2 -- b )
* `u>` ( u1 u2 -- b )
* `u>=` ( u1 u2 -- b )
* `u<` ( u1 u2 -- b )
* `u<=` ( u1 u2 -- b )

### 부동소수점 연산
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

### 비트 연산
* `&` ( x1 x2 -- x )
* `|` ( x1 x2 -- x )
* `^` ( x1 x2 -- x )
* `~` ( x1 -- x )
* `<<` ( x1 u -- x )
* `>>` ( x1 u -- x )

### 스택 조작
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

### 호출 가능한 값과 변수
* `set` ( x id -- )  
```
10 $var set \ 변수 'var'를 초기값 10으로 선언합니다.
20 $var set \ 변수 'var'의 값을 20으로 바꿉니다.
```
* `call` ( id -- ? )
```
10 $var set \ 변수 'var'를 초기값 10으로 선언합니다.
$var call \ 변수 'var'의 값을 얻습니다.
var \ 변수 'var'의 값을 얻습니다.

$cr macro 10 putc end \ 매크로 'cr'을 선언합니다.
$cr call \ 매크로 'cr'을 호출합니다.
cr \ 매크로 'cr'을 호출합니다.
```


### 메모리
#### 동적 메모리 할당
* `alloc` ( u -- addr )  
*u* 바이트를 동적 할당하고, 할당된 메모리의 주소를 반환합니다.
* `resize` ( u addr -- addr )  
메모리 블럭 *addr*의 크기를 *u* 바이트로 변경합니다. 그리고 재할당된 메모리의 포인터를 반환합니다.
* `free` ( addr -- )  
메모리 블럭 *addr*을 해제합니다.

#### 지역 스택 메모리 할당
* `allot` ( u -- addr )  
*u* 바이트를 메모리 풀에서 할당하고, 할당된 메모리의 주소를 반환합니다.  
할당된 메모리는 함수 호출이 끝나면 해제됩니다.

#### 메모리 접근
* `fetch` ( addr -- x )  
*addr*에 저장된 값을 인출합니다.
* `store` ( x addr -- )  
*x* 값을 *addr* 메모리 공간에 저장합니다.

### 타입 변환
* `s>f` ( s -- f )
* `u>f` ( u -- f )
* `f>s` ( f -- s )
* `f>u` ( f -- u )

### 입출력
* `geti` ( -- s )
* `getu` ( -- u )
* `getf` ( -- f )
* `getcs` ( -- ... u1 u)  
*u*는 문자열의 길이이고, *u1*과 그 뒤의 값들은 문자열의 유니코드 값입니다.
* `putc` ( u -- )
* `puti` ( s -- )
* `putu` ( u -- )
* `putf` ( f -- )
* `show` ( -- )  
스택 값들을 표시합니다.

## 리터럴
### 숫자 리터럴
* 정수 : `255`, `0255`, `0xff`, `0o377`, `0b11111111`
* 부동소수점 : `0.25`, `.25`, `00.250`, `0.25e0`, `2.5e-1`, `0.025e1`

### 문자 리터럴
* 유니코드 문자 : `'あ'` -> `[ 12354 ]`
* 문자열 : `'Hello'` -> `[ 111 108 108 101 72 ]`
* 문자열과 길이 : `"안녕하세요!"` -> `[ 33 50836 49464 54616 45397 50504 6 ]`

#### 이스케이프 시퀀스
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
* `\nnn` -> nnn으로 주어진 8진수 값입니다.
* `\xhh` -> hh로 주어진 16진수 값입니다.
* `\uhhhh` -> 10000 아래의 16진수 유니코드 값입니다.
* `\Uhhhhhhhh` -> 16진수 유니코드 값입니다.

## 식별자
컨트롤 키워드, 내장 연산자, 리터럴은 식별자가 될 수 없습니다.
* 식별자 정의하기 : `$main`, `$a`, `$Pos`, `$x`  
함수, 매크로, 구조체, 변수를 정의할 때 사용합니다. 저 값들은 고유한 부호 없는 정수형 값입니다.
* 식별자 호출하기 : `main`, `a`, `Pos`  
변수의 값을 얻거나 함수나 매크로를 호출합니다. 구조체의 경우 구조체의 크기를 얻습니다.
* 구조체 멤버의 주소 가져오기 : `Pos.x`  
자세한 사용은 '구조체' 참조.


## 주석
* 한 줄 주석 : `\이것은 주석입니다`, `\ 이것은 주석입니다`, `2 5 + \이것은 주석입니다`
* 여러 줄 주석 / 스택 효과 주석 : 
```
(이것은 주석입니다)
( 이것은
주석입니다 )
$add func ( n1 n2 -- x ) \ 이것은 스택 효과 주석입니다.
	+
end
```

## 구조체
구조체는 다음과 같이 선언합니다.
```
$Pos struct
	$x member
	$y member
end
```
Pos는 구조체의 이름이고, x와 y의 두 개의 멤버 변수를 가지고 있습니다.

구조체를 사용하려면 구조체의 크기 만큼의 메모리 블럭을 할당합니다. 구조체의 이름은 구조체의 크기를 반환합니다.
구조체의 멤버는 `Pos.x` 꼴로 호출하는데, 앞에 해당 구조체 형식의 메모리 블럭 주소가 있어야 합니다. 그러면 멤버 변수의 주소를 반환합니다.
```
Pos allot $p1 set \ Pos 구조체를 할당한 뒤 p1 변수에 대입.
50 p1 Pos.x store \ p1의 멤버 변수 x에 50의 값 저장.
p1 Pos.x fetch puti \ p1의 멤버 변수 x의 값을 출력.
```