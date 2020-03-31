#include "HandMadeOpenCLMatrix.h"

#include <utility>
#include <iostream>
#include <fstream>
#include <vector>

namespace prepare
{
	// kernels
	// not found better solution...
	cl::Program sources;



	static cl::KernelFunctor<
		std::vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>&,
		const std::vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>&,
		const std::vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>&,
		int,
		int,
		int
		>* simpleMultKernel;

	static cl::KernelFunctor<
		std::vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>&,
		const std::vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>&,
		const std::vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>&,
		int,
		int
		>* simpleAddKernel;

	static cl::KernelFunctor<
		std::vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>&,
		const std::vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>&,
		const std::vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>&,
		int,
		int
	>* simpleSubKernel;


	int prepare_OpenCL()
	{
		// need OpenCL 2.0
		std::vector<cl::Platform> platforms;
		cl::Platform::get(&platforms);
		cl::Platform plat;
		for (auto &p : platforms) {
			std::string platver = p.getInfo<CL_PLATFORM_VERSION>();
			std::cerr << "Plat: " << platver << "\n";
			if (platver.find("OpenCL 2.") != std::string::npos) {
				plat = p;
			}
		}
		if (plat() == 0) {
			std::cout << "No OpenCL 2.0 platform found.\n";
			return -1;
		}

		cl::Platform newP = cl::Platform::setDefault(plat);
		if (newP != plat) {
			std::cout << "Error setting default platform.";
			return -1;
		}
		// we choose platform and so on ...
		// time to create& build kernels
		std::string kernelMult{
R"(
	kernel void SimpleMult(
						global float *res,
						global const float *lhs,
						global const float *rhs,
						int res_str,
						int res_col,
						int lhs_col
						)
	{
		//printf("res_str %d, res_col %d, lhs_col %d\n", res_str, res_col, lhs_col);
		//printf("local0 %d, local1 %d   global0 %d, global1 %d\n", get_local_id(0), get_local_id(1),
		//							get_global_id(0), get_global_id(1));

		float current_res = 0;
			for(int i = 0; i < lhs_col;  i++)
			current_res += lhs[get_global_id(0) * lhs_col + i] 
						* rhs[i * res_col + get_global_id(1)];
		
		


		res[get_global_id(0) * res_col + get_global_id(1)] = current_res; 
	})"
		};
		std::string kernelLocalMult{
R"(
	kernel void SimpleMult(
						global float *res,
						global const float *lhs,
						global const float *rhs,
						int res_str,
						int res_col,
						int lhs_col
						)
	{
		//printf("res_str %d, res_col %d, lhs_col %d\n", res_str, res_col, lhs_col);
		//printf("local0 %d, local1 %d   global0 %d, global1 %d\n", get_local_id(0), get_local_id(1),
		//							get_global_id(0), get_global_id(1));

		float current_res = 0;
			for(int i = 0; i < lhs_col;  i++)
			current_res += lhs[get_global_id(0) * lhs_col + i] 
						* rhs[i * res_col + get_global_id(1)];
		
		


		res[get_global_id(0) * res_col + get_global_id(1)] = current_res; 
	})"
		};


		std::string kernelAdd{
		R"(
	kernel void SimpleAdd(
						global float *res,
						global const float *lhs,
						global const float *rhs,
						int str,
						int col
						)
	{
		//printf("res_str %d, res_col %d, lhs_col %d\n", res_str, res_col, lhs_col);
		//printf("local0 %d, local1 %d   global0 %d, global1 %d\n", get_local_id(0), get_local_id(1),
		//							get_global_id(0), get_global_id(1));
		float current_res = 0;


		current_res = lhs[get_global_id(0) * col + get_global_id(1)]
					+ rhs[get_global_id(0) * col + get_global_id(1)];
		res[get_global_id(0) * col + get_global_id(1)] = current_res; 
	})"
		};
		std::string kernelSub{
		R"(
	kernel void SimpleSub(
						global float *res,
						global const float *lhs,
						global const float *rhs,
						int str,
						int col
						)
	{
		//printf("res_str %d, res_col %d, lhs_col %d\n", res_str, res_col, lhs_col);
		//printf("local0 %d, local1 %d   global0 %d, global1 %d\n", get_local_id(0), get_local_id(1),
		//							get_global_id(0), get_global_id(1));
		float current_res = 0;


		current_res = lhs[get_global_id(0) * col + get_global_id(1)]
					- rhs[get_global_id(0) * col + get_global_id(1)];
		res[get_global_id(0) * col + get_global_id(1)] = current_res; 
	})"
		};

		std::vector<std::string> programStrings {
			kernelAdd,
			kernelSub,
			kernelMult 
		};
		sources = cl::Program(
			programStrings);

		try {
			sources.build("-cl-std=CL2.0");
		}
		catch (...) {
			// Print build info for all devices
			cl_int buildErr = CL_SUCCESS;
			auto buildInfo = sources.getBuildInfo<CL_PROGRAM_BUILD_LOG>(&buildErr);
			for (auto &pair : buildInfo) {
				std::cerr << pair.second << std::endl << std::endl;
			}

			return 1;
		}
		// we build all....
		simpleMultKernel = new
			cl::KernelFunctor<
			std::vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>&,
			const std::vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>&,
			const std::vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>&,
			int,
			int,
			int
			>(sources, "SimpleMult");

		simpleAddKernel = new cl::KernelFunctor<
			std::vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>&,
			const std::vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>&,
			const std::vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>&,
			int,
			int
			>(sources, "SimpleAdd");

		simpleSubKernel = new cl::KernelFunctor<
			std::vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>&,
			const std::vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>&,
			const std::vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>&,
			int,
			int
			>(sources, "SimpleSub");

		return 0;
	}

	void exit_OpenCL()
	{
		delete simpleMultKernel;
		delete simpleAddKernel;
		delete simpleSubKernel;
	}

}




HandMadeOpenCLMatrix::HandMadeOpenCLMatrix(const HandMadeOpenCLMatrix& copy):
    _raw_data(copy._raw_data),
    Str(copy.Str),
    Col(copy.Col)
    {}
HandMadeOpenCLMatrix& HandMadeOpenCLMatrix::operator=(const HandMadeOpenCLMatrix& copy)
{
    _raw_data = copy._raw_data;
    Col = copy.Col;
    Str = copy.Str;
    return *this;  
}

HandMadeOpenCLMatrix::HandMadeOpenCLMatrix(const std::string& FileName)
{
        
    std::fstream f(FileName);
    f >> Str >> Col;
    _raw_data.resize(Str*Col);
       
    std::for_each(_raw_data.begin(), 
                  _raw_data.end(),
                [&f](float& value)
                {
                    f >> value;    
                }
    );
    return;    
}
HandMadeOpenCLMatrix::HandMadeOpenCLMatrix(int str, int col):
    _raw_data(str * col),
    Str(str),
    Col(col){}

void HandMadeOpenCLMatrix::dump() const
{
	int current_col = 0;
	std::cerr << "Sizes:" << std::endl;
	std::cerr << "Str = " << Str << std::endl;
	std::cerr << "Col = " << Col << std::endl;
    std::for_each(_raw_data.begin(), 
                  _raw_data.end(),
        [&current_col, this](const float& value)
        {
		    current_col++;
            std::cerr<< value <<"\t";
			if (current_col % (Col) == 0)
			{
				std::cerr << std::endl;
			}
        });                    
}


void simple_multiply( HandMadeOpenCLMatrix& res,
                    const HandMadeOpenCLMatrix& lhs,
                    const HandMadeOpenCLMatrix& rhs)
{
	res.fill_with_zeros();
    assert(res.Str == lhs.Str &&
        res.Col == rhs.Col &&
        lhs.Col == rhs.Str);

	cl_int error;
	(*(prepare::simpleMultKernel))(
		cl::EnqueueArgs(cl::NDRange(res.Str, res.Col)),
		res._raw_data,
		lhs._raw_data,
		rhs._raw_data,
		res.Str,
		res.Col,
		lhs.Col,
		error
	);
}
#if 0
void local_multiply(HandMadeOpenCLMatrix& res,
	const HandMadeOpenCLMatrix& lhs,
	const HandMadeOpenCLMatrix& rhs)
{
	res.fill_with_zeros();
	assert(res.Str == lhs.Str &&
		res.Col == rhs.Col &&
		lhs.Col == rhs.Str);

	cl_int error;
	(*(prepare::simpleMultKernel))(
		cl::EnqueueArgs(cl::NDRange(res.Str, res.Col)),
		res._raw_data,
		lhs._raw_data,
		rhs._raw_data,
		res.Str,
		res.Col,
		lhs.Col,
		error
		);
}
#endif


void addition( HandMadeOpenCLMatrix& res, const HandMadeOpenCLMatrix& lhs, const HandMadeOpenCLMatrix& rhs)
{
    res.fill_with_zeros();
    assert(res.Str == lhs.Str && 
           lhs.Str == rhs.Str);
    assert(res.Col == lhs.Col && 
           lhs.Col == rhs.Col);
	cl_int error;
	(*prepare::simpleAddKernel)(
		cl::EnqueueArgs(cl::NDRange(res.Str, res.Col)),
		res._raw_data,
		lhs._raw_data,
		rhs._raw_data,
		res.Str,
		res.Col,
		error
	);
}
void sub( HandMadeOpenCLMatrix& res, const HandMadeOpenCLMatrix& lhs, const HandMadeOpenCLMatrix& rhs)
{
	res.fill_with_zeros();
	assert(res.Str == lhs.Str &&
		lhs.Str == rhs.Str);
	assert(res.Col == lhs.Col &&
		lhs.Col == rhs.Col);
	cl_int error;
	(*prepare::simpleSubKernel)(
		cl::EnqueueArgs(cl::NDRange(res.Str, res.Col)),
		res._raw_data,
		lhs._raw_data,
		rhs._raw_data,
		res.Str,
		res.Col,
		error
	);
}
