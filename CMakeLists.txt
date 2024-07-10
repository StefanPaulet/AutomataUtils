cmake_minimum_required(VERSION 3.15)
project(AutomataUtils)

set(CMAKE_CXX_STANDARD 20)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(COMPILER_SPECIFIC_COVERAGE_FLAGS "-fcoverage-mapping -fprofile-instr-generate -O0 -g")
else()
    set(COMPILER_SPECIFIC_COVERAGE_FLAGS "")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILER_SPECIFIC_COVERAGE_FLAGS}")

add_executable(main source/target/main.cpp
        source/regex/Regex.cpp
        source/alphabet/Alphabet.cpp
        source/operator/Operator.hpp)
target_include_directories(main PUBLIC source)

add_subdirectory(test/unittest)

enable_testing()