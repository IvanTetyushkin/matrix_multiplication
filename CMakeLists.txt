cmake_minimum_required( VERSION 3.8...3.15)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
project( Myproject VERSION 1.0
                   DESCRIPTION "Hello world"
                   LANGUAGES CXX)
enable_testing()

## CM libs

find_library(cmrt_Loc igfxcmrt64 extern/CM)
find_library(libcm_Loc libcm extern/CM)


## Benchmark Part
find_library(Benchmark_main_Loc benchmark_main extern/benchmark)
find_library(Benchmark_Loc benchmark extern/benchmark)


## end Benchmark part


# openmp part
find_package(OpenMP)
if(NOT OpenMP_CXX_FOUND)     
    message( SEND_ERROR "No openMP found" )
endif()

# OpenCL part
find_package(OpenCL REQUIRED)


set(CMAKE_CXX_STANDART 17)

add_subdirectory(apps)
add_subdirectory(core)


option(PACKAGE_TESTS "Build the tests" ON)
if(PACKAGE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
