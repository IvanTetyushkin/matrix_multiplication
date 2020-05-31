#include "HandMadeCPUMatrix.h"
#include "HandMadeOpenCLMatrix.h"

#include "benchmark/benchmark.h"

#include <iostream>

#include <algorithm>

#include <numeric>

#define EIGEN_NO_CUDA
#include <iostream>
#include <Eigen/Dense>

#include "CPU_diag.hpp"
#include "OCL_Diag.hpp"
#define OCL
#if 0
constexpr float b_2 = 1;


void generate_next_matrix(int col)
{
	int real_size = col * col;
	std::vector<float> to_return(real_size * real_size);

	for (int i{0}; i < real_size; i++)
		for (int j{ 0 }; j < real_size; j++)
		{
			if (i == j)// diag
			{
				to_return[i *real_size + j] = 1 + 4 * b_2;
			}
			else if ((i + 1) == (j))
			{
				to_return[i *real_size + j] = -b_2;
			}
			else if (i == (j + 1))
			{
				to_return[i * real_size + j] = -b_2;
			}
			else if ((i + col) == j)
			{
				to_return[i *real_size + j] = -b_2;
			}
			else if (i == (j + col))
			{
				to_return[i*real_size + j] = -b_2;
			}
			else to_return[i*real_size + j] = 0;
		}
	for (int i{ 0 }; i < real_size; i++)
	{
		for (int j{ 0 }; j < real_size; j++)
		{
			std::cout << to_return[i*real_size + j] << "\t";
		}
		std::cout << "\n";
	}
}

void generate_current_matrix(int col)
{
	int real_size = col * col;
	std::vector<float> to_return(real_size * real_size);

	for (int i{ 0 }; i < real_size; i++)
		for (int j{ 0 }; j < real_size; j++)
		{
			if (i == j)// diag
			{
				to_return[i *real_size + j] = 2 - 4 * b_2;
			}
			else if ((i + 1) == (j))
			{
				to_return[i *real_size + j] = b_2;
			}
			else if (i == (j + 1))
			{
				to_return[i * real_size + j] = b_2;
			}
			else if ((i + col) == j)
			{
				to_return[i *real_size + j] = b_2;
			}
			else if (i == (j + col))
			{
				to_return[i*real_size + j] = b_2;
			}
			else to_return[i*real_size + j] = 0;
		}
	for (int i{ 0 }; i < real_size; i++)
	{
		for (int j{ 0 }; j < real_size; j++)
		{
			std::cout << to_return[i*real_size + j] << "\t";
		}
		std::cout << "\n";
	}
}

constexpr int sync_size = 30;

 




auto MultiplyOCL = [](benchmark::State &state,
	int param)
{
	HandMadeOpenCLMatrix tmp(param, 1);

	HandMadeOpenCLMatrix matrix(param, param);
	HandMadeOpenCLMatrix current(param, 1);
	HandMadeOpenCLMatrix old(param, 1);
	HandMadeOpenCLMatrix next(param, 1);
	for (auto _ : state)
	{
		tmp.prepare();
		matrix.prepare();
		current.prepare();
		old.prepare();
		next.prepare();
		for (int i = 0; i < sync_size; i++)
		{
			simple_multiply(tmp, matrix, current);
			addition(next, tmp, old);
		}
		tmp.getResult();
		matrix.getResult();
		current.getResult();
		old.getResult();
		next.getResult();
		// sub(lhs, lhs, rhs);

	}
};

#endif

constexpr int sync_size = 32;

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

static void nonStable_CPU_diag(benchmark::State& state)
{
	int param = state.range(0);
	CPU_diag_matrix A(param, param);
	A.fill_diag_with_value(0, -1);
	A.fill_diag_with_value(1, -2);
	A.fill_diag_with_value(3, -3);
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
	prepare::prepare_diag_ocl();
	int param = state.range(0);
	OCL_diag_matrix A(param, param);
	A.fill_diag_with_value(0, -1);
	A.fill_diag_with_value(1, -2);
	A.fill_diag_with_value(3, -3);
	A.fill_diag_with_value(param - 1, 1 - param);
	A.fill_diag_with_value(param - 2, 2 - param);
	OCL_vector tmp(param);
	OCL_vector current(param);
	OCL_vector old(param);
	OCL_vector next(param);
	for (auto _ : state)
	{
		tmp.prepare();
		current.prepare();
		old.prepare();
		next.prepare();
		for (int i = 0; i < sync_size; i++)
		{
			multiply(tmp, A, current);
			add(next, tmp, old);
		}
		tmp.getResult();
		current.getResult();
		old.getResult();
		next.getResult();
	}
	prepare::exit_diag_ocl();
}

BENCHMARK(nonStable_Eigen)->DenseRange(32, 1024, 32);
BENCHMARK(nonStable_OCL_diag)->DenseRange(32, 1024, 32);
BENCHMARK(nonStable_CPU_diag)->DenseRange(32, 1024, 32);
BENCHMARK_MAIN();
