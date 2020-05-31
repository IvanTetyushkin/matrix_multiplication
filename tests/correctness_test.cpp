


#include "HandMadeCPUMatrix.h"
#include "HandMadeOpenCLMatrix.h"
#include <gtest/gtest.h>

#include <iostream>


using arg_list = std::tuple<std::pair<int, int>, std::pair<int, int>, std::pair<int, int>>;

template<class MatrixA, class MatrixB>
void fill_the_same_random(MatrixA& lhs, MatrixB& rhs, int str, int col)
{
	for (int i = 0; i < str; ++i)
	{
		for (int j = 0; j < col; ++j)
		{
			lhs(i, j) = rhs(i, j) = std::rand() % 100;
		}
	}
}

template<class MatrixA, class MatrixB>
bool check_results(const MatrixA& lhs, const MatrixB& rhs, int str, int col)
{
	bool equal = 1;
	for (int i = 0; i < str; i++)
	{
		for (int j = 0; j < col; j++)
		{
			equal &= (lhs(i, j) == rhs(i, j));
			if (equal == 0)
				return equal;
		}
	}
	return equal;
}


//for gtests
class MyFixture : public ::testing::Test {};

class CheckCorrectness : public MyFixture
{
private:
	int res_str;
	int res_col;
	int lhs_str;
	int lhs_col;
	int rhs_str;
	int rhs_col;
public:
	explicit CheckCorrectness(std::pair<int, int> res_size,
									std::pair<int, int> lhs_size,
									std::pair<int, int> rhs_size) :
		res_str(res_size.first),
		res_col(res_size.second),
		lhs_str(lhs_size.first),
		lhs_col(lhs_size.second),
		rhs_str(rhs_size.first),
		rhs_col(rhs_size.second){}
	void TestBody() override
	{
		HandMadeCPUMatrix lhs_cpu(lhs_str, lhs_col);
		HandMadeCPUMatrix rhs_cpu(rhs_str, rhs_col);
		HandMadeOpenCLMatrix lhs_ocl(lhs_str, lhs_col);
		HandMadeOpenCLMatrix rhs_ocl(rhs_str, rhs_col);

		HandMadeCPUMatrix res_cpu(res_str, res_col);
		HandMadeOpenCLMatrix res_ocl(res_str, res_col);
		fill_the_same_random(lhs_cpu, lhs_ocl, lhs_str, lhs_col);
		fill_the_same_random(rhs_cpu, rhs_ocl, rhs_str, rhs_col);
		res_ocl.fill_with_zeros();
		res_cpu.fill_with_zeros();
		EXPECT_TRUE(check_results(res_cpu, res_ocl, res_str, res_col));
		simple_multiply(res_cpu, lhs_cpu, rhs_cpu);
		res_ocl.prepare();
		lhs_ocl.prepare();
		rhs_ocl.prepare();
		simple_multiply(res_ocl, lhs_ocl, rhs_ocl);
		res_ocl.getResult();
		lhs_ocl.getResult();
		rhs_ocl.getResult();
		//res_cpu.dump();
		//res_ocl.dump();
		EXPECT_TRUE(check_results(res_cpu, res_ocl, res_str, res_col));
		res_cpu.fill_with_zeros();
		EXPECT_FALSE(check_results(res_cpu, res_ocl, res_str, res_col));
	}
};

class CheckCorrectnessBlock : public MyFixture
{
private:
	int res_str;
	int res_col;
	int lhs_str;
	int lhs_col;
	int rhs_str;
	int rhs_col;
	int block_size;
public:
	explicit CheckCorrectnessBlock(std::pair<int, int> res_size,
		std::pair<int, int> lhs_size,
		std::pair<int, int> rhs_size, int _block_size) :
		res_str(res_size.first),
		res_col(res_size.second),
		lhs_str(lhs_size.first),
		lhs_col(lhs_size.second),
		rhs_str(rhs_size.first),
		rhs_col(rhs_size.second),
		block_size(_block_size){}
	void TestBody() override
	{
		HandMadeCPUMatrix lhs_cpu(lhs_str, lhs_col);
		HandMadeCPUMatrix rhs_cpu(rhs_str, rhs_col);

		HandMadeCPUMatrix res_cpu_simple(res_str, res_col);
		HandMadeCPUMatrix res_cpu_block(res_str, res_col);

		HandMadeOpenCLMatrix lhs_ocl(lhs_str, lhs_col);
		HandMadeOpenCLMatrix rhs_ocl(rhs_str, rhs_col);

		fill_the_same_random(lhs_cpu, lhs_ocl, lhs_str, lhs_col);
		fill_the_same_random(rhs_cpu, rhs_ocl, rhs_str, rhs_col);
		HandMadeOpenCLMatrix res_ocl(res_str, res_col);
		res_ocl.prepare();
		lhs_ocl.prepare();
		rhs_ocl.prepare();
		simple_multiply(res_ocl, lhs_ocl, rhs_ocl);
		res_ocl.getResult();
		lhs_ocl.getResult();
		rhs_ocl.getResult();

		res_cpu_simple.fill_with_zeros();
		res_cpu_block.fill_with_zeros();
		EXPECT_TRUE(check_results(res_cpu_simple, res_cpu_block, res_str, res_col));
		simple_multiply(res_cpu_simple, lhs_cpu, rhs_cpu);
		EXPECT_FALSE(check_results(res_cpu_simple, res_cpu_block, res_str, res_col));
		block_multiply(res_cpu_block, lhs_cpu, rhs_cpu, block_size);
		//res_cpu.dump();
		//res_ocl.dump();
		EXPECT_TRUE(check_results(res_cpu_simple, res_cpu_block, res_str, res_col));
		EXPECT_TRUE(check_results(res_cpu_simple, res_ocl, res_str, res_col));

	}
};




void RegisterTests(const std::vector<arg_list>& simple, const std::vector<std::pair<arg_list, int>>& for_block )
{
	for (const auto& v : simple)
	{
		std::pair<int, int> res_size, lhs_size, rhs_size;
		std::tie(res_size, lhs_size, rhs_size) = v;
		::testing::RegisterTest(
			"CheckCorrectnessMult", ("Test:res_size" + std::to_string(res_size.first) + ":" +
							std::to_string(res_size.second) 
						+ ":lhs_size"+std::to_string(lhs_size.first) + ":"+
									std::to_string(lhs_size.second) 
				+ ":rhs_size" + std::to_string(rhs_size.first) + ":"+
				std::to_string(rhs_size.second)).c_str(), nullptr,
			nullptr,
			__FILE__, __LINE__,
			[=]()->MyFixture* {return new CheckCorrectness(res_size, 
											lhs_size, 
											rhs_size); });
	}
	for (const auto& v : for_block)
	{
		std::pair<int, int> res_size, lhs_size, rhs_size;
		std::tie(res_size, lhs_size, rhs_size) = v.first;
		::testing::RegisterTest(
			"CheckCorrectnessMultBlock", ("Test:res_size" + std::to_string(res_size.first) + ":" +
				std::to_string(res_size.second)
				+ ":lhs_size" + std::to_string(lhs_size.first) + ":" +
				std::to_string(lhs_size.second)
				+ ":rhs_size" + std::to_string(rhs_size.first) + ":" +
				std::to_string(rhs_size.second)+ 
				":block_size"+ std::to_string(v.second)).c_str(), nullptr,
				nullptr,
				__FILE__, __LINE__,
				[=]()->MyFixture* {return new CheckCorrectnessBlock(res_size,
					lhs_size,
					rhs_size, v.second); });
	}


}
// end gtest part

std::tuple<int, int, int> create_random_sizes_matrix_mul_simple()
{
	int a = std::rand() % 2000 + 1;// or may be size 0
	int b = std::rand() % 2000 + 1;
	int k = std::rand() % 2000 + 1;
	return std::tuple<int, int, int>(a, k, b);
}
std::tuple<int, int, int, int> create_random_sizes_matrix_block()
{
	int block_size = std::rand() % 100 + 1;// or may be size 0
	int in_blocks_a = std::rand() % 50 + 1;
	int in_blocks_b = std::rand() % 50 + 1;
	int in_blocks_k = std::rand() % 50+ 1;
	return std::tuple<int, int, int, int>(block_size * in_blocks_a,
											block_size * in_blocks_k,
											block_size * in_blocks_b,
											block_size);
}


int main(int argc, char *argv[])
{
        std::vector<arg_list> param_mult_right = {
			{{4,4},{4,2},{2,4}}
        };
		for (int i = 0; i < 10; i++)
		{
			int a, k, b;
			std::tie(a, k, b) = create_random_sizes_matrix_mul_simple();
			param_mult_right.push_back({ {a,b}, {a, k} ,{k,b} });
		}
		std::vector<std::pair<arg_list, int>> param_mult_block;
		for (int i = 0; i < 10; i++)
		{
			int a, k, b, block;
			std::tie(a, k, b, block) = create_random_sizes_matrix_block();
			param_mult_block.push_back({ {{a,b},{a, k},{k,b}} ,{block} });
		}





		prepare::prepare_OpenCL();
		::testing::InitGoogleTest(&argc, argv);
		RegisterTests(param_mult_right, param_mult_block);
		std::cout << "testing gtests" << std::endl;
		RUN_ALL_TESTS();
		prepare::exit_OpenCL();

}
