add_library( SimpleMatrix STATIC SimpleMatrix.cpp)

add_library( CPUMatrix STATIC CPUMatrix.cpp)
target_link_libraries(CPUMatrix PUBLIC SimpleMatrix)

target_include_directories(SimpleMatrix PUBLIC ../core)
target_include_directories(CPUMatrix PUBLIC ../core)
