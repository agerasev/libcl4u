#pragma once

#include <string>
#include <exception>

#include "exception.hpp"
#include "context.hpp"
#include "memory_object.hpp"

namespace cl
{
class buffer_object : public memory_object
{
private:
	cl_mem mem;
	size_t size;
	
public:
	buffer_object(cl_context c_context, size_t c_size) throw(cl::exception)
	  : size(c_size)
	{
		cl_int ret;
		mem = clCreateBuffer(c_context,CL_MEM_READ_WRITE,size,NULL,&ret);
		if(ret != CL_SUCCESS)
		{
			throw cl::cl_exception("clCreateBuffer",ret);
		}
	}
	buffer_object(const context &c_context, size_t c_size) throw(cl::exception)
	  : buffer_object(c_context.get_cl_context(),c_size)
	{
		
	}
	virtual ~buffer_object() throw(cl::exception)
	{
		cl_int ret;
		ret = clReleaseMemObject(mem);
		if(ret != CL_SUCCESS)
		{
			throw cl::cl_exception("clReleaseMemObject",ret);
		}
	}
	
	virtual cl_mem get_cl_mem()
	{
		return mem;
	}
	
	virtual void acquire()
	{
		
	}
	
	virtual void release()
	{
		
	}
	
	size_t get_size() const noexcept
	{
		return size;
	}
	
	void load_data(cl_command_queue queue, void *data, size_t offset, size_t length) const throw(cl::exception)
	{
		cl_int ret;
		ret = clEnqueueReadBuffer(queue,mem,CL_TRUE,offset,length,data,0,NULL,NULL);
		if(ret != CL_SUCCESS)
		{
			throw cl::cl_exception("clEnqueueReadBuffer",ret);
		}
	}
	
	void load_data(cl_command_queue queue, void *data, size_t length) const throw(cl::exception)
	{
		load_data(queue,data,0,length);
	}
	
	void load_data(cl_command_queue queue, void *data) const throw(cl::exception)
	{
		load_data(queue,data,size);
	}
	
	void store_data(cl_command_queue queue, void *data, size_t offset, size_t length) throw(cl::exception)
	{
		cl_int ret;
		ret = clEnqueueWriteBuffer(queue,mem,CL_TRUE,offset,length,data,0,NULL,NULL);
		if(ret != CL_SUCCESS)
		{
			throw cl::cl_exception("clEnqueueWriteBuffer",ret);
		}
	}
	
	void store_data(cl_command_queue queue, void *data, size_t length) throw(cl::exception)
	{
		store_data(queue,data,0,length);
	}
	
	void store_data(cl_command_queue queue, void *data) throw(cl::exception)
	{
		store_data(queue,data,size);
	}
};
}
