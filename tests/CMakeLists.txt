add_subdirectory("${PROJECT_SOURCE_DIR}/extern/googletest" "extern/googletest")

add_subdirectory("${PROJECT_SOURCE_DIR}/extern/benchmark" "extern/benchmark")

mark_as_advanced(
BUILD_GMOCK BUILD_GTEST BUILD_SHARED_LIBS
gmock_build_tests gtest_build_samples gtest_build_tests
gtest_disable_pthreads gtest_force_shared_crt gtest_hide_internal_symbols
)
mark_as_advanced(
benchmark_main benchmark_main output_test_helper output_test_helper
complexity_test  cxx03_test user_counters_thousands_test  commandlineflags_gtest
commandlineflags_gtest benchmark_name_gtest output_test_helper  cxx03_test
user_counters_thousands_test memory_manager_test display_aggregates_only_test
display_aggregates_only_test reporter_output_test user_counters_test
)

set_target_properties(benchmark  PROPERTIES FOLDER extern) 

set_target_properties(gtest PROPERTIES FOLDER extern)
set_target_properties(gtest_main PROPERTIES FOLDER extern)
set_target_properties(gmock PROPERTIES FOLDER extern)
set_target_properties(gmock_main PROPERTIES FOLDER extern)

macro(package_add_test TESTNAME)
    add_executable(${TESTNAME} ${ARGN})
    #target_link_libraries(${TESTNAME} PRIVATE gtest gmock gtest_main)
    target_link_libraries(${TESTNAME} PRIVATE benchmark )
    add_test(NAME ${TESTNAME} COMMAND ${TESTNAME})
#    set_target_properties(${TESTNAME} PROPERTIES FOLDER tests)
endmacro()

package_add_test(speed_test test_Matrix.cpp)


target_include_directories(speed_test PUBLIC ../core)
target_link_libraries(speed_test PUBLIC CPUMatrix)


