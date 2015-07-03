#pragma once

#include <CL/cl.h>

namespace cl
{
class memory_object
{
public:
	memory_object() = default;
	virtual ~memory_object() = default;
	
	virtual void bind_queue(cl_command_queue queue) = 0;
	
	virtual cl_mem get_cl_mem() const = 0;
	virtual void acquire() = 0;
	virtual void release() = 0;
};
}
