#include <iostream>
#include "HandMadeOpenCLMatrix.h"
#include <vector>
#include <algorithm>
#include "OCL_diag.hpp"

using namespace std; 

int main(void)
{
	if (prepare::prepare_diag_ocl() != 0)
		return -1;// compile all needed...
	try {
		cout << "Add check\n";
		OCL_vector lhs(16);
		lhs.fill_with_value(-7.0);
		OCL_vector rhs(16);
		std::vector<float> to_put(16);
		std::iota(to_put.begin(), to_put.end(), 1.7);
		rhs.fill_with_vector(to_put);
		OCL_vector res(16);
		res.str_dump();
		lhs.str_dump();
		rhs.str_dump();
		res.prepare();
		rhs.prepare();
		lhs.prepare();

		add(res, lhs, rhs);

		res.getResult();
		rhs.getResult();
		lhs.getResult();
		res.str_dump();


		OCL_diag_matrix A(16, 16);
		A.fill_diag_with_value(5, 7.6);
		//A.fill_diag_with_value(2, 1.6);
		A.fill_diag_with_value(0, -1.1);
		A.raw_dump();

		A.pretty_dump();
		rhs.col_dump();
		A.prepare();
		rhs.prepare();
		res.prepare();
		multiply(res, A, rhs);
		res.getResult();
		A.getResult();
		rhs.getResult();
		res.col_dump();

		prepare::exit_diag_ocl();
		cout << TO_STR(hi_there) << "\n";
	}
	catch (std::exception& e)
	{
		std::cout << e.what();
	}
	return 0;
}