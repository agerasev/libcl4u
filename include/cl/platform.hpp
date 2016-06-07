#pragma once

#include <string>
#include <list>
#include <vector>
#include <functional>

#include "opencl.hpp"

#include "exception.hpp"

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
	
	std::string info(cl_platform_info platform_info);
	cl_platform_id id() const;
public:
	static int get_count();
	static std::vector<cl_platform_id> get_ids();
	static std::list<platform> get_all();
	static platform get(int no = 0) throw(exception);
};
}
