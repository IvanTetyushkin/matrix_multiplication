#include "basic_diag.hpp"

class CM_diag_matrix;
class CM_vector;

class CM_vector : public base_vector<float, std::allocator<float>>
{
protected:
	std::string get_class() const noexcept final
	{
		return "CM_vector";
	};
public:
	CM_vector(int size) :
		base_vector(size) {}
	//friend multiply_diag(CPU_vector& res, const CPU_diag_matrix& lhs, )
	friend void multiply(CM_vector& res, const CM_diag_matrix& lhs, const CM_vector& rhs);
	friend void add(CM_vector& res, const CM_vector& lhs, const CM_vector& rhs);
	friend void sub(CM_vector& res, const CM_vector& lhs, const CM_vector& rhs);

	void prepare()
	{
		///
	}
	void getResult()
	{
		///
	}
};
class CM_diag_matrix : public base_diag_matrix<float, std::allocator<float>>
{
public:
	CM_diag_matrix(int str, int col) :
		base_diag_matrix<float, std::allocator<float>>(str, col) {}
	friend void multiply(CM_vector& res, const CM_diag_matrix& lhs, const CM_vector& rhs);

	std::string get_class() const noexcept final
	{
		return "CM_diag_matrix";
	}
	void prepare()
	{
		// tbd
	}
	void getResult()
	{
		// tbd
	}
	friend void multiply(CM_vector& res, const CM_diag_matrix& lhs, const CM_vector& rhs);
};
