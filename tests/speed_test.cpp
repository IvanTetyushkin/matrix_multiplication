#include "HandMadeCPUMatrix.h"
#include "HandMadeOpenCLMatrix.h"

#include "benchmark/benchmark.h"

#include <iostream>

#include <algorithm>

#include <numeric>

#define EIGEN_NO_CUDA
#include <iostream>
#include <Eigen/Dense>

#define OCL

constexpr int sync_size = 30;

 
using namespace Eigen;
// part benchmarks
auto MultiplyCPU =  []( benchmark::State &state, 
int param)
{
	HandMadeCPUMatrix tmp(param, 1);

	HandMadeCPUMatrix matrix(param, param);
	HandMadeCPUMatrix current(param, 1);
	HandMadeCPUMatrix old(param, 1);
	HandMadeCPUMatrix next(param, 1);
	for (auto _ : state)
	{
		for (int i = 0; i < sync_size; i++)
		{	
			simple_multiply(tmp, matrix, current);
			addition(next, tmp, old);
		}
           // sub(lhs, lhs, rhs);
            
    }
};

auto TestCPUBlock = [](benchmark::State &state,
	int param)
{
	HandMadeCPUMatrix tmp(param, 1);

	HandMadeCPUMatrix matrix(param, param);
	HandMadeCPUMatrix current(param, 1);
	HandMadeCPUMatrix old(param, 1);
	HandMadeCPUMatrix next(param, 1);
	for (auto _ : state)
	{
		for (int i = 0; i < sync_size; i++)
		{
			block_multiply(tmp, matrix, current, 32);
			addition(next, tmp, old);
		}
		// sub(lhs, lhs, rhs);

	}
};

auto TestEigen = [](benchmark::State &state,
	int param)
{
	MatrixXd matrix = MatrixXd::Random(param, param);
	VectorXd tmp(param, 1);
	VectorXd current(param, 1);
	VectorXd old(param, 1);
	VectorXd next(param, 1);

	for (auto _ : state)
	{
		for (int i = 0; i < sync_size; i++)
		{
			tmp = matrix * current;
			next = tmp + old;
		}
	}
};


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

void RegisterBenchmark( const std::vector<int>& values){
    for( const auto& v : values){
        (benchmark::RegisterBenchmark((std::string("HandMadeCPUMatrix")+":"+
        std::to_string(v)).c_str(),
        MultiplyCPU, v));
#ifdef OCL
		(benchmark::RegisterBenchmark((std::string("HandMadeOpenCLMatrix") + ":" +
			std::to_string(v)).c_str(),
			MultiplyOCL, v));
#endif
		(benchmark::RegisterBenchmark((std::string("Eigen") + ":" +
			std::to_string(v)).c_str(),
			TestEigen, v));
#if 0
		(benchmark::RegisterBenchmark((std::string("HandMadeCPUMatrixBlock") + ":" +
			std::to_string(v)).c_str(),
			TestCPUBlock, v));
#endif
    }    
}
// end benchmarks

std::vector<int> generate_speed_param()
{
	constexpr int diff = 32; // the size is about 32, 64, 96, ...
	constexpr int max_size = 32*50;// last size is about 1024*1024
	constexpr int count = max_size / 32;
	std::vector<int> result(count);
	std::iota(std::begin(result), std::end(result), 1);
	std::for_each(std::begin(result) , std::end(result), [diff](int &n) { n*= diff; });
	return result;


}


int main(int argc, char *argv[])
{
#ifdef OCL
	if (prepare::prepare_OpenCL() != 0)
		return -1;// compile all needed...
	
#endif
	auto param = generate_speed_param();
#ifdef Dump
		
		std::for_each(std::begin(param), std::end(param), [](int size) {
			std::cout << size << std::endl;
		});
#endif

        benchmark::Initialize(&argc, argv);
        
        RegisterBenchmark(param);

        benchmark::RunSpecifiedBenchmarks();
#ifdef OCL
		prepare::exit_OpenCL();
#endif
}
