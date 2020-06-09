#ifndef _CM_DIAG
#define _CM_DIAG
#include "basic_diag.hpp"
#ifdef ADD_CM
#include "cm_rt.h"
class CM_diag_matrix;
class CM_vector;

class CM_vector : public base_vector<float, std::allocator <float >>
{
protected:
	std::string get_class() const noexcept final
	{
		return "CM_vector";
	};
	CmBuffer* gpu_data = nullptr;
	SurfaceIndex* gpu_index = nullptr;
	CmThreadSpace* threads_simple = nullptr;

public:
	CmEvent* status = nullptr;
	CM_vector(int size) :
		base_vector(size) {}
	friend void multiply(CM_vector& res, const CM_diag_matrix& lhs, const CM_vector& rhs);
	friend void add(CM_vector& res, const CM_vector& lhs, const CM_vector& rhs);
	friend void sub(CM_vector& res, const CM_vector& lhs, const CM_vector& rhs);


	// should be called after all adding diags
	void alloc_gpu_mem();
	// before end of life of object
	void dealloc_gpu_mem();

	void copy_to_gpu();
	void getResult();
};
class CM_diag_matrix : public base_diag_matrix<float, std::allocator<float>>
{
	CmBuffer* gpu_data = nullptr;
	SurfaceIndex* gpu_index = nullptr;
	CmThreadSpace* threads_simple = nullptr;
public:
	CM_diag_matrix(int str, int col) :
		base_diag_matrix<float, std::allocator<float>>(str, col) {}
	friend void multiply(CM_vector& res, const CM_diag_matrix& lhs, const CM_vector& rhs);

	std::string get_class() const noexcept final
	{
		return "CM_diag_matrix";
	}
	void alloc_gpu_mem();
	void dealloc_gpu_mem();

	void copy_to_gpu();
	void getResult();


	friend void multiply(CM_vector& res, const CM_diag_matrix& lhs, const CM_vector& rhs);
};

namespace prepare
{
	int prepare_diag_CM();
	int exit_diag_CM();
}
#endif
#endif
