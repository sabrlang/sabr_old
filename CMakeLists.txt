cmake_minimum_required(
	VERSION 3.6
)

project(
	csabr
	LANGUAGES C
	VERSION 0.5
)

add_compile_options(
	-D_GNU_SOURCE
	-Wall
)

set(CMAKE_C_FLAGS_DEBUG "-O0 -g")
set(CMAKE_C_FLAGS_RELEASE "-O2")


include_directories(
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/include/cctl
	${PROJECT_SOURCE_DIR}/include/compiler
	${PROJECT_SOURCE_DIR}/include/interpreter
	${PROJECT_SOURCE_DIR}/src
	${PROJECT_SOURCE_DIR}/src/compiler
	${PROJECT_SOURCE_DIR}/src/interpreter
)

file(
	GLOB
	common_srcs
	"${PROJECT_SOURCE_DIR}/src/*.c"
	"${PROJECT_SOURCE_DIR}/include/*.h"
	"${PROJECT_SOURCE_DIR}/include/cctl/*.h"
)

file(
	GLOB
	inter_srcs
	"${PROJECT_SOURCE_DIR}/src/interpreter/*.c"
	"${PROJECT_SOURCE_DIR}/include/interpreter/*.h"
)

file(
	GLOB
	comp_srcs
	"${PROJECT_SOURCE_DIR}/src/compiler/*.c"
	"${PROJECT_SOURCE_DIR}/include/compiler/*.h"
)

add_executable(
	sabre
	${inter_srcs} ${common_srcs}
)

add_executable(
	sabrc
	${comp_srcs} ${common_srcs}
)

target_link_libraries(sabre m)

message("CMAKE_SYSTEM_NAME: " ${CMAKE_SYSTEM_NAME})
message("CMAKE_C_COMPILER_ID: " ${CMAKE_C_COMPILER_ID})
message("CMAKE_GENERATOR: " ${CMAKE_GENERATOR})
message("CMAKE_BINARY_DIR: " ${CMAKE_BINARY_DIR})

message("PROJECT_VERSION: " ${PROJECT_VERSION})