add_library( HandMadeCPUMatrix STATIC  HandMadeCPUMatrix.cpp)
target_include_directories(HandMadeCPUMatrix PUBLIC ../core)



add_library( HandMadeOpenCLMatrix STATIC  HandMadeOpenCLMatrix.cpp)
target_include_directories(HandMadeOpenCLMatrix PUBLIC ../core)
target_include_directories(HandMadeOpenCLMatrix PUBLIC ../extern/OpenCL)

target_link_libraries(HandMadeOpenCLMatrix ${OpenCL_Loc})

# new ones, here...
# ocl
add_library( OCL_diag STATIC OCL_diag.cpp)
target_include_directories(OCL_diag PUBLIC ../core)
target_include_directories(OCL_diag PUBLIC ../extern/OpenCL)
target_link_libraries(OCL_diag ${OpenCL_Loc})


# CM
add_library( CM_diag STATIC CM_diag.cpp)
target_include_directories(CM_diag PUBLIC ../core)
target_include_directories(CM_diag PUBLIC ../extern/CM)
# not now
# target_link_libraries(CM_diag ${CM_Loc})

# cpu single-thread
add_library(CPU_diag STATIC CPU_diag.cpp)
target_include_directories(CPU_diag PUBLIC ../core)
