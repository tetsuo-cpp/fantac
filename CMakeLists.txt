cmake_minimum_required(VERSION 3.9)
project(fantac)

# Compiler flags.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

# Build compiler binary.
set(
  FANTAC_FILES
  lib/Compiler/FantaC.cpp
  src/main.cpp
  )

add_executable(fantac ${FANTAC_FILES})

target_include_directories(fantac PRIVATE lib)
