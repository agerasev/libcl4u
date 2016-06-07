#include <cl/buffer_object.hpp>

cl_mem cl::buffer_object::create_cl_buffer(cl_context context, size_t size) throw(cl::exception)
{
	cl_int ret;
	cl_mem mem = clCreateBuffer(context,CL_MEM_READ_WRITE,size,NULL,&ret);
	if(ret != CL_SUCCESS)
		throw cl::cl_exception("clCreateBuffer",ret);
	return mem;
}

cl::buffer_object::buffer_object(cl_context context, size_t size) throw(cl::exception)
	: proto_buffer_object(create_cl_buffer(context,size),size)
{
	
}

cl::buffer_object::buffer_object(const context &c_context, size_t c_size) throw(cl::exception)
	: buffer_object(c_context.id(),c_size)
{
	
}

cl::buffer_object::~buffer_object()
{
	clReleaseMemObject(get_cl_mem());
}

void cl::buffer_object::acquire() const
{
	
}

void cl::buffer_object::release() const
{
	
}

