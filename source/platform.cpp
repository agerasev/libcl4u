#include <cl/platform.hpp>

using namespace cl;

std::string platform::info(cl_platform_info platform_info) {
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

cl_platform_id platform::id() const {
	return _platform_id;
}

int platform::get_count() {
	cl_uint count;
	clGetPlatformIDs(0, nullptr, &count);
	return count;
}
std::vector<cl_platform_id> platform::get_ids() {
	int count = get_count();
	std::vector<cl_platform_id> ids(count);
	clGetPlatformIDs(count, ids.data(), nullptr);
	return ids;
}
std::list<platform> platform::get_all() {
	int count = get_count();
	std::vector<cl_platform_id> ids = get_ids();
	
	std::list<platform> platforms;
	for(int i = 0; i < count; ++i) {
		platforms.push_back(platform(ids[i]));
	}
	return platforms;
}
platform platform::get(int no) throw(exception) {
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
