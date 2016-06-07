#pragma once

#include <string>
#include <list>
#include <vector>
#include <functional>

#include "opencl.hpp"

#include "exception.hpp"

#ifndef CL_NO_GL_INTEROP
#ifdef __gnu_linux__
#include <GL/glx.h>
#elif _WIN32
#include <windows.h>
#include <wingdi.h>
#endif
#endif // CL_NO_GL_INTEROP

namespace cl
{
class platform
{
private:
	cl_platform_id _platform_id;
	
	platform() : _platform_id((cl_platform_id) 0) {}
	platform(cl_platform_id platform_id) : _platform_id(platform_id) {}
	
public:
	~platform() = default;
	
	std::string info(cl_platform_info platform_info) {
		int err;
		
		size_t size;
		err = clGetPlatformInfo(_platform_id, platform_info, 0, nullptr, &size);
		if(err != CL_SUCCESS)
			throw cl_exception("clGetPlatformInfo",err);
		
		std::vector<char> data(size);
		err = clGetPlatformInfo(_platform_id, platform_info, size, data.data(), nullptr);
		if(err != CL_SUCCESS)
			throw cl_exception("clGetPlatformInfo",err);
		
		return std::string(data.data());
	}
	cl_platform_id id() const {
		return _platform_id;
	}
public:
	static int get_count() {
		cl_uint count;
		clGetPlatformIDs(0, nullptr, &count);
		return count;
	}
	static std::vector<cl_platform_id> get_ids() {
		int count = get_count();
		std::vector<cl_platform_id> ids(count);
		clGetPlatformIDs(count, ids.data(), nullptr);
		return ids;
	}
	static std::list<platform> get_all() {
		int count = get_count();
		std::vector<cl_platform_id> ids = get_ids();
		
		std::list<platform> platforms;
		for(int i = 0; i < count; ++i) {
			platforms.push_back(platform(ids[i]));
		}
		return platforms;
	}
	static platform get(int no = 0) throw(exception) {
		int count = get_count();
		if(count < 1)
			throw cl::exception("Cannot find OpenCL supporting platform");
		
		std::vector<cl_platform_id> ids = get_ids();
		cl_platform_id id;
		
		if(no >= count) {
			fprintf(stderr, "no platform at index %d, using 0\n", no);
			id = ids[0];
		} else {
			id = ids[no];
		}
		
		return platform(id);
	}
	
};
}
