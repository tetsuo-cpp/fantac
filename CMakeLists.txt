cmake_minimum_required(VERSION 3.9)
project(fantac)

# Compiler flags.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

# Setup LLVM.
find_package(LLVM REQUIRED CONFIG)
include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Dynamically link to LLVM.
set(LLVM_LINK_LLVM_DYLIB ON)
set(llvm_libs LLVM)

# Setup fmt.
add_subdirectory(
  "${PROJECT_SOURCE_DIR}/external/fmt"
  "external/fmt"
  )

# Build compiler binary.
set(
  FANTAC_FILES
  lib/CodeGen/IRGenerator.cpp
  lib/Compiler/FantaC.cpp
  lib/Parse/Lexer.cpp
  lib/Parse/Parser.cpp
  lib/Parse/Token.cpp
  src/main.cpp
  )

add_executable(fantac ${FANTAC_FILES})

target_link_libraries(fantac ${llvm_libs} fmt)
target_include_directories(fantac PRIVATE lib)

if (DEFINED SANITIZER_TYPE)
  if (${SANITIZER_TYPE} STREQUAL "ASan")
    target_link_libraries(fantac -fsanitize=address)
  elseif (${SANITIZER_TYPE} STREQUAL "UBSan")
    target_link_libraries(fantac -fsanitize=undefined)
  endif()
endif()
