#include <cl/queue.hpp>

cl::queue::queue(cl_context __context, cl_device_id __device_id) throw(cl_exception)
{
	cl_int err;
#ifdef CL_PROFILING
	_queue = clCreateCommandQueue(__context, __device_id, CL_QUEUE_PROFILING_ENABLE, &err);
#else
	_queue = clCreateCommandQueue(__context, __device_id, 0, &err);
#endif
	if(err != CL_SUCCESS)
		throw cl_exception("clCreateCommandQueue",err);
}

cl::queue::queue(const context &__context, cl_device_id __device_id) throw(cl_exception)
	: queue(__context.get_cl_context(),__device_id)
{
	
}

cl::queue::~queue()
{
	clFinish(_queue);
	clReleaseCommandQueue(_queue);
}

void cl::queue::flush()
{
	clFlush(_queue);
}

cl_command_queue cl::queue::get_cl_command_queue() const
{
	return _queue;
}
