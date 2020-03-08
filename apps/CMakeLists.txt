add_executable( hello hello.cpp)
target_link_libraries(hello PUBLIC HandMadeCPUMatrix)

add_executable( OpenCLSimple OpenCLSimple.cpp)
target_link_libraries(OpenCLSimple PUBLIC HandMadeOpenCLMatrix)


 
