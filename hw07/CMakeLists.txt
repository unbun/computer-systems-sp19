cmake_minimum_required(VERSION 3.15)
project(hw07)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(tools)

add_executable(hw07
        tools/check-sorted.c
        tools/gen-input.c
        tools/Makefile
        tools/print-data.c
        barrier.c
        barrier.h
        float_vec.c
        float_vec.h
        Makefile
        ssort.c
        test.pl
        utils.c
        utils.h)
