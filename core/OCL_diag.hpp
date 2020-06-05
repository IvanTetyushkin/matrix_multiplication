#include "basic_diag.hpp"

#include "CL/cl2.hpp"

#include "CL/cl.h"

class OCL_diag_matrix;
class OCL_vector;

class OCL_vector : public base_vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>
{
protected:
	std::string get_class() const noexcept final
	{
		return "OCL_vector";
	};
public:
	OCL_vector(int size) :
		base_vector(size) {}
	friend void multiply(OCL_vector& res, const OCL_diag_matrix& lhs, const OCL_vector& rhs);
	friend void add(OCL_vector& res, const OCL_vector& lhs, const OCL_vector& rhs);
	friend void sub(OCL_vector& res, const OCL_vector& lhs, const OCL_vector& rhs);

	void prepare()
	{
		cl::unmapSVM(data);
	}
	void getResult()
	{
		cl::mapSVM(data);
	}
};

class OCL_diag_matrix : public base_diag_matrix<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>
{
public:
	OCL_diag_matrix(int str, int col) :
		base_diag_matrix<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>(str, col) {}
	friend void multiply(OCL_vector& res, const OCL_diag_matrix& lhs, const OCL_vector& rhs);

	std::string get_class() const noexcept final
	{
		return "OCL_diag_matrix";
	}
	void prepare()
	{
		cl::unmapSVM(raw_data);
	}
	void getResult()
	{
		cl::mapSVM(raw_data);
	}
	friend void multiply(OCL_vector& res, const OCL_diag_matrix& lhs, const OCL_vector& rhs);
	friend void local_mult(OCL_vector& res, const OCL_diag_matrix& lhs, const OCL_vector& rhs);
};


namespace prepare
{
	int prepare_diag_ocl();
	int exit_diag_ocl();
}

std::string GetOpenCLErrorName(cl_int errorCode);
std::string  GetOpenCLErrorDescription(cl_int err);
std::string GetOpenCLErrorInfo(cl_int err);
