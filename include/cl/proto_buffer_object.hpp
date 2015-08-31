#pragma once

#include "exception.hpp"
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
	proto_buffer_object(cl_mem mem, size_t size) throw(cl::exception);
	virtual ~proto_buffer_object();
	
	virtual void bind_queue(cl_command_queue queue) override;
	virtual cl_command_queue get_queue() const override;
	
	virtual cl_mem get_cl_mem() const override;
	
	size_t get_size() const noexcept;
	
	void load_data(void *data, size_t offset, size_t length) const throw(exception);
	void load_data(void *data, size_t length) const throw(exception);
	void load_data(void *data) const throw(exception);
	void store_data(const void *data, size_t offset, size_t length) throw(exception);
	void store_data(const void *data, size_t length) throw(exception);
	void store_data(const void *data) throw(exception);
};
}
