#include "basic_diag.hpp"
#include "common.hpp"

class CPU_diag_matrix;
class CPU_vector;
class CPU_vector : public base_vector<float, std::allocator<float>>
{
protected:
	std::string get_class() const noexcept final
	{
		return "CPU_vector";
	};
public:
	CPU_vector(int size) :
		base_vector(size) {}
	//friend multiply_diag(CPU_vector& res, const CPU_diag_matrix& lhs, )
	friend void multiply(CPU_vector& res, const CPU_diag_matrix& lhs, const CPU_vector& rhs);
	friend void add(CPU_vector& res, const CPU_vector& lhs, const CPU_vector& rhs);
	friend void sub(CPU_vector& res, const CPU_vector& lhs, const CPU_vector& rhs);
};

class CPU_diag_matrix : public base_diag_matrix<float, std::allocator<float>>
{
public:
	CPU_diag_matrix(int str, int col) :
		base_diag_matrix(str, col) {}
	friend void multiply(CPU_vector& res, const CPU_diag_matrix& lhs, const CPU_vector& rhs);

	std::string get_class() const noexcept final
	{
		return "CPU_diag_matrix";
	}
};



