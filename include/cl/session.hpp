#pragma once

#include <cstdio>
#include <map>

#include "opencl.hpp"

#include "exception.hpp"
#include "platform.hpp"
#include "context.hpp"
#include "queue.hpp"

#include "kernel.hpp"
#include "buffer_object.hpp"

namespace cl
{
class session
{
private:
	platform _platform;
	device   _device;
	context  _context;

	queue _queue;
	
public:
	session(int platform_no = 0) throw(exception) :
		_platform(platform::get(platform_no)),
		_device(device::get(&_platform)),
		_context(&_device),
		_queue(_context, _device)
	{
		printf("CL_PLATFORM_VERSION: %s\n", _platform.info(CL_PLATFORM_VERSION).c_str());
		printf("CL_PLATFORM_NAME: %s\n",    _platform.info(CL_PLATFORM_NAME).c_str());
		printf("CL_PLATFORM_VENDOR: %s\n",  _platform.info(CL_PLATFORM_VENDOR).c_str());
		fflush(stdout);
	}
	~session()
	{
		
	}
	
	platform &get_platform()
	{
		return _platform;
	}
	
	device &get_device()
	{
		return _device;
	}
	
	context &get_context()
	{
		return _context;
	}
	
	queue &get_queue()
	{
		return _queue;
	}
};
}
