#!/bin/bash

echo "Writing LLVM IR to disk."
build/release/fantac test.c warn &> test.ll

echo "Compiling to object code."
llc -filetype=obj -o test.o test.ll

echo "Linking with test main."
clang test_main.c test.o -o test

echo "Running test."
./test
