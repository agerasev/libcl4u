#include <cl/platform.hpp>

cl::platform::platform() throw(exception)
{
	// Get platform information
	cl_uint num_platforms;
	clGetPlatformIDs(0, nullptr, &num_platforms);
	if(num_platforms < 1)
		throw cl::exception("Cannot find OpenCL supporting platform");
	clGetPlatformIDs(1, &_platform_id, nullptr);
}

cl_platform_id cl::platform::get_id() const
{
	return _platform_id;
}

cl_device_id cl::platform::get_default_device() throw(exception)
{
	// Get device information
	cl_uint num_devices;
	cl_device_id device_id;
	clGetDeviceIDs(_platform_id, CL_DEVICE_TYPE_DEFAULT, 0, nullptr, &num_devices);
	if(num_devices < 1)
		throw exception("Cannot find OpenCL supporting device for current platform");
	clGetDeviceIDs(_platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, nullptr);
	return device_id;
}

std::vector<cl_device_id> cl::platform::get_all_devices() throw(exception)
{
	// Get device information
	cl_uint num_devices;
	std::vector<cl_device_id> device_ids;
	clGetDeviceIDs(_platform_id, CL_DEVICE_TYPE_ALL, 0, nullptr, &num_devices);
	if(num_devices < 1)
		throw exception("Cannot find OpenCL supporting device for current platform");
	device_ids.resize(num_devices,0);
	clGetDeviceIDs(_platform_id, CL_DEVICE_TYPE_ALL, num_devices, device_ids.data(), nullptr);
	return device_ids;
}

