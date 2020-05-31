cmake_minimum_required( VERSION 3.8...3.15)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
project( Myproject VERSION 1.0
                   DESCRIPTION "Hello world"
                   LANGUAGES CXX)
enable_testing()

## OpenCL part
find_library(OpenCL_Loc OpenCL extern/OpenCL)

## end OpenCL part

## Benchmark Part
find_library(Benchmark_main_Loc benchmark_main extern/benchmark)
find_library(Benchmark_Loc benchmark extern/benchmark)


## end Benchmark part

## gogletest lib
find_library(Gtest_main_Loc gtest_main extern/googletest)


find_library(Gmock_main_Loc gmock_main extern/googletest)

## end google tests

set(CMAKE_CXX_STANDART 17)

add_subdirectory(matrix)
add_subdirectory(apps)
add_subdirectory(core)


option(PACKAGE_TESTS "Build the tests" ON)
if(PACKAGE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
