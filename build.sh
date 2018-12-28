#!/bin/bash

echo "Writing LLVM IR to disk."
./fantac test.c warn &> test.ll

echo "Compiling to object code."
llc -filetype=obj -o test.o test.ll

echo "Linking with test main."
clang++ test_main.cpp test.o -o test

echo "Running test."
./test
