#pragma once

#include <CL/cl.h>

#include "globals.hpp"

#include "exception.hpp"
#include "platform.hpp"
#include "context.hpp"
#ifdef CL_GL_INTEROP
#include "gl_context.hpp"
#endif
#include "queue.hpp"

namespace cl
{
class session
{
private:
	platform _platform;
	cl_device_id _device_id;
#ifdef CL_GL_INTEROP
	gl_context _context;
#else
	context _context;
#endif
	queue _queue;
	
public:
	session() throw(exception) :
	  _platform(),
	  _device_id(_platform.get_default_device()),
#ifdef CL_GL_INTEROP
	  _context(_platform.get_id()),
#else // CL_GL_INTEROP
	  _context(_device_id),
#endif // CL_GL_INTEROP
	  _queue(_context,_device_id)
	{
		
	}
	~session()
	{
		
	}
	
	platform &get_platform()
	{
		return _platform;
	}
	
	cl_device_id get_device_id()
	{
		return _device_id;
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
