#include <iostream>
#include <vector>
#include <algorithm>
#include "OCL_diag.hpp"
#include <cassert>

using namespace std; 

#if 0
constexpr int num_str = 96;
constexpr int num_col = 96;
constexpr float dx = 0.1;
constexpr float dy = 0.1;
#endif

float f(float x, float y)
{
	return x + y;
}

float getStep()
{
	float step = 0;
	std::cin >> step;
	return step;
}
float getDistance()
{
	float x_1 = 0, y_1 = 0;
	float x_2 = 0, y_2 = 0;
	std::cin >> x_1 >> y_1;
	std::cin >> x_2 >> y_2;
	float Dx = std::abs(x_1 - x_2);
	float Dy = std::abs(y_1 - y_2);
	assert(Dx == Dy && "not square");
	return Dx;
}
void fill_vector(OCL_vector& vec, int num)
{
	// top
	for (int i = 0; i < num; ++i)
	{	
		std::cin >> vec(i, num - 1, num);
	}
	//right
	for (int i = 1; i < num; ++i)
	{	
		std::cin >> vec(num - 1, num - i - 1, num);
	}
	// bottom
	for (int i = 1; i < num; ++i)
	{	
		std::cin >> vec(num - i - 1, 0, num);
	}
	// right
	for (int i = 1; i < num - 1; ++i)
	{	
		std::cin >> vec(0, i, num);
	}
}

void fill_matrix(OCL_diag_matrix& matrix, int num)
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
		&& i <num*(num- 1))
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
		matrix.fill_diag_with_values(0, diag_0);
		matrix.fill_diag_with_values(1, diag_1);
		matrix.fill_diag_with_values(num, diag_col);
		matrix.fill_diag_with_values(num* num - 1, diag_last);
		matrix.fill_diag_with_values(num* num - num, diag_2_col);

}

int main(void)
{
	if (prepare::prepare_diag_ocl() != 0)
		return -1;// compile all needed...
	try {
		float step = getStep();
		float dist = getDistance();
		assert(dist > 0 && "negative distance");
		int num = static_cast<int>(dist / step + 1);
		//cout << "num is" << num << endl;

		OCL_vector field(num * num);
		OCL_vector next_field(num * num);
		field.fill_with_value(-34);
		next_field.fill_with_value(-105);
		fill_vector(field, num);

		OCL_diag_matrix A(num* num,num* num);
		//A.raw_dump();
		//A.pretty_dump();
		fill_matrix(A, num);
		//A.raw_dump();
		//A.pretty_dump();
		A.prepare();
		next_field.prepare();
		field.prepare();



		// calculations starts
		int num_iter = 0;
		float eps = 0.01;
		int need_iter = 2 * num * num / 3.14 * log(1 / eps);
		//cerr << "need iter " << need_iter << "\n";
		while (num_iter < need_iter)
		{
			multiply_check(next_field, A, field, num);
			std::swap(next_field, field);
			num_iter++;
		}
		A.getResult();
		next_field.getResult();
		field.getResult();
		field.prettydump(num, num);
		prepare::exit_diag_ocl();
	}
	catch (std::exception& e)
	{
		std::cout << e.what();
	}
	return 0;
}