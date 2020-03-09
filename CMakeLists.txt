cmake_minimum_required( VERSION 3.8...3.15)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
project( Myproject VERSION 1.0
                   DESCRIPTION "Hello world"
                   LANGUAGES CXX)
enable_testing()
find_library(OpenCL_Loc OpenCL extern/OpenCL)

set(CMAKE_CXX_STANDART 17)

add_subdirectory(matrix)
add_subdirectory(apps)
add_subdirectory(core)


option(PACKAGE_TESTS "Build the tests" ON)
    if(PACKAGE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
