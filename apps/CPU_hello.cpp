#include <iostream>
#include "HandMadeCPUMatrix.h"

#ifdef _DEBUG
#define BASIC_DEBUG
#endif
#include "CPU_diag.hpp"
#include <numeric>



int main(int ac, char *av[])
{
	try {
		CPU_vector vec(4);
		vec.col_dump();
		vec(3) = 5;
		vec.str_dump();
		std::vector<float> example(4);
		std::iota(example.begin(), example.end(), 7.3);
		vec.fill_with_vector(example);
		vec.str_dump();
		vec(3) = 5;
		vec.col_dump();

		CPU_diag_matrix test(9, 4);
		test.fill_diag_with_value(3, -1);
		std::vector<float> to_put = { 1.0,2.1,3.2,4.3 };
		test.raw_dump();
		test.pretty_dump();
		test.fill_diag_with_values(2, to_put);
		test(2, 0) = 100;
		test.raw_dump();
		test.pretty_dump();

		std::cout << "multiply checking...\n";

		CPU_diag_matrix lhs(7, 7);
		CPU_vector rhs(7);
		CPU_vector res(7);
		lhs.raw_dump();
		lhs.pretty_dump();
		rhs.str_dump();
		res.str_dump();

		lhs.fill_diag_with_value(0, -1.3);
		lhs.fill_diag_with_value(2, 7.3);

		lhs.fill_diag_with_value(3, -1);
		std::vector<float> tmp(7);
		std::iota(tmp.begin(), tmp.end(), 40.4);
		lhs.fill_diag_with_values(6, tmp);
		lhs.fill_diag_with_values(4, tmp);
		lhs.raw_dump();
		lhs.pretty_dump();

		res.fill_with_value(-100.2);
		std::iota(rhs.begin(), rhs.end(), -1.1);
		rhs.str_dump();
		std::cout << "after mult\n";

		multiply(res, lhs, rhs);

		res.str_dump();

		std::cout << "check add and sub\n";
		res.str_dump();
		rhs.str_dump();
		CPU_vector add_res(7);
		add(add_res, res, rhs);
		add_res.str_dump();

		add_res.str_dump();
		std::cout << "---\n";
		rhs.str_dump();


		sub(res, add_res, rhs);
		res.str_dump();

	} 
	catch (char* error)
	{
		std::cout << error << "\n";
	}



    return 0;
}
