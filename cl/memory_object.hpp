#pragma once

#include <CL/cl.h>

namespace cl
{
class memory_object
{
	virtual cl_mem get_cl_mem() = 0;
	virtual void acquire() = 0;
	virtual void release() = 0;
};
}
