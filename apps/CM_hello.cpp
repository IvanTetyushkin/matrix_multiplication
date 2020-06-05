#include "CM_diag.hpp"

 // The only CM runtime header file that you need is cm_rt.h.
 // It includes all of the CM runtime.

constexpr int size = 16;
int main()
{
    prepare::prepare_diag_CM();
    CM_vector res(size);
    CM_vector lhs(size);
    CM_vector rhs(size);
    std::vector<float> to_put(size);
    std::iota(to_put.begin(), to_put.end(), 7.7);
    res.fill_with_value(0.0);
    lhs.fill_with_vector(to_put);

    std::iota(to_put.begin(), to_put.end(), 1.7);
    rhs.fill_with_vector(to_put);
    lhs.alloc_gpu_mem();
    rhs.alloc_gpu_mem();
    res.alloc_gpu_mem();
    res.copy_to_gpu();
    lhs.copy_to_gpu();
    rhs.copy_to_gpu();
    lhs.str_dump();
    rhs.str_dump();

    add(res, lhs, rhs);

    res.getResult();

    res.str_dump();

    CM_diag_matrix A(size, size);

    A.fill_diag_with_values(0, to_put);
    std::iota(to_put.begin(), to_put.end(), 3.2);
    A.fill_diag_with_values(3, to_put);
    A.raw_dump();
    A.pretty_dump();
    rhs.col_dump();
    A.alloc_gpu_mem();
    A.copy_to_gpu();
    res.fill_with_value(-1.7);
    res.copy_to_gpu();
    
    multiply(res, A, rhs);
    res.getResult();
    res.str_dump();
    res.dealloc_gpu_mem();
    lhs.dealloc_gpu_mem();
    rhs.dealloc_gpu_mem();
    A.dealloc_gpu_mem();


    prepare::exit_diag_CM();
}
