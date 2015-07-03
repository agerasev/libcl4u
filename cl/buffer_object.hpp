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
	cl_command_queue _queue = 0;
	cl_mem mem;
	size_t size;
	
public:
	buffer_object(cl_context c_context, size_t c_size) throw(cl::exception)
	  : size(c_size)
	{
		cl_int ret;
		mem = clCreateBuffer(c_context,CL_MEM_READ_WRITE,size,NULL,&ret);
		if(ret != CL_SUCCESS)
			throw cl::cl_exception("clCreateBuffer",ret);
	}
	buffer_object(const context &c_context, size_t c_size) throw(cl::exception)
	  : buffer_object(c_context.get_cl_context(),c_size)
	{
		
	}
	virtual ~buffer_object()
	{
		clReleaseMemObject(mem);
	}
	
	virtual void bind_queue(cl_command_queue queue) override
	{
		_queue = queue;
	}
	
	virtual cl_mem get_cl_mem() const override
	{
		return mem;
	}
	
	virtual void acquire() override
	{
		
	}
	
	virtual void release() override
	{
		
	}
	
	size_t get_size() const noexcept
	{
		return size;
	}
	
	void load_data(void *data, size_t offset, size_t length) const throw(exception)
	{
		cl_int ret;
		if(_queue == 0)
			throw exception("Buffer hasn't bound to queue");
		ret = clEnqueueReadBuffer(_queue,mem,CL_TRUE,offset,length,data,0,NULL,NULL);
		if(ret != CL_SUCCESS)
			throw cl_exception("clEnqueueReadBuffer",ret);
	}
	
	void load_data(void *data, size_t length) const throw(exception)
	{
		load_data(data,0,length);
	}
	
	void load_data(void *data) const throw(exception)
	{
		load_data(data,size);
	}
	
	void store_data(const void *data, size_t offset, size_t length) throw(exception)
	{
		cl_int ret;
		if(_queue == 0)
			throw exception("Buffer hasn't bound to queue");
		ret = clEnqueueWriteBuffer(_queue,mem,CL_TRUE,offset,length,data,0,NULL,NULL);
		if(ret != CL_SUCCESS)
			throw cl_exception("clEnqueueWriteBuffer",ret);
	}
	
	void store_data(const void *data, size_t length) throw(exception)
	{
		store_data(data,0,length);
	}
	
	void store_data(const void *data) throw(exception)
	{
		store_data(data,size);
	}
};
}
