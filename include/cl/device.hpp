#pragma once

#include "opencl.hpp"

#include "platform.hpp"

namespace cl {
class device {
public:
	static const int
	GL_INTEROP = (1 << 0);
	
private:
	platform *_platform;
	cl_device_id _device_id;
	int _features = 0;
	
	device() : device(nullptr, 0) {}
	device(platform *p, cl_device_id device_id, int features = 0)
		: _platform(p), _device_id(device_id), _features(features) {}
public:
	platform *get_platform() {
		return _platform;
	}
	int features() const {
		return _features;
	}
	std::string info(cl_device_info device_info) {
		size_t size;
		clGetDeviceInfo(_device_id, device_info, 0, nullptr, &size);
		std::vector<char> data(size);
		clGetDeviceInfo(_device_id, device_info, size, data.data(), nullptr);
		return std::string(data.data());
	}
	cl_device_id id() const {
		return _device_id;
	}
public:
	static int get_count(platform *p, cl_device_type t = CL_DEVICE_TYPE_ALL) {
		cl_uint count;
		clGetDeviceIDs(p->id(), t, 0, nullptr, &count);
		return count;
	}
	
	static std::vector<cl_device_id> get_ids(platform *p, cl_device_type t = CL_DEVICE_TYPE_ALL) {
		int count = get_count(p, t);
		std::vector<cl_device_id> ids(count);
		clGetDeviceIDs(p->id(), t, count, ids.data(), nullptr);
		return ids;
	}
	
	static std::list<device> get_all(platform *p, cl_device_type t = CL_DEVICE_TYPE_ALL) {
		std::list<device> devices;
		for(cl_device_id id : get_ids(p, t)) {
			devices.push_back(device(p, id));
		}
		return devices;
	}
	
	static device get(platform *p, cl_device_type t = CL_DEVICE_TYPE_DEFAULT, int no = 0) throw(exception) {
		int count = get_count(p, t);
		if(count < 1)
			throw exception("Cannot find OpenCL supporting device for current platform");
		std::vector<cl_device_id> ids = get_ids(p, t);
		if(no < count)
			return device(p, ids[no]);
		return device(p, ids[0]);
	}
	
	static std::vector<cl_context_properties> get_gl_properties(platform *p) throw(exception) {
		std::vector<cl_context_properties> props;
#ifdef __gnu_linux__
		// Create CL context properties, add GLX context & handle to DC
		props = std::vector<cl_context_properties>({
			CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(),  // GLX Context
			CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(), // GLX Display
			CL_CONTEXT_PLATFORM, (cl_context_properties)p->id(),               // OpenCL platform
			0
		});
#elif _WIN32
		// Create CL context properties, add WGL context & handle to DC
		props = std::vector<cl_context_properties>({
			CL_GL_CONTEXT_KHR,   (cl_context_properties)wglGetCurrentContext(), // WGL Context
			CL_WGL_HDC_KHR,      (cl_context_properties)wglGetCurrentDC(),      // WGL HDC
			CL_CONTEXT_PLATFORM, (cl_context_properties)p->id(),                // OpenCL platform
			0
		});
#else
		throw exception("Other than GLX and WGL contexts isn't supported yet");
#endif
		return props;
	}
	
	static cl_int (*get_gl_function(platform *p))(const cl_context_properties*,cl_gl_context_info,size_t,void*,size_t*) throw(exception) {
		void *func = clGetExtensionFunctionAddressForPlatform(p->id(), "clGetGLContextInfoKHR");
		if(func == nullptr)
			throw exception("Extension function 'clGetGLContextInfoKHR' address is NULL for current platform");
		return (cl_int(*)(const cl_context_properties*,cl_gl_context_info,size_t,void*,size_t*)) func;
	}
	
	static int get_count_gl(platform *p) throw(exception) {
		size_t size;
		get_gl_function(p)(get_gl_properties(p).data(), CL_DEVICES_FOR_GL_CONTEXT_KHR, 0, nullptr, &size);
		int count = size / sizeof(cl_device_id);
		return count;
	}
	
	static bool is_gl_interop_supported(platform *p) {
		try {
			if(get_count_gl(p) > 0)
				return true;
		} catch(exception) {}
		return false;
	}
	
	static std::vector<cl_device_id> get_ids_gl(platform *p) throw(exception) {
		int count = get_count_gl(p);
		std::vector<cl_device_id> ids(count);
		get_gl_function(p)(get_gl_properties(p).data(), CL_DEVICES_FOR_GL_CONTEXT_KHR, count, ids.data(), nullptr);
		return ids;
	}
	
	static std::list<device> get_all_gl(platform *p) throw(exception) {
		std::vector<cl_device_id> ids = get_ids_gl(p);
		std::list<device> devices;
		for(cl_device_id id : ids) {
			devices.push_back(device(p, id, GL_INTEROP));
		}
		return devices;
	}
	
	static device get_gl(platform *p, int no = 0) throw(exception) {
		int count = get_count_gl(p);
		if(count < 1)
			throw exception("Cannot find OpenCL-OpenGL Interoperability supporting device for current platform");
		std::vector<cl_device_id> ids = get_ids_gl(p);
		if(no < count)
			no = 0;
		return device(p, ids[no], GL_INTEROP);
	}
};
}
