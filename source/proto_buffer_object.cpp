#include <cl/proto_buffer_object.hpp>

cl::proto_buffer_object::proto_buffer_object(cl_mem mem, size_t size) throw(cl::exception)
	: _mem(mem), _size(size)
{
	
}

cl::proto_buffer_object::~proto_buffer_object()
{
	
}

void cl::proto_buffer_object::bind_queue(cl_command_queue queue)
{
	_queue = queue;
}

cl_command_queue cl::proto_buffer_object::get_queue() const
{
	return _queue;
}

cl_mem cl::proto_buffer_object::get_cl_mem() const
{
	return _mem;
}

size_t cl::proto_buffer_object::get_size() const noexcept
{
	return _size;
}

void cl::proto_buffer_object::load_data(void *data, size_t offset, size_t length) const throw(exception)
{
	cl_int ret;
	if(_queue == 0)
		throw exception("Buffer hasn't bound to queue");
	ret = clEnqueueReadBuffer(_queue,_mem,CL_TRUE,offset,length,data,0,NULL,NULL);
	if(ret != CL_SUCCESS)
		throw cl_exception("clEnqueueReadBuffer",ret);
}

void cl::proto_buffer_object::load_data(void *data, size_t length) const throw(exception)
{
	load_data(data,0,length);
}

void cl::proto_buffer_object::load_data(void *data) const throw(exception)
{
	load_data(data,_size);
}

void cl::proto_buffer_object::store_data(const void *data, size_t offset, size_t length) throw(exception)
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

void cl::proto_buffer_object::store_data(const void *data, size_t length) throw(exception)
{
	store_data(data,0,length);
}

void cl::proto_buffer_object::store_data(const void *data) throw(exception)
{
	store_data(data,_size);
}
