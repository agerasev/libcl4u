#pragma once

#include "context.hpp"

#ifdef CL_GL_INTEROP

#include <CL/cl_gl.h>

#ifdef __gnu_linux__
#include <GL/glx.h>
#endif

namespace cl
{
class gl_context : public context
{
public:
	gl_context(const cl_platform_id &platform_id) throw(exception)
	{
#ifdef __gnu_linux__
		cl_int err;
		
		// Create CL context properties, add GLX context & handle to DC
		cl_context_properties properties[] = {
			CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(), // GLX Context
			CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(), // GLX Display
			CL_CONTEXT_PLATFORM, (cl_context_properties)platform_id, // OpenCL platform
			0
		};
	
		// Find CL capable devices in the current GL context
		std::vector<cl_device_id> device_ids;
		size_t size;
		void *func = clGetExtensionFunctionAddress("clGetGLContextInfoKHR");
		if(func == nullptr)
			throw exception("Extension function 'clGetGLContextInfoKHR' address is NULL");
		
		((cl_int(*)(const cl_context_properties*,cl_gl_context_info,size_t,void*,size_t*))func)
				(properties, CL_DEVICES_FOR_GL_CONTEXT_KHR, 0, nullptr, &size);
		int count = size / sizeof(cl_device_id);
		if(count < 1)
			throw exception("Cannot find OpenCL supporting device for OpenGL context for current platform");
		
		device_ids.resize(count,0);
		
		((cl_int(*)(const cl_context_properties*,cl_gl_context_info,size_t,void*,size_t*))func)
				(properties, CL_DEVICES_FOR_GL_CONTEXT_KHR, size, device_ids.data(), nullptr);
		
		_context = clCreateContext(properties, count, device_ids.data(), nullptr, 0, &err);
		if(err != CL_SUCCESS)
			throw cl_exception("clCreateContext",err);
#else
#error "Other than GLX contexts isn't supported yet"
#endif // __gnu_linux__
	}
};
}
#endif
