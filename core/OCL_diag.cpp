﻿#include "OCL_diag.hpp"


#include <cassert>
using namespace std;

namespace prepare
{
	cl::Program sources;

	const std::string simpleAdd = TO_STR(
		kernel void SimpleAdd(
			global float* res,
			global const float* lhs,
			global const float* rhs
		)
	{
		res[get_global_id(0)] = lhs[get_global_id(0)] + rhs[get_global_id(0)];
	}
		kernel void SimpleSub(
			global float* res,
			global const float* lhs,
			global const float* rhs
		)
	{
		res[get_global_id(0)] = lhs[get_global_id(0)] - rhs[get_global_id(0)];
	}
	);
	const string multSources = TO_STR(
		inline int positive_modulo(int i, int n) {
		return (i % n + n) % n;
	}
	const int glob_part_size = 32;
	kernel void SimpleDiagMul(
		global float* res, // vector
		global const float* lhs,// diag matrix
		global const float* rhs, // vector diag
		const int vec_size,
		const int diag_num
	)
	{
		int calculated_diag_num = -1;// real diag num

		int group_num = get_global_id(0);
		//printf("hi thread num gr %d \n", group_num);
		float partion[glob_part_size] = { 0 };
		int raw_answer_offset = group_num * glob_part_size;



		for (int raw_diag = 0; raw_diag < diag_num; raw_diag++)
		{
			calculated_diag_num = (int)lhs[vec_size + (vec_size + 1) * raw_diag];

			// calculation begin...
			global const float* start_diag = lhs + vec_size + (vec_size + 1) * raw_diag + 1;
			int start_diag_offset = positive_modulo(raw_answer_offset - calculated_diag_num, vec_size);
			for (int j = 0; j < glob_part_size; j++)
			{
				partion[j] += start_diag[(start_diag_offset + j) % vec_size] * rhs[(start_diag_offset + j) % vec_size];
			}
		}

		// copy answer
		for (int i = 0;
			i < glob_part_size;
			i++)
		{
			res[raw_answer_offset + i] = partion[i];
		}

	}
	kernel void SimpleDiagMul_check(
		global float* res, // vector
		global const float* lhs,// diag matrix
		global const float* rhs, // vector diag
		const int vec_size,
		const int diag_num,
		const int part_size// to check appropriate size for calculations, max = glob_part_size * 4
	)
	{
		int calculated_diag_num = -1;// real diag num

		int group_num = get_global_id(0);
		float partion[glob_part_size * 4] = { 0 };
		int raw_answer_offset = group_num * part_size;



		for (int raw_diag = 0; raw_diag < diag_num; raw_diag++)
		{
			calculated_diag_num = (int)lhs[vec_size + (vec_size + 1) * raw_diag];

			// calculation begin...
			global const float* start_diag = lhs + vec_size + (vec_size + 1) * raw_diag + 1;
			int start_diag_offset = positive_modulo(raw_answer_offset - calculated_diag_num, vec_size);
			for (int j = 0; j < part_size; j++)
			{
				partion[j] += start_diag[(start_diag_offset + j) % vec_size] * rhs[(start_diag_offset + j) % vec_size];
			}
		}

		for (int i = 0;
			i < part_size;
			i++)
		{
			res[raw_answer_offset + i] = partion[i];
		}
	}
	);

	static cl::KernelFunctor<
		std::vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>&,
		const std::vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>&,
		const std::vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>&
	>* simpleAddKernel;
	static cl::KernelFunctor<
		std::vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>&,
		const std::vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>&,
		const std::vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>&
	>* simpleSubKernel;

	static cl::KernelFunctor<
		std::vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>&,
		const std::vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>&,
		const std::vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>&,
		const int,
		const int
	>* simpleDiagMulKernel;

	static cl::KernelFunctor<
		std::vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>&,
		const std::vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>&,
		const std::vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>&,
		const int,
		const int, const int
	>* simpleDiagMul_size_checkKernel;

	int prepare_diag_ocl()
	{
		// need OpenCL 2.0
		std::vector<cl::Platform> platforms;
		cl::Platform::get(&platforms);
		cl::Platform plat;
		for (auto &p : platforms) {
			std::string platver = p.getInfo<CL_PLATFORM_VERSION>();
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

		std::vector<std::string> programStrings{
			simpleAdd, multSources
		};



		
		try {
		sources = cl::Program(
			programStrings);
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
		simpleAddKernel = new cl::KernelFunctor<
			std::vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>&,
			const std::vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>&,
			const std::vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>&
		>(sources, "SimpleAdd");

		simpleSubKernel = new cl::KernelFunctor<
			std::vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>&,
			const std::vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>&,
			const std::vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>&
		>(sources, "SimpleSub");

		simpleDiagMul_size_checkKernel = new cl::KernelFunctor<
			std::vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>&,
			const std::vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>&,
			const std::vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>&,
			const int, const int, const int
		>(sources, "SimpleDiagMul_check");

		simpleDiagMulKernel = new cl::KernelFunctor<
			std::vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>&,
			const std::vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>&,
			const std::vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>>&,
			const int, const int
		>(sources, "SimpleDiagMul");




		return 0;
	}
	int exit_diag_ocl()
	{
		delete simpleAddKernel;
		delete simpleDiagMulKernel;
		delete simpleDiagMul_size_checkKernel;
		delete simpleSubKernel;
		return 0;
	}

	


	

}

void add(OCL_vector& res, const OCL_vector& lhs, const OCL_vector& rhs)
{
	if (
		lhs.get_size() != rhs.get_size()
		||
		res.get_size() != lhs.get_size()
		)
	{
		throw "vector_add_size-mismatch";
	}
	cl_int error;
	
	prepare::simpleAddKernel->operator()(
		cl::EnqueueArgs(cl::NDRange(res.get_size())),
		res.data,
		lhs.data,
		rhs.data,
		error
		).wait();
	if (error != 0)
		std::cerr << GetOpenCLErrorInfo(error);
}
void sub(OCL_vector& res, const OCL_vector& lhs, const OCL_vector& rhs)
{
	if (
		lhs.get_size() != rhs.get_size()
		||
		res.get_size() != lhs.get_size()
		)
	{
		throw "vector_sub_size-mismatch";
	}
	cl_int error;
	
	prepare::simpleSubKernel->operator()(
		cl::EnqueueArgs(cl::NDRange(res.get_size())),
		res.data,
		lhs.data,
		rhs.data,
		error
		).wait();
	if (error != 0)
		std::cerr << GetOpenCLErrorInfo(error);
}

void multiply(OCL_vector& res, const OCL_diag_matrix& lhs, const OCL_vector& rhs)
{
	if (lhs.col != rhs.get_size())
		throw "lhs and rhs mismatch";
	if (lhs.str != res.get_size())
		throw "lhs and res mismatch";

	if (lhs.col != lhs.col)
		throw "Seems unsupported now sizes";
	
	cl_int error;

	prepare::simpleDiagMulKernel->operator()(
		cl::EnqueueArgs(cl::NDRange(res.get_size()/32), 1),
		res.data,
		lhs.raw_data,
		rhs.data,
		rhs.get_size(),
		lhs.get_alloc_diag_num(),
		error
		).wait();
	if (error != 0)
		std::cerr << GetOpenCLErrorInfo(error);
}
void multiply_check(OCL_vector& res, const OCL_diag_matrix& lhs, const OCL_vector& rhs, int part_size)
{
	if (lhs.col != rhs.get_size())
		throw "lhs and rhs mismatch";
	if (lhs.str != res.get_size())
		throw "lhs and res mismatch";

	if (lhs.col != lhs.col)
		throw "Seems unsupported now sizes";
	// without any mallocs
	cl_int error;

	prepare::simpleDiagMul_size_checkKernel->operator()(
		cl::EnqueueArgs(cl::NDRange(res.get_size()/part_size), 1),
		res.data,
		lhs.raw_data,
		rhs.data,
		rhs.get_size(),
		lhs.get_alloc_diag_num(),
		part_size,
		error
		).wait();
	if (error != 0)
		std::cerr << GetOpenCLErrorInfo(error);
}

std::string GetOpenCLErrorInfo(cl_int err) {
	return "Error " + GetOpenCLErrorName(err) + " (" + std::to_string((int)err) + ")\nDescription: " + GetOpenCLErrorDescription(err);
}

std::string GetOpenCLErrorName(cl_int errorCode)
{
	switch (errorCode)
	{
	case CL_SUCCESS:                            return "CL_SUCCESS";
	case CL_DEVICE_NOT_FOUND:                   return "CL_DEVICE_NOT_FOUND";
	case CL_DEVICE_NOT_AVAILABLE:               return "CL_DEVICE_NOT_AVAILABLE";
	case CL_COMPILER_NOT_AVAILABLE:             return "CL_COMPILER_NOT_AVAILABLE";
	case CL_MEM_OBJECT_ALLOCATION_FAILURE:      return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
	case CL_OUT_OF_RESOURCES:                   return "CL_OUT_OF_RESOURCES";
	case CL_OUT_OF_HOST_MEMORY:                 return "CL_OUT_OF_HOST_MEMORY";
	case CL_PROFILING_INFO_NOT_AVAILABLE:       return "CL_PROFILING_INFO_NOT_AVAILABLE";
	case CL_MEM_COPY_OVERLAP:                   return "CL_MEM_COPY_OVERLAP";
	case CL_IMAGE_FORMAT_MISMATCH:              return "CL_IMAGE_FORMAT_MISMATCH";
	case CL_IMAGE_FORMAT_NOT_SUPPORTED:         return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
	case CL_BUILD_PROGRAM_FAILURE:              return "CL_BUILD_PROGRAM_FAILURE";
	case CL_MAP_FAILURE:                        return "CL_MAP_FAILURE";
	case CL_MISALIGNED_SUB_BUFFER_OFFSET:       return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
	case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:    return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
	case CL_COMPILE_PROGRAM_FAILURE:            return "CL_COMPILE_PROGRAM_FAILURE";
	case CL_LINKER_NOT_AVAILABLE:               return "CL_LINKER_NOT_AVAILABLE";
	case CL_LINK_PROGRAM_FAILURE:               return "CL_LINK_PROGRAM_FAILURE";
	case CL_DEVICE_PARTITION_FAILED:            return "CL_DEVICE_PARTITION_FAILED";
	case CL_KERNEL_ARG_INFO_NOT_AVAILABLE:      return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";
	case CL_INVALID_VALUE:                      return "CL_INVALID_VALUE";
	case CL_INVALID_DEVICE_TYPE:                return "CL_INVALID_DEVICE_TYPE";
	case CL_INVALID_PLATFORM:                   return "CL_INVALID_PLATFORM";
	case CL_INVALID_DEVICE:                     return "CL_INVALID_DEVICE";
	case CL_INVALID_CONTEXT:                    return "CL_INVALID_CONTEXT";
	case CL_INVALID_QUEUE_PROPERTIES:           return "CL_INVALID_QUEUE_PROPERTIES";
	case CL_INVALID_COMMAND_QUEUE:              return "CL_INVALID_COMMAND_QUEUE";
	case CL_INVALID_HOST_PTR:                   return "CL_INVALID_HOST_PTR";
	case CL_INVALID_MEM_OBJECT:                 return "CL_INVALID_MEM_OBJECT";
	case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:    return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
	case CL_INVALID_IMAGE_SIZE:                 return "CL_INVALID_IMAGE_SIZE";
	case CL_INVALID_SAMPLER:                    return "CL_INVALID_SAMPLER";
	case CL_INVALID_BINARY:                     return "CL_INVALID_BINARY";
	case CL_INVALID_BUILD_OPTIONS:              return "CL_INVALID_BUILD_OPTIONS";
	case CL_INVALID_PROGRAM:                    return "CL_INVALID_PROGRAM";
	case CL_INVALID_PROGRAM_EXECUTABLE:         return "CL_INVALID_PROGRAM_EXECUTABLE";
	case CL_INVALID_KERNEL_NAME:                return "CL_INVALID_KERNEL_NAME";
	case CL_INVALID_KERNEL_DEFINITION:          return "CL_INVALID_KERNEL_DEFINITION";
	case CL_INVALID_KERNEL:                     return "CL_INVALID_KERNEL";
	case CL_INVALID_ARG_INDEX:                  return "CL_INVALID_ARG_INDEX";
	case CL_INVALID_ARG_VALUE:                  return "CL_INVALID_ARG_VALUE";
	case CL_INVALID_ARG_SIZE:                   return "CL_INVALID_ARG_SIZE";
	case CL_INVALID_KERNEL_ARGS:                return "CL_INVALID_KERNEL_ARGS";
	case CL_INVALID_WORK_DIMENSION:             return "CL_INVALID_WORK_DIMENSION";
	case CL_INVALID_WORK_GROUP_SIZE:            return "CL_INVALID_WORK_GROUP_SIZE";
	case CL_INVALID_WORK_ITEM_SIZE:             return "CL_INVALID_WORK_ITEM_SIZE";
	case CL_INVALID_GLOBAL_OFFSET:              return "CL_INVALID_GLOBAL_OFFSET";
	case CL_INVALID_EVENT_WAIT_LIST:            return "CL_INVALID_EVENT_WAIT_LIST";
	case CL_INVALID_EVENT:                      return "CL_INVALID_EVENT";
	case CL_INVALID_OPERATION:                  return "CL_INVALID_OPERATION";
	case CL_INVALID_GL_OBJECT:                  return "CL_INVALID_GL_OBJECT";
	case CL_INVALID_BUFFER_SIZE:                return "CL_INVALID_BUFFER_SIZE";
	case CL_INVALID_MIP_LEVEL:                  return "CL_INVALID_MIP_LEVEL";
	case CL_INVALID_GLOBAL_WORK_SIZE:           return "CL_INVALID_GLOBAL_WORK_SIZE";
	case CL_INVALID_PROPERTY:                   return "CL_INVALID_PROPERTY";
	case CL_INVALID_IMAGE_DESCRIPTOR:           return "CL_INVALID_IMAGE_DESCRIPTOR";
	case CL_INVALID_COMPILER_OPTIONS:           return "CL_INVALID_COMPILER_OPTIONS";
	case CL_INVALID_LINKER_OPTIONS:             return "CL_INVALID_LINKER_OPTIONS";
	case CL_INVALID_DEVICE_PARTITION_COUNT:     return "CL_INVALID_DEVICE_PARTITION_COUNT";
	case CL_INVALID_PIPE_SIZE:                  return "CL_INVALID_PIPE_SIZE";
	case CL_INVALID_DEVICE_QUEUE:               return "CL_INVALID_DEVICE_QUEUE";

	default:
		return "UNKNOWN ERROR CODE";
	}
}

// from http://www.techdarting.com/2014/01/opencl-errors.html
std::string GetOpenCLErrorDescription(cl_int err) {
	std::string result = "";

	switch (err) {
	case CL_SUCCESS: result += "Everything is good!"; break;
	case CL_DEVICE_NOT_FOUND: result += "No OpenCL devices that matched given device type were found"; break;
	case CL_DEVICE_NOT_AVAILABLE: result += "No OpenCL compatible device was found"; break;
	case CL_COMPILER_NOT_AVAILABLE: result += "OpenCL Compiler perhaps failed to configure itself, or check your OpenCL installation"; break;
	case CL_MEM_OBJECT_ALLOCATION_FAILURE: result += "Failed to allocate memory for buffer object"; break;
	case CL_OUT_OF_RESOURCES: result += "failure to allocate resources required by the OpenCL implementation on the device"; break;
	case CL_OUT_OF_HOST_MEMORY: result += "failure to allocate resources required by the OpenCL implementation on the host"; break;
	case CL_PROFILING_INFO_NOT_AVAILABLE: result += "returned by clGetEventProfilingInfo, if the CL_QUEUE_PROFILING_ENABLE flag is not set for the command-queue and if the profiling information is currently not available"; break;
	case CL_MEM_COPY_OVERLAP: result += "if source and destination buffers are the same buffer object and the source and destination regions overlap"; break;
	case CL_IMAGE_FORMAT_MISMATCH: result += "src and dst image do not use the same image format"; break;
	case CL_IMAGE_FORMAT_NOT_SUPPORTED: result += "the image format is not supported."; break;
	case CL_BUILD_PROGRAM_FAILURE: result += "program build error for given device, Use clGetProgramBuildInfo API call to get the build log of the kernel compilation."; break;
	case CL_MAP_FAILURE: result += "failed to map the requested region into the host address space. This error does not occur for buffer objects created with CL_MEM_USE_HOST_PTR or CL_MEM_ALLOC_HOST_PTR"; break;
	case CL_MISALIGNED_SUB_BUFFER_OFFSET: result += "no devices in given context associated with buffer for which the origin value is aligned to the CL_DEVICE_MEM_BASE_ADDR_ALIGN value"; break;
	case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST: result += "returned by clWaitForEvents(), execution status of any of the events in event list is a negative integer value i.e., error"; break;
	case CL_COMPILE_PROGRAM_FAILURE: result += "failed to compile the program source. Error occurs if clCompileProgram does not return until the compile has completed"; break;
	case CL_LINKER_NOT_AVAILABLE: result += "Linker unavailable"; break;
	case CL_LINK_PROGRAM_FAILURE: result += "failed to link the compiled binaries and perhaps libraries"; break;
	case CL_DEVICE_PARTITION_FAILED: result += "given partition name is supported by the implementation but input device couldn't be partitioned further"; break;
	case CL_KERNEL_ARG_INFO_NOT_AVAILABLE: result += "argument information is not available for the given kernel"; break;
	case CL_INVALID_VALUE: result += "values passed in the flags parameter is not valid"; break;
	case CL_INVALID_DEVICE_TYPE: result += "device type specified is not valid, its returned by clCreateContextFromType / clGetDeviceIDs"; break;
	case CL_INVALID_PLATFORM: result += "the specified platform is not a valid platform, its returned by clGetPlatformInfo /clGetDeviceIDs / clCreateContext / clCreateContextFromType"; break;
	case CL_INVALID_DEVICE: result += "device/s specified are not valid"; break;
	case CL_INVALID_CONTEXT: result += "the given context is invalid OpenCL context, or the context associated with certain parameters are not the same"; break;
	case CL_INVALID_QUEUE_PROPERTIES: result += "specified properties are valid but are not supported by the device, its returned by clCreateCommandQueue / clSetCommandQueueProperty"; break;
	case CL_INVALID_COMMAND_QUEUE: result += "the specified command-queue is not a valid command-queue"; break;
	case CL_INVALID_HOST_PTR: result += "host pointer is NULL and CL_MEM_COPY_HOST_PTR or CL_MEM_USE_HOST_PTR are set in flags or if host_ptr is not NULL but CL_MEM_COPY_HOST_PTR or CL_MEM_USE_HOST_PTR are not set in flags. returned by clCreateBuffer / clCreateImage2D / clCreateImage3D"; break;
	case CL_INVALID_MEM_OBJECT: result += "the passed parameter is not a valid memory, image, or buffer object"; break;
	case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR: result += "image format specified is not valid or is NULL, clCreateImage2D /clCreateImage3D returns this."; break;
	case CL_INVALID_IMAGE_SIZE: result += "Its returned by create Image functions 2D/3D, if specified image width or height are outbound or 0"; break;
	case CL_INVALID_SAMPLER: result += "specified sampler is an invalid sampler object"; break;
	case CL_INVALID_BINARY: result += "program binary is not a valid binary for the specified device, returned by clBuildProgram / clCreateProgramWithBinary"; break;
	case CL_INVALID_BUILD_OPTIONS: result += "the given build options are not valid"; break;
	case CL_INVALID_PROGRAM: result += "the given program is an invalid program object, returned by clRetainProgram / clReleaseProgram / clBuildProgram / clGetProgramInfo / clGetProgramBuildInfo / clCreateKernel / clCreateKernelsInProgram"; break;
	case CL_INVALID_PROGRAM_EXECUTABLE: result += "if there is no successfully built executable for program returned by clCreateKernel, there is no device in program then returned by clCreateKernelsInProgram, if no successfully built program executable present for device associated with command queue then returned by clEnqueueNDRangeKernel / clEnqueueTask"; break;
	case CL_INVALID_KERNEL_NAME: result += "mentioned kernel name is not found in program"; break;
	case CL_INVALID_KERNEL_DEFINITION: result += "arguments mismatch for the __kernel function definition and the passed ones, returned by clCreateKernel"; break;
	case CL_INVALID_KERNEL: result += "specified kernel is an invalid kernel object"; break;
	case CL_INVALID_ARG_INDEX: result += "clSetKernelArg if an invalid argument index is specified"; break;
	case CL_INVALID_ARG_VALUE: result += "the argument value specified is NULL, returned by clSetKernelArg"; break;
	case CL_INVALID_ARG_SIZE: result += "the given argument size (arg_size) do not match size of the data type for an argument, returned by clSetKernelArg"; break;
	case CL_INVALID_KERNEL_ARGS: result += "the kernel argument values have not been specified, returned by clEnqueueNDRangeKernel / clEnqueueTask"; break;
	case CL_INVALID_WORK_DIMENSION: result += "given work dimension is an invalid value, returned by clEnqueueNDRangeKernel"; break;
	case CL_INVALID_WORK_GROUP_SIZE: result += "the specified local workgroup size and number of workitems specified by global workgroup size is not evenly divisible by local workgroup size"; break;
	case CL_INVALID_WORK_ITEM_SIZE: result += "no. of workitems specified in any of local work group sizes is greater than the corresponding values specified by CL_DEVICE_MAX_WORK_ITEM_SIZES in that particular dimension"; break;
	case CL_INVALID_GLOBAL_OFFSET: result += "global_work_offset is not NULL. Must currently be a NULL value. In a future revision of OpenCL, global_work_offset can be used but not until OCL 1.2"; break;
	case CL_INVALID_EVENT_WAIT_LIST: result += "event wait list is NULL and (no. of events in wait list > 0), or event wait list is not NULL and no. of events in wait list is 0, or specified event objects are not valid events"; break;
	case CL_INVALID_EVENT: result += "invalid event objects specified"; break;
	case CL_INVALID_GL_OBJECT: result += "not a valid GL buffer object"; break;
	case CL_INVALID_BUFFER_SIZE: result += "the value of the parameter size is 0 or exceeds CL_DEVICE_MAX_MEM_ALLOC_SIZE for all devices specified in the parameter context, returned by clCreateBuffer"; break;
	case CL_INVALID_GLOBAL_WORK_SIZE: result += "specified global work size is NULL, or any of the values specified in global work dimensions are 0 or exceeds the range given by the sizeof(size_t) for the device on which the kernel will be enqueued, returned by clEnqueueNDRangeKernel"; break;
	case CL_INVALID_PROPERTY: result += "context property name in properties is not a supported property name, returned by clCreateContext"; break;
	case CL_INVALID_IMAGE_DESCRIPTOR: result += "values specified in image description are invalid"; break;
	case CL_INVALID_COMPILER_OPTIONS: result += "compiler options specified by options are invalid, returned by clCompileProgram"; break;
	case CL_INVALID_LINKER_OPTIONS: result += "linker options specified by options are invalid, returned by clLinkProgram"; break;
	default: result += "No description available"; break;
	}

	return result;
}

