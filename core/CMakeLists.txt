add_library( HandMadeCPUMatrix STATIC  HandMadeCPUMatrix.cpp)
target_include_directories(HandMadeCPUMatrix PUBLIC ../core)



add_library( HandMadeOpenCLMatrix STATIC  HandMadeOpenCLMatrix.cpp)
target_include_directories(HandMadeOpenCLMatrix PUBLIC ../core)
target_include_directories(HandMadeOpenCLMatrix PUBLIC ../extern/OpenCL)

target_link_libraries(HandMadeOpenCLMatrix ${OpenCL_Loc})
