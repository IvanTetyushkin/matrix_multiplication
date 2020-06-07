#include "CPU_diag.hpp"

void add(CPU_vector& res, const CPU_vector& lhs, const CPU_vector& rhs)
{
	if (res.get_size() != lhs.get_size())
		throw "mismatch in add";
	if (res.get_size() != rhs.get_size())
		throw "mismatch in add";
	std::transform(lhs.begin(), lhs.end(), rhs.begin(), res.begin(), std::plus<float>());
}

void sub(CPU_vector& res, const CPU_vector& lhs, const CPU_vector& rhs)
{
	if (res.get_size() != lhs.get_size())
		throw "mismatch in sub";
	if (res.get_size() != rhs.get_size())
		throw "mismatch in sub";
	std::transform(lhs.begin(), lhs.end(), rhs.begin(), res.begin(), std::minus<float>());
}
// need change to more fast...
void multiply(CPU_vector& res, const CPU_diag_matrix& lhs, const CPU_vector& rhs)
{
	if (lhs.col != rhs.get_size())
		throw "lhs and rhs mismatch";
	if (lhs.str != res.get_size())
		throw "lhs and res mismatch";

	if (lhs.col != lhs.col)
		throw "Seems unsupported now sizes";
	/// seems alright
	res.fill_with_value(0);
	// without any mallocs

	for (int raw_diag = 0 ; raw_diag < lhs.get_alloc_diag_num(); raw_diag++)// throw all diags
	{
		int diag_num = lhs.raw_data[lhs.str + (1 + lhs.col) * raw_diag];
			auto diag_ptr = lhs.get_pointer_to_exist_diag(diag_num);
			// good for make parallel
			for (int i = 0; i < res.get_size(); i++)
			{

				res(i) += diag_ptr[positive_modulo(i - diag_num, rhs.get_size())] * rhs(positive_modulo(i - diag_num, rhs.get_size()));

			}
	}
}

void check_norm(const CPU_vector& lhs, const CPU_vector& rhs, int& need_next,  float stop_error)
{
	if (rhs.get_size() != lhs.get_size())
		throw "mismatch in error";
	int size = rhs.get_size();
	for (int i = 0; i < size/ 16; i++)
	{
		float current_error = 0;
		for (int j = 0; j < 16; j++)
		{
			current_error += abs(lhs(i * 16 + j) - rhs(i* 16 + j));
		}
		if ( current_error > stop_error)
		{
            need_next = 1;
            return;
		}
	}
	return;
}

