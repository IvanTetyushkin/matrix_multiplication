#include <iostream>

#ifdef _DEBUG
#define BASIC_DEBUG
#endif
#include "CPU_diag.hpp"
#include <numeric>

constexpr int num_str = 16;
constexpr int num_col = 16;
constexpr float dx = 0.1;
constexpr float dy = 0.1;


float f(float x, float y)
{
	return x + y;
}


using namespace std;
int main(int ac, char *av[])
{
	try {
		CPU_vector field(num_str * num_col);
		CPU_vector next_field(num_str * num_col);
		field.fill_with_value(-1);
		next_field.fill_with_value(-1);
		field.prettydump(num_str, num_col);
		for (int i = 0; i < num_col; i++)
		{
			field(0, i, num_col) = f(0 * dx, i*dy);// up 
			field(num_str - 1, i, num_col) = f((num_str - 1)*dx , i *dy);// botton
			field(i, 0, num_col) = f(i*dx, 0*dy);//right part
			field(i, num_col - 1, num_col) = f(i*dx, (num_col -1)*dy);// right part
		}
		cout << "================\n";
        field.prettydump(num_str, num_col);

		CPU_diag_matrix A(num_str* num_col,num_str* num_col);
		//A.raw_dump();
		//A.pretty_dump();
		std::vector<float> diag_0(num_str * num_col);
		std::vector<float> diag_1(num_str * num_col);
		vector<float> diag_col(num_str * num_col);
		vector<float> diag_last(num_col * num_str);
		vector<float> diag_2_col(num_col * num_str);
		for (int i = 0; i < num_str * num_col; i++)
		{
			int ost = num_col - 2;
			if (i < num_col)
			{
				diag_0[i] = 1;
			}
			if (i % num_col < ost && 
				(i >= num_col
		&& i <num_col*(num_str- 1))
				)
			{
				diag_1[i] = 0.25;
			}
			if ((i + num_col - 1) % num_col < ost &&
				(i + num_col - 1) < num_col * (num_str - 1))
			{
				diag_col[i] = 0.25;
			}
			if (((i + num_col - 2) % num_col < ost)
				&& i > num_col&& i < num_col * (num_str - 1))
				diag_last[i] = 0.25;
			if ((i > num_col * 2) &&
				(i + num_col - 1) % num_col < ost
				)
				diag_2_col[i] = 0.25;


			if (i >= num_col * (num_str - 1))
			{
				diag_0[i] = 1;

			}
			if (i % num_col == 0)
			{
				diag_0[i] = 1;
			}
			if (i % num_col == (num_col - 1))
			{
				diag_0[i] = 1;
			}


		}
		A.fill_diag_with_values(0, diag_0);
		A.fill_diag_with_values(1, diag_1);
		A.fill_diag_with_values(num_col, diag_col);
		A.fill_diag_with_values(num_col* num_str - 1, diag_last);
		A.fill_diag_with_values(num_col* num_str - num_col, diag_2_col);
		
		//A.raw_dump();
		//A.pretty_dump();

		// calculations starts
		int num_iter = 0;
		float eps = 0.001;
		int need_iter = 2 * pow(num_col * num_str, 2) / pow(3.14, 2) * logf(1 / eps);
		cout << " need_iter = " << need_iter << "\n";
		while (num_iter < need_iter)
		{
			multiply_omp(next_field, A, field);
			std::swap(next_field, field);
			num_iter++;
		}
		cout << "num iterations" << num_iter << "\n";
		field.prettydump(num_str, num_col);







	} 
	catch (char* error)
	{
		std::cout << error << "\n";
	}



    return 0;
}
