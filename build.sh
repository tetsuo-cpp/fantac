#!/bin/bash

# Write LLVM IR to file.
./fantac test.c warn &> test.ll

# Compile to object code.
llc -filetype=obj -o test.o test.ll

# Link with test main.
clang++ test_main.cpp test.o -o test

# Run test.
./test
