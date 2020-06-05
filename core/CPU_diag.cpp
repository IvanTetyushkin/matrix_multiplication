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
		throw "mismatch in add";
	if (res.get_size() != rhs.get_size())
		throw "mismatch in add";
	std::transform(lhs.begin(), lhs.end(), rhs.begin(), res.begin(), std::minus<float>());
}

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

	for (int diag = 0 ; diag < lhs.str; diag++)// throw all diags
	{
		if (lhs.check_already_alloced(diag))
		{
			auto diag_ptr = lhs.get_pointer_to_exist_diag(diag);
			// good for make parallel
			for (int i = 0; i < res.get_size(); i++)
			{

				res(i) += diag_ptr[positive_modulo(i - diag, rhs.get_size())] * rhs(positive_modulo(i - diag, rhs.get_size()));

			}
		}
	}
}

