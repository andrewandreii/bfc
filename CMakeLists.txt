cmake_minimum_required(VERSION 3.31)
project(bfc VERSION 0.0.1 LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB COMPILER src/emitters/*.c src/parsing/*.c src/utils/*.c)
add_library(bfc_compiler ${COMPILER})
target_include_directories(bfc_compiler PRIVATE include)

add_executable(bfc src/main.c)
target_include_directories(bfc PRIVATE include)
target_link_libraries(bfc PRIVATE bfc_compiler)
