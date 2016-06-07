#include <cl/device.hpp>

#ifndef CL_NO_GL_INTEROP
#ifdef __gnu_linux__
#include <GL/glx.h>
#elif _WIN32
#include <windows.h>
#include <wingdi.h>
#endif
#endif // CL_NO_GL_INTEROP

using namespace cl;

platform *device::get_platform() {
	return _platform;
}
int device::features() const {
	return _features;
}
std::string device::info(cl_device_info device_info) {
	size_t size;
	clGetDeviceInfo(_device_id, device_info, 0, nullptr, &size);
	std::vector<char> data(size);
	clGetDeviceInfo(_device_id, device_info, size, data.data(), nullptr);
	return std::string(data.data());
}
cl_device_id device::id() const {
	return _device_id;
}

int device::get_count(platform *p, cl_device_type t) {
	cl_uint count;
	clGetDeviceIDs(p->id(), t, 0, nullptr, &count);
	return count;
}

std::vector<cl_device_id> device::get_ids(platform *p, cl_device_type t) {
	int count = get_count(p, t);
	std::vector<cl_device_id> ids(count);
	clGetDeviceIDs(p->id(), t, count, ids.data(), nullptr);
	return ids;
}

std::list<device> device::get_all(platform *p, cl_device_type t) {
	std::list<device> devices;
	for(cl_device_id id : get_ids(p, t)) {
		devices.push_back(device(p, id));
	}
	return devices;
}

device device::get(platform *p, cl_device_type t, int no) throw(exception) {
	int count = get_count(p, t);
	if(count < 1)
		throw exception("Cannot find OpenCL supporting device for current platform");
	std::vector<cl_device_id> ids = get_ids(p, t);
	if(no < count)
		return device(p, ids[no]);
	return device(p, ids[0]);
}

std::vector<cl_context_properties> device::get_gl_properties(platform *p) throw(exception) {
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

cl_int (*device::get_gl_function(platform *p))(const cl_context_properties*,cl_gl_context_info,size_t,void*,size_t*) throw(exception) {
	void *func = clGetExtensionFunctionAddressForPlatform(p->id(), "clGetGLContextInfoKHR");
	if(func == nullptr)
		throw exception("Extension function 'clGetGLContextInfoKHR' address is NULL for current platform");
	return (cl_int(*)(const cl_context_properties*,cl_gl_context_info,size_t,void*,size_t*)) func;
}

int device::get_count_gl(platform *p) throw(exception) {
	cl_uint err;
	size_t size;
	err = get_gl_function(p)(get_gl_properties(p).data(), CL_DEVICES_FOR_GL_CONTEXT_KHR, 0, nullptr, &size);
	if(err != CL_SUCCESS)
		throw cl_exception("clGetGLContextInfoKHR", err);
	int count = size / sizeof(cl_device_id);
	return count;
}

bool device::is_gl_interop_supported(platform *p) {
	try {
		if(get_count_gl(p) > 0)
			return true;
	} catch(exception) {}
	return false;
}

std::vector<cl_device_id> device::get_ids_gl(platform *p) throw(exception) {
	cl_uint err;
	int count = get_count_gl(p);
	std::vector<cl_device_id> ids(count);
	err = get_gl_function(p)(get_gl_properties(p).data(), CL_DEVICES_FOR_GL_CONTEXT_KHR, count*sizeof(cl_context_properties), ids.data(), nullptr);
	if(err != CL_SUCCESS)
		throw cl_exception("clGetGLContextInfoKHR", err);
	return ids;
}

std::list<device> device::get_all_gl(platform *p) throw(exception) {
	std::vector<cl_device_id> ids = get_ids_gl(p);
	std::list<device> devices;
	for(cl_device_id id : ids) {
		devices.push_back(device(p, id, feature::GL_INTEROP));
	}
	return devices;
}

device device::get_gl(platform *p, int no) throw(exception) {
	int count = get_count_gl(p);
	if(count < 1)
		throw exception("Cannot find OpenCL-OpenGL Interoperability supporting device for current platform");
	std::vector<cl_device_id> ids = get_ids_gl(p);
	if(no < count)
		no = 0;
	return device(p, ids[no], feature::GL_INTEROP);
}
