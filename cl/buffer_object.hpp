#pragma once

#include <string>
#include <exception>

#include "exception.hpp"
#include "context.hpp"
#include "proto_buffer_object.hpp"

namespace cl
{
class buffer_object : public proto_buffer_object
{
private:
	static cl_mem create_cl_buffer(cl_context context, size_t size) throw(cl::exception)
	{
		cl_int ret;
		cl_mem mem = clCreateBuffer(context,CL_MEM_READ_WRITE,size,NULL,&ret);
		if(ret != CL_SUCCESS)
			throw cl::cl_exception("clCreateBuffer",ret);
		return mem;
	}

public:
	buffer_object(cl_context context, size_t size) throw(cl::exception)
	  : proto_buffer_object(create_cl_buffer(context,size),size)
	{
		
	}
	buffer_object(const context &c_context, size_t c_size) throw(cl::exception)
	  : buffer_object(c_context.get_cl_context(),c_size)
	{
		
	}
	virtual ~buffer_object()
	{
		clReleaseMemObject(get_cl_mem());
	}
	
	virtual void acquire() override
	{
		
	}
	
	virtual void release() override
	{
		
	}
};
}
