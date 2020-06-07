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

using namespace std;

float f(float x, float y)
{
	return x + y;
}

constexpr float dx = 0.1;
constexpr float dy = 0.1;
// for Laplas eq
//ill in for the dirichlet problem
template<typename vector>
void fill_vector(vector& v, int num)
{
		for (int i = 0; i < num; i++)
		{
			v(0, i, num) = f(0 * dx, i*dy);// up 
			v(num - 1, i, num) = f((num - 1)*dx , i *dy);// botton
			v(i, 0, num) = f(i*dx, 0*dy);//right part
			v(i, num - 1, num) = f(i*dx, (num -1)*dy);// right part
		}

}

template<typename matrix>
void fill_matrix(matrix& m, int num)
{
    std::vector<float> diag_0(num * num);
    std::vector<float> diag_1(num * num);
    vector<float> diag_col(num * num);
    vector<float> diag_last(num * num);
    vector<float> diag_2_col(num * num);
    for (int i = 0; i < num * num; i++)
    {
        int ost = num - 2;
        if (i < num)
        {
            diag_0[i] = 1;
        }
        if (i % num < ost &&
            (i >= num
                && i < num * (num - 1))
            )
        {
            diag_1[i] = 0.25;
        }
        if ((i + num - 1) % num < ost &&
            (i + num - 1) < num * (num - 1))
        {
            diag_col[i] = 0.25;
        }
        if (((i + num - 2) % num < ost)
            && i > num&& i < num * (num - 1))
            diag_last[i] = 0.25;
        if ((i > num * 2) &&
            (i + num - 1) % num < ost
            )
            diag_2_col[i] = 0.25;


        if (i >= num * (num - 1))
        {
            diag_0[i] = 1;

        }
        if (i % num == 0)
        {
            diag_0[i] = 1;
        }
        if (i % num == (num - 1))
        {
            diag_0[i] = 1;
        }


    }
    m.fill_diag_with_values(0, diag_0);
    m.fill_diag_with_values(1, diag_1);
    m.fill_diag_with_values(num, diag_col);
    m.fill_diag_with_values(num * num - 1, diag_last);
    m.fill_diag_with_values(num * num - num, diag_2_col);

}
// using Jacobi

constexpr float stop_error = 0.001;
static void CPU_diag(benchmark::State& state)
{
	int num = state.range(0);// 0, 16, 32, ....
	int line_num = num * num;
	CPU_diag_matrix A(line_num, line_num);
	CPU_vector field(line_num);
	CPU_vector next_field(line_num);
	fill_matrix(A, num);
	fill_vector(field, num);

	next_field.fill_with_value(-1.0f);
	int iterations = 0;

	
    int need_next = 1;
	for (auto _ : state)
	{
		while (need_next)
		{
			iterations++;
			multiply(next_field, A, field);
			need_next = 0;
			std::swap(next_field, field);
			check_norm(next_field, field, need_next, stop_error);
		}
	}
	state.counters["iterations"] = iterations;
}
static void OCL_diag(benchmark::State& state)
{
	if (prepare::prepare_diag_ocl() != 0)
		exit(-10);
	int num = state.range(0);// 0, 16, 32, ....
	int line_num = num * num;
	OCL_diag_matrix A(line_num, line_num);
	OCL_vector field(line_num);
	OCL_vector next_field(line_num);
	fill_matrix(A, num);
	fill_vector(field, num);

	next_field.fill_with_value(-1.0f);

	OCL_vector need_next(1);
	need_next(0) = 1;

	int iterations = 0;
	for (auto _ : state)
	{
        A.prepare();
        field.prepare();
        next_field.prepare();
		while (need_next(0))
		{
			iterations++;
			multiply(next_field, A, field);
			need_next(0) = 0;
			std::swap(next_field, field);
			need_next.prepare();
			check_norm(next_field, field, need_next, stop_error);
			need_next.getResult();
		}
		A.getResult();
		field.getResult();
		next_field.getResult();
	}
	state.counters["iterations"] = iterations;

	prepare::exit_diag_ocl();
}
static void CM_diag(benchmark::State& state)
{
	if (prepare::prepare_diag_CM() != 0)
		exit(-10);
	int num = state.range(0);// 0, 16, 32, ....
	int line_num = num * num;
	CM_diag_matrix A(line_num, line_num);
	CM_vector field(line_num);
	CM_vector next_field(line_num);
	fill_matrix(A, num);
	fill_vector(field, num);

	next_field.fill_with_value(-1.0f);

	CM_vector need_next(1);
	A.alloc_gpu_mem();
	field.alloc_gpu_mem();
	next_field.alloc_gpu_mem();
	need_next.alloc_gpu_mem();

	need_next(0) = 1;
	int iterations = 0;
	for (auto _ : state)
	{
        A.copy_to_gpu();
        field.copy_to_gpu();
        next_field.copy_to_gpu();// not need in fact
		while (need_next(0))
		{
			iterations++;
			multiply(next_field, A, field);
			need_next(0) = 0;
			std::swap(next_field, field);
			need_next.copy_to_gpu();
			check_norm(next_field, field, need_next, stop_error);
			need_next.getResult();
		}
		A.getResult();
		field.getResult();
		next_field.getResult();
	}
	state.counters["iterations"] = iterations;

	prepare::exit_diag_CM();
}
static void CPU_diag_calculations_only(benchmark::State& state)
{
	int num = state.range(0);// 0, 16, 32, ....
	int line_num = num * num;
	CPU_diag_matrix A(line_num, line_num);
	CPU_vector field(line_num);
	CPU_vector next_field(line_num);
	fill_matrix(A, num);
	fill_vector(field, num);

	next_field.fill_with_value(-1.0f);
	int iterations = 0;

	
    int need_next = 1;
	for (auto _ : state)
	{
		while (need_next)
		{
			iterations++;
			multiply(next_field, A, field);
			need_next = 0;
			std::swap(next_field, field);
			check_norm(next_field, field, need_next, stop_error);
		}
	}
	state.counters["iterations"] = iterations;
}
static void OCL_diag_calculations_only(benchmark::State& state)
{
	if (prepare::prepare_diag_ocl() != 0)
		exit(-10);
	int num = state.range(0);// 0, 16, 32, ....
	int line_num = num * num;
	OCL_diag_matrix A(line_num, line_num);
	OCL_vector field(line_num);
	OCL_vector next_field(line_num);
	fill_matrix(A, num);
	fill_vector(field, num);

	next_field.fill_with_value(-1.0f);

	OCL_vector need_next(1);
	need_next(0) = 1;

	int iterations = 0;
    A.prepare();
    field.prepare();
    next_field.prepare();
	for (auto _ : state)
	{
		while (need_next(0))
		{
			iterations++;
			multiply(next_field, A, field);
			need_next(0) = 0;
			std::swap(next_field, field);
			need_next.prepare();
			check_norm(next_field, field, need_next, stop_error);
			need_next.getResult();
		}
	}
	state.counters["iterations"] = iterations;
    A.getResult();
    field.getResult();
    next_field.getResult();

	prepare::exit_diag_ocl();
}
static void CM_diag_calculations_only(benchmark::State& state)
{
	if (prepare::prepare_diag_CM() != 0)
		exit(-10);
	int num = state.range(0);// 0, 16, 32, ....
	int line_num = num * num;
	CM_diag_matrix A(line_num, line_num);
	CM_vector field(line_num);
	CM_vector next_field(line_num);
	fill_matrix(A, num);
	fill_vector(field, num);

	next_field.fill_with_value(-1.0f);

	CM_vector need_next(1);
	A.alloc_gpu_mem();
	field.alloc_gpu_mem();
	next_field.alloc_gpu_mem();
	need_next.alloc_gpu_mem();

	need_next(0) = 1;
	int iterations = 0;
    A.copy_to_gpu();
    field.copy_to_gpu();
    next_field.copy_to_gpu();// not need in fact

	for (auto _ : state)
	{
		while (need_next(0))
		{
			iterations++;
			multiply(next_field, A, field);
			need_next(0) = 0;
			std::swap(next_field, field);
			need_next.copy_to_gpu();
			check_norm(next_field, field, need_next, stop_error);
			need_next.getResult();
		}
	}
    A.getResult();
    field.getResult();
    next_field.getResult();
	state.counters["iterations"] = iterations;

	prepare::exit_diag_CM();
}




constexpr int from = 16;
constexpr int to = 80;
constexpr int step = 16;
constexpr int from_size = 0;
constexpr int to_size = 256;
constexpr int step_size = 16;
//BENCHMARK(nonStable_Eigen)->DenseRange(32, 1024, 32);
#ifdef speed
BENCHMARK(nonStable_CM_diag)->DenseRange(from, to, step);
BENCHMARK(nonStable_OCL_diag)->DenseRange(from, to, step);
BENCHMARK(nonStable_CPU_diag)->DenseRange(from, to, step);
#else

BENCHMARK(CM_diag)->DenseRange(from, to, step);
BENCHMARK(OCL_diag)->DenseRange(from, to, step);
BENCHMARK(CPU_diag)->DenseRange(from, to, step);
BENCHMARK(CM_diag_calculations_only)->DenseRange(from, to, step);
BENCHMARK(OCL_diag_calculations_only)->DenseRange(from, to, step);
BENCHMARK(CPU_diag_calculations_only)->DenseRange(from, to, step);
#endif
BENCHMARK_MAIN();
