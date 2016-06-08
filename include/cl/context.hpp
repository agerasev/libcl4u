#pragma once

#include <vector>

#include "opencl.hpp"

#include "exception.hpp"
#include "device.hpp"

namespace cl {
class context {
private:
	device *_device = nullptr;
	cl_context _context = 0;
	
public:
	context() : context(nullptr, 0) {}
	context(device *dev, cl_context ctx) : _device(dev), _context(ctx) {}
	context(device *dev) throw(exception) : _device(dev) {
		cl_int err;
		cl_device_id devid = dev->id();
#ifndef CL_NO_GL_INTEROP
		if(dev->features() & feature::GL_INTEROP) {
			_context = clCreateContext(device::get_gl_properties(dev->get_platform()).data(), 1, &devid, nullptr, nullptr, &err);
		} else {
#else
		{
#endif
			_context = clCreateContext(nullptr, 1, &devid, nullptr, nullptr, &err);
		}
		if(err != CL_SUCCESS)
			throw cl_exception("clCreateContext",err);
	}
	
	~context() {
		if(_context != 0) {
			clReleaseContext(_context);
		}
	}
	
	cl_context id() const {
		return _context;
	}
	
	device *get_device() {
		return _device;
	}
};
}
