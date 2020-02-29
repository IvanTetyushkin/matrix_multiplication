
add_executable( hello hello.cpp)




target_link_libraries(hello PUBLIC SimpleMatrix)
target_link_libraries(hello PUBLIC CPUMatrix)
 
