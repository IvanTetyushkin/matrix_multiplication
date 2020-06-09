# new ones, here...
# ocl
add_library( OCL_diag STATIC OCL_diag.cpp)
target_include_directories(OCL_diag PUBLIC ../core)
target_include_directories(OCL_diag PUBLIC ../extern/OpenCL)
target_link_libraries(OCL_diag OpenCL::OpenCL)


# CM
if(ADD_CM)
	add_library( CM_diag STATIC CM_diag.cpp)
	target_compile_definitions(CM_diag PRIVATE  ADD_CM)
	target_include_directories(CM_diag PUBLIC ../core)
	target_include_directories(CM_diag PUBLIC ../extern/CM)
	target_link_libraries(CM_diag ${libcm_Loc})
	target_link_libraries(CM_diag ${cmrt_Loc})
endif()
# cpu single-thread, omp
add_library(CPU_diag STATIC CPU_diag.cpp)
target_link_libraries(CPU_diag PUBLIC OpenMP::OpenMP_CXX)
target_include_directories(CPU_diag PUBLIC ../core)
