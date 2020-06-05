
macro(package_add_test_speed TESTNAME)
    add_executable(${TESTNAME} ${ARGN})
    target_link_libraries(${TESTNAME} PRIVATE ${Benchmark_main_Loc} )
    target_link_libraries(${TESTNAME} PRIVATE ${Benchmark_Loc} )
    #what is it
    #target_link_libraries(${TESTNAME} PRIVATE ${External_loc})
    target_include_directories(${TESTNAME} PUBLIC ../core)
    target_include_directories(${TESTNAME} PUBLIC ../extern/benchmark)
    add_test(NAME ${TESTNAME} COMMAND ${TESTNAME})
endmacro()

macro(package_add_test_correctness TESTNAME)
    add_executable(${TESTNAME} ${ARGN})
    target_link_libraries(${TESTNAME} PRIVATE ${Gtest_main_Loc})
    target_link_libraries(${TESTNAME} PRIVATE ${Gmock_main_Loc})
    
    target_include_directories(${TESTNAME} PUBLIC ../core)
    target_include_directories(${TESTNAME} PUBLIC ../extern/googletest)
    add_test(NAME ${TESTNAME} COMMAND ${TESTNAME})
endmacro()

package_add_test_speed(speed_test speed_test.cpp)
package_add_test_correctness(correctness_test correctness_test.cpp)

target_include_directories(speed_test PUBLIC ../extern/eigen)
target_include_directories(correctness_test PUBLIC ../extern/eigen)


target_link_libraries(speed_test PUBLIC CPU_diag)
target_link_libraries(speed_test PUBLIC OCL_diag)
target_link_libraries(speed_test PUBLIC CM_diag)


