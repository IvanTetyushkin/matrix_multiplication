add_executable( CPU_hello CPU_hello.cpp)
target_link_libraries(CPU_hello PUBLIC CPU_diag)

add_executable( OCL_hello OCL_hello.cpp)
target_link_libraries(OCL_hello PUBLIC OCL_diag)


target_include_directories(OCL_hello PUBLIC ../core)
 

add_executable( CM_hello CM_hello.cpp)
target_link_libraries(CM_hello PUBLIC CM_diag)
target_include_directories(CM_hello PUBLIC ../core)
