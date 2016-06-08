#pragma once

#include "opencl.hpp"

#include "platform.hpp"
#include "features.hpp"

namespace cl {
class device {
private:
	platform *_platform;
	cl_device_id _device_id;
	int _features = 0;
	
	device() : device(nullptr, 0) {}
	device(platform *p, cl_device_id device_id, int features = 0)
		: _platform(p), _device_id(device_id), _features(features) {}
public:
	platform *get_platform();
	int features() const;
	std::string info(cl_device_info device_info);
	cl_device_id id() const;
public:
	static int get_count(platform *p, cl_device_type t = CL_DEVICE_TYPE_ALL);
	static std::vector<cl_device_id> get_ids(platform *p, cl_device_type t = CL_DEVICE_TYPE_ALL);
	static std::list<device> get_all(platform *p, cl_device_type t = CL_DEVICE_TYPE_ALL);
	static device get(platform *p, cl_device_type t = CL_DEVICE_TYPE_DEFAULT, int no = 0) throw(exception);
	static std::vector<cl_context_properties> get_gl_properties(platform *p) throw(exception);
#ifndef CL_NO_GL_INTEROP
	static cl_int (*get_gl_function(platform *p))(const cl_context_properties*,cl_gl_context_info,size_t,void*,size_t*) throw(exception);
	static int get_count_gl(platform *p) throw(exception);
	static bool is_gl_interop_supported(platform *p);
	static std::vector<cl_device_id> get_ids_gl(platform *p) throw(exception);
	static std::list<device> get_all_gl(platform *p) throw(exception);
	static device get_gl(platform *p, int no = 0) throw(exception);
#endif
};
}
