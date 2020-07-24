cmake_minimum_required(VERSION 3.16)
project(Sudoku C)

set(CMAKE_C_STANDARD 11)

add_executable(Sudoku main.c semaphore.h cmake-build-debug/semaphore.c)