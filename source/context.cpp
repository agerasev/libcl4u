#include <cl/context.hpp>

cl::context::context(cl_device_id dev) throw(exception)
{
	cl_int err;
	_context = clCreateContext(nullptr, 1, &dev, nullptr, nullptr, &err);
	if(err != CL_SUCCESS)
		throw cl_exception("clCreateContext",err);
}

cl::context::context(const std::vector<cl_device_id> &vdev)
{
	cl_int err;
	_context = clCreateContext(nullptr, vdev.size(), vdev.data(), nullptr, nullptr, &err);
	if(err != CL_SUCCESS)
		throw cl_exception("clCreateContext",err);
}

cl::context::~context()
{
	clReleaseContext(_context);
}

cl_context cl::context::get_cl_context() const
{
	return _context;
}

