#include "CM_diag.hpp"
#include "common/cm_rt_helpers.h"
#include "common/isa_helpers.h"
#include "common/bitmap_helpers.h"

namespace prepare {
    static CmProgram* CM_vector_add_program = nullptr;
    static CmKernel* CM_vector_add_kernel = nullptr;
    static CmTask* CM_vector_add_task = nullptr;
    
    static CmKernel* CM_vector_sub_kernel = nullptr;
    static CmTask* CM_vector_sub_task = nullptr;
    
    static CmProgram* CM_mult_program = nullptr;
    static CmKernel* CM_mult_kernel = nullptr;
    static CmTask* CM_mult_task = nullptr;

    
    static CmQueue* cmd_queue = nullptr;// put by this
    
    static CmDevice* cm_device = nullptr;

    int prepare_diag_CM()
    {
        unsigned int version = 0;
        cm_result_check(::CreateCmDevice(cm_device, version));

        std::string vector_add = cm::util::isa::loadFile("../../cm_kernels/vector_operations.isa");
        if (vector_add.size() == 0) {
            std::cerr << "Error: empty ISA binary vector operations.\n";
            std::exit(1);
        }


        std::string mult = cm::util::isa::loadFile("../../cm_kernels/multiplication.isa");
        if (mult.size() == 0) {
            std::cerr << "Error: empty ISA binary multiplication.\n";
            std::exit(1);
        }

        cm_result_check(cm_device->LoadProgram(const_cast<char*>(mult.data()),
            mult.size(),
            CM_mult_program));
        cm_result_check(cm_device->LoadProgram(const_cast<char*>(vector_add.data()),
            vector_add.size(),
            CM_vector_add_program));

        cm_result_check(cm_device->CreateKernel(CM_mult_program,
            "mult_simple",
            CM_mult_kernel));
        cm_result_check(cm_device->CreateKernel(CM_vector_add_program,
            "vector_add",
            CM_vector_add_kernel));
        cm_result_check(cm_device->CreateKernel(CM_vector_add_program,
            "vector_sub",
            CM_vector_sub_kernel));




        cm_result_check(cm_device->CreateTask(CM_mult_task));
        cm_result_check(cm_device->CreateTask(CM_vector_add_task));
        cm_result_check(cm_device->CreateTask(CM_vector_sub_task));
        

        cm_result_check(CM_mult_task->AddKernel(CM_mult_kernel));
        cm_result_check(CM_vector_add_task->AddKernel(CM_vector_add_kernel));
        cm_result_check(CM_vector_sub_task->AddKernel(CM_vector_sub_kernel));


        cm_result_check(cm_device->CreateQueue(cmd_queue));
        return 0;


    }
    int exit_diag_CM()
    {
        cm_result_check(cm_device->DestroyTask(CM_vector_add_task));
        cm_result_check(cm_device->DestroyTask(CM_vector_sub_task));
        cm_result_check(cm_device->DestroyTask(CM_mult_task));
        cm_device->DestroyKernel(CM_mult_kernel);
        cm_device->DestroyKernel(CM_vector_add_kernel);
        cm_device->DestroyKernel(CM_vector_sub_kernel);
        cm_device->DestroyProgram(CM_vector_add_program);
        cm_device->DestroyProgram(CM_mult_program);
        
        cm_result_check(::DestroyCmDevice(cm_device));
        return 0;
    
    }
}
 void CM_diag_matrix::alloc_gpu_mem()
 {
     cm_result_check(prepare::cm_device->CreateBuffer(sizeof(float) * raw_data.size(), gpu_data));
     cm_result_check(gpu_data->GetIndex(gpu_index));
     cm_result_check(prepare::cm_device->CreateThreadSpace(col/32, 1, threads_simple));
 }
 void CM_diag_matrix::copy_to_gpu()
 {
    cm_result_check(gpu_data->WriteSurface((unsigned char*)raw_data.data(), nullptr));
 }
 void CM_diag_matrix::getResult()
 {
     cm_result_check(gpu_data->ReadSurface((unsigned char*)raw_data.data(), nullptr));
 }

 void CM_diag_matrix::dealloc_gpu_mem()
 {
     cm_result_check(prepare::cm_device->DestroySurface(gpu_data));
     gpu_index = nullptr;
     prepare::cm_device->DestroyThreadSpace(threads_simple);
     threads_simple = nullptr;
 }


void CM_vector::alloc_gpu_mem()
{
    cm_result_check(prepare::cm_device->CreateBuffer(sizeof(float) * data.size(),    gpu_data));
    cm_result_check(gpu_data->GetIndex(gpu_index));
    int threads_num = get_size() / 16;
    if (get_size() / 16 == 0)
    {
        threads_num = 1;
    }
    cm_result_check(prepare::cm_device->CreateThreadSpace( threads_num ,1,threads_simple));
}
void CM_vector::copy_to_gpu()
{
    cm_result_check(gpu_data->WriteSurface((unsigned char*)data.data(), nullptr));

}
void CM_vector::getResult()
{
        cm_result_check(gpu_data->ReadSurface((unsigned char*)data.data(),nullptr));
}
void CM_vector::dealloc_gpu_mem()
{
    cm_result_check(prepare::cm_device->DestroySurface(gpu_data));
    gpu_index = nullptr;
    prepare::cm_device->DestroyThreadSpace(threads_simple);
    threads_simple = nullptr;
    prepare::cmd_queue->DestroyEvent(status);
}
void multiply(CM_vector& res, const CM_diag_matrix& lhs, const CM_vector& rhs)
{
	if (lhs.col != rhs.get_size())
		throw "lhs and rhs mismatch";
	if (lhs.str != res.get_size())
		throw "lhs and res mismatch";

	if (lhs.col != lhs.col)
		throw "Seems unsupported now sizes";
    int size = rhs.get_size();
    int diag_num = lhs.get_alloc_diag_num();



    (prepare::CM_mult_kernel->SetKernelArg(0, sizeof(SurfaceIndex), res.gpu_index));
    (prepare::CM_mult_kernel->SetKernelArg(1, sizeof(SurfaceIndex), lhs.gpu_index));
    (prepare::CM_mult_kernel->SetKernelArg(2, sizeof(SurfaceIndex), rhs.gpu_index));
    (prepare::CM_mult_kernel->SetKernelArg(3, sizeof(size), &size));
    (prepare::CM_mult_kernel->SetKernelArg(4, sizeof(diag_num), &diag_num));

    prepare::cmd_queue->EnqueueFast(prepare::CM_mult_task, res.status, lhs.threads_simple);// non blocking...

    res.status->WaitForTaskFinished();
}
void add(CM_vector& res, const CM_vector& lhs, const CM_vector& rhs)
{
	if (
		lhs.get_size() != rhs.get_size()
		||
		res.get_size() != lhs.get_size()
		)
	{
		throw "vector_add_size-mismatch";
	}
    prepare::CM_vector_add_kernel->SetKernelArg(0, sizeof(SurfaceIndex), res.gpu_index);
    prepare::CM_vector_add_kernel->SetKernelArg(1, sizeof(SurfaceIndex), lhs.gpu_index);
    prepare::CM_vector_add_kernel->SetKernelArg(2, sizeof(SurfaceIndex), rhs.gpu_index);


    prepare::cmd_queue->EnqueueFast(prepare::CM_vector_add_task, res.status, res.threads_simple);// non blocking...

    res.status->WaitForTaskFinished();
}

void sub(CM_vector& res, const CM_vector& lhs, const CM_vector& rhs)
{
	if (
		lhs.get_size() != rhs.get_size()
		||
		res.get_size() != lhs.get_size()
		)
	{
		throw "vector_sub_size-mismatch";
	}
    prepare::CM_vector_sub_kernel->SetKernelArg(0, sizeof(SurfaceIndex), res.gpu_index);
    prepare::CM_vector_sub_kernel->SetKernelArg(1, sizeof(SurfaceIndex), lhs.gpu_index);
    prepare::CM_vector_sub_kernel->SetKernelArg(2, sizeof(SurfaceIndex), rhs.gpu_index);


    prepare::cmd_queue->EnqueueFast(prepare::CM_vector_sub_task, res.status, res.threads_simple);// non blocking...

    res.status->WaitForTaskFinished();
}
