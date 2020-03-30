cmake_minimum_required(VERSION 3.15)
project(unnas_hw08)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(tests)

add_executable(unnas_hw08
        tests/01-hello.c
        tests/02-array-sum.c
        tests/03-list-sum.c
        tests/04-two-lists.c
        tests/05-many-sizes.c
        tests/Makefile
        tests/README.tests
        tests/tools.h
        hmalloc.c
        hmalloc.h
        Makefile
        test.pl)
