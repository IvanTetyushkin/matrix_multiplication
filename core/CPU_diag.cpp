#include "CPU_diag.hpp"

void add(CPU_vector& res, const CPU_vector& lhs, const CPU_vector& rhs)
{
	if (res.get_size() != lhs.get_size())
		throw "mismatch in add";
	if (res.get_size() != rhs.get_size())
		throw "mismatch in add";
	std::transform(lhs.begin(), lhs.end(), rhs.begin(), res.begin(), std::plus<int>());
}

void sub(CPU_vector& res, const CPU_vector& lhs, const CPU_vector& rhs)
{
	if (res.get_size() != lhs.get_size())
		throw "mismatch in add";
	if (res.get_size() != rhs.get_size())
		throw "mismatch in add";
	std::transform(lhs.begin(), lhs.end(), rhs.begin(), res.begin(), std::minus<int>());
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

	for (int i{ 0 }; i < lhs.str; i++)// throw all diags
	{
		if (lhs.check_already_alloced(i))
		{
#ifdef mul_debug
			std::cout << "in as i = " << i << "\n";
#endif
			auto second_rhs_it = rhs.begin() + lhs.str - i;

			auto second_lhs_it = lhs.get_exist_diag_iterator_begin(i) + lhs.str - i;

			auto second_res_it = res.begin() + i;

			transform_saved(
				lhs.get_exist_diag_iterator_begin(i),
				second_lhs_it,
				rhs.begin(),
				second_res_it, std::multiplies<int>());
#ifdef mul_debug
			std::cout << "half done\n";
			lhs.pretty_dump();
			rhs.str_dump();
			res.str_dump();
#endif
			transform_saved(
				second_lhs_it,
				lhs.get_exist_diag_iterator_end(i),
				second_rhs_it,
				res.begin(),
				std::multiplies<int>()
			);
#ifdef mul_debug
			std::cout << "diag done\n";
			lhs.pretty_dump();
			rhs.str_dump();
			res.str_dump();
#endif
		}
	}
}

