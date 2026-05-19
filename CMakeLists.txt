cmake_minimum_required(VERSION 3.14)
project(memory_allocator, C)
include_directories(include)
add_executable(test src/mem_allocator.c src/testdriver.c)
