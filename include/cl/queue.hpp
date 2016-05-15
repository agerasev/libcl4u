#pragma once

#include "opencl.hpp"

#include "context.hpp"

namespace cl
{
class queue
{
private:
	cl_command_queue _queue;
	
public:
	queue(cl_context __context, cl_device_id __device_id) throw(cl_exception);
	queue(const context &__context, cl_device_id __device_id) throw(cl_exception);
	~queue();
	
	void flush();
	cl_command_queue get_cl_command_queue() const;
};
}
