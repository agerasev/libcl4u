#pragma once

#include <vector>

#include "opencl.hpp"

#include "exception.hpp"

namespace cl
{
class context
{
protected:
	cl_context _context;
	
	context() = default;
	
public:
	context(cl_device_id dev) throw(exception);
	context(const std::vector<cl_device_id> &vdev);
	~context();
	cl_context get_cl_context() const;
};
}
