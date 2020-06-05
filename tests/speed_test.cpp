
constexpr int sync_size = 10;

#ifdef benchmark

#include "benchmark/benchmark.h"

#include <iostream>

#include <algorithm>

#include <numeric>
#ifdef _WIN32
#pragma comment ( lib, "Shlwapi.lib" )
#endif

#define EIGEN_NO_CUDA
#include <iostream>
//#include <Eigen/Dence>// for now it not included ... should  check...

#include "CPU_diag.hpp"
#include "OCL_diag.hpp"
#include "CM_diag.hpp"
#define OCL

#if 0
static void nonStable_Eigen(benchmark::State& state)
{
using namespace Eigen;
	MatrixXd matrix(state.range(0), state.range(0));

	VectorXd tmp(state.range(0), 1);
	VectorXd current(state.range(0), 1);
	VectorXd old(state.range(0), 1);
	VectorXd next(state.range(0), 1);

	for (auto _ : state)
	{
		for (int i = 0; i < sync_size; i++)
		{
			tmp = matrix * current;
			next = tmp + old;
		}
	}
}
#endif

static void nonStable_CPU_diag(benchmark::State& state)
{
	int param = state.range(0);
	CPU_diag_matrix A(param, param);
	A.fill_diag_with_value(0, -1.0);
	A.fill_diag_with_value(1, -2.0);
	A.fill_diag_with_value(3, -3.0);
	A.fill_diag_with_value(param - 1, 1 - param);
	A.fill_diag_with_value(param - 2, 2 - param);
	CPU_vector tmp(param);
	CPU_vector current(param);
	CPU_vector old(param);
	CPU_vector next(param);
	for (auto _ : state)
	{
		for (int i = 0; i < sync_size; i++)
		{
			multiply(tmp, A, current);
			add(next, tmp, old);
		}
	}

}
static void nonStable_OCL_diag(benchmark::State& state)
{
	for (auto _ : state)
	{
	prepare::prepare_diag_ocl();
	int param = state.range(0);
	OCL_diag_matrix A(param, param);
	A.fill_diag_with_value(0, -1.0);
	A.fill_diag_with_value(1, -2.0);
	A.fill_diag_with_value(3, -3.0);
	A.fill_diag_with_value(param - 1, 1 - param);
	A.fill_diag_with_value(param - 2, 2 - param);
	OCL_vector tmp(param);
	OCL_vector current(param);
	OCL_vector old(param);
	OCL_vector next(param);
		A.prepare();
		tmp.prepare();
		current.prepare();
		old.prepare();
		next.prepare();
		for (int i = 0; i < sync_size; i++)
		{
			multiply(tmp, A, current);
			add(next, tmp, old);
		std::swap(next, current);
		std::swap(next, old);
		}
		tmp.getResult();
		current.getResult();
		old.getResult();
		next.getResult();
		A.getResult();
	prepare::exit_diag_ocl();
	}
}

static void nonStable_CM_diag(benchmark::State& state)
{
	for (auto _ : state)
	{
	prepare::prepare_diag_CM();
	int param = state.range(0);
	CM_diag_matrix A(param, param);
	A.fill_diag_with_value(0, -1.0);
	A.fill_diag_with_value(1, -2.0);
	A.fill_diag_with_value(3, -3.0);
	A.fill_diag_with_value(param - 1, 1 - param);
	A.fill_diag_with_value(param - 2, 2 - param);
	CM_vector tmp(param);
	CM_vector current(param);
	CM_vector old(param);
	CM_vector next(param);
		A.alloc_gpu_mem();
		tmp.alloc_gpu_mem();
        old.alloc_gpu_mem();
		current.alloc_gpu_mem();
		next.alloc_gpu_mem();
		A.copy_to_gpu();
		tmp.copy_to_gpu();
		current.copy_to_gpu();
		old.copy_to_gpu();
		next.copy_to_gpu();
		for (int i = 0; i < sync_size; i++)
		{
            multiply(tmp, A, current);
            add(next, tmp, old);
            std::swap(next, current);
            std::swap(next, old);
		}
		tmp.getResult();
		current.getResult();
		old.getResult();
		next.getResult();
		A.getResult();
	tmp.dealloc_gpu_mem();
	current.dealloc_gpu_mem();
	old.dealloc_gpu_mem();
	A.dealloc_gpu_mem();
	next.dealloc_gpu_mem();
	prepare::exit_diag_CM();
	}
}

constexpr int from = 32;
constexpr int to = 32;
constexpr int step = 32;
//BENCHMARK(nonStable_Eigen)->DenseRange(32, 1024, 32);
BENCHMARK(nonStable_CM_diag)->DenseRange(from, to, step);
BENCHMARK(nonStable_OCL_diag)->DenseRange(from, to, step);
BENCHMARK(nonStable_CPU_diag)->DenseRange(from, to, step);
BENCHMARK_MAIN();
#else
#include <iostream>
#include "CPU_diag.hpp"
#include "OCL_diag.hpp"
#include "CM_diag.hpp"
#include <chrono> 
using namespace std;



static void nonStable_OCL_diag(int param)
{
	prepare::prepare_diag_ocl();
	OCL_diag_matrix A(param, param);
	A.fill_diag_with_value(0, -1.0);
	A.fill_diag_with_value(1, -2.0);
	A.fill_diag_with_value(3, -3.0);
	A.fill_diag_with_value(param - 1, 1 - param);
	A.fill_diag_with_value(param - 2, 2 - param);
	OCL_vector tmp(param);
	OCL_vector current(param);
	OCL_vector old(param);
	OCL_vector next(param);

	// start
	auto start = chrono::high_resolution_clock::now();
		A.prepare();
		tmp.prepare();
		current.prepare();
		old.prepare();
		next.prepare();
		for (volatile int i = 0; i < sync_size; i++)
		{
			multiply(tmp, A, current);
			add(next, tmp, old);
			std::swap(next, current);
			std::swap(next, old);
		}
		tmp.getResult();
		current.getResult();
		old.getResult();
		next.getResult();
		A.getResult();
		// end

	auto end = chrono::high_resolution_clock::now();
	auto duration = chrono::duration_cast<chrono::nanoseconds>(end - start);
	cout << "nonStable_diag_OCL,"<< param<< ",";
	cout << duration.count() << "\n";
	prepare::exit_diag_ocl();
}

static void nonStable_CM_diag(int param)
{
	prepare::prepare_diag_CM();
	CM_diag_matrix A(param, param);
	A.fill_diag_with_value(0, -1.0);
	A.fill_diag_with_value(1, -2.0);
	A.fill_diag_with_value(3, -3.0);
	A.fill_diag_with_value(param - 1, 1 - param);
	A.fill_diag_with_value(param - 2, 2 - param);
	CM_vector tmp(param);
	CM_vector current(param);
	CM_vector old(param);
	CM_vector next(param);
    A.alloc_gpu_mem();
    tmp.alloc_gpu_mem();
    old.alloc_gpu_mem();
    current.alloc_gpu_mem();
    next.alloc_gpu_mem();
    // start
	auto start = chrono::high_resolution_clock::now();
        A.copy_to_gpu();
        tmp.copy_to_gpu();
        current.copy_to_gpu();
        old.copy_to_gpu();
        next.copy_to_gpu();
        for (volatile int i = 0; i < sync_size; i++)
        {
            multiply(tmp, A, current);
            add(next, tmp, old);
            std::swap(next, current);
            std::swap(next, old);
        }
        tmp.getResult();
        current.getResult();
        old.getResult();
        next.getResult();
        A.getResult();
	auto end = chrono::high_resolution_clock::now();
	auto duration = chrono::duration_cast<chrono::nanoseconds>(end - start);
	cout << "nonStable_diag_CM,"<< param<< ",";
	cout << duration.count() << "\n";
	tmp.dealloc_gpu_mem();
	current.dealloc_gpu_mem();
	old.dealloc_gpu_mem();
	A.dealloc_gpu_mem();
	next.dealloc_gpu_mem();
	prepare::exit_diag_CM();
}
int main()
{
	cout << "type, size, time\n";
	for (int i = 32; i <= 1024 * 2; i += 32)
	{

	nonStable_CM_diag(i);
	nonStable_OCL_diag(i);
	}
	return 0;
}
#endif
