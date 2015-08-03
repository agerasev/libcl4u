#pragma once

#include "memory_object.hpp"

namespace cl
{
class proto_buffer_object : public memory_object
{
private:
	cl_command_queue _queue = 0;
	cl_mem _mem;
	size_t _size;
	
public:
	proto_buffer_object(cl_mem mem, size_t size) throw(cl::exception)
	  : _mem(mem), _size(size)
	{
		
	}
	
	virtual ~proto_buffer_object()
	{
		
	}
	
	virtual void bind_queue(cl_command_queue queue) override
	{
		_queue = queue;
	}
	
	virtual cl_command_queue get_queue() const override
	{
		return _queue;
	}
	
	virtual cl_mem get_cl_mem() const override
	{
		return _mem;
	}
	
	size_t get_size() const noexcept
	{
		return _size;
	}
	
	void load_data(void *data, size_t offset, size_t length) const throw(exception)
	{
		cl_int ret;
		if(_queue == 0)
			throw exception("Buffer hasn't bound to queue");
		ret = clEnqueueReadBuffer(_queue,_mem,CL_TRUE,offset,length,data,0,NULL,NULL);
		if(ret != CL_SUCCESS)
			throw cl_exception("clEnqueueReadBuffer",ret);
	}
	
	void load_data(void *data, size_t length) const throw(exception)
	{
		load_data(data,0,length);
	}
	
	void load_data(void *data) const throw(exception)
	{
		load_data(data,_size);
	}
	
	void store_data(const void *data, size_t offset, size_t length) throw(exception)
	{
		cl_int ret;
		if(_queue == 0)
			throw exception("Buffer hasn't bound to queue");
		acquire();
		ret = clEnqueueWriteBuffer(_queue,_mem,CL_TRUE,offset,length,data,0,NULL,NULL);
		release();
		if(ret != CL_SUCCESS)
			throw cl_exception("clEnqueueWriteBuffer",ret);
	}
	
	void store_data(const void *data, size_t length) throw(exception)
	{
		store_data(data,0,length);
	}
	
	void store_data(const void *data) throw(exception)
	{
		store_data(data,_size);
	}
};
}
