#pragma once

#include <vector>

#include "opencl.hpp"

#include "exception.hpp"

namespace cl
{
class platform
{
private:
	cl_platform_id _platform_id;
	
public:
	platform() throw(exception);
	~platform() = default;
	
	cl_platform_id get_id() const;
	cl_device_id get_default_device() throw(exception);
	std::vector<cl_device_id> get_all_devices() throw(exception);
};
}
