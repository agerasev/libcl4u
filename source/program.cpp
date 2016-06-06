#include <cl/program.hpp>

#include <regex>

#include <cstdlib>
#include <iostream>

#include <includer.hpp>

cl::program::build_exception::build_exception(const std::string &msg)
	: cl::exception(msg.data())
{
	
}

const std::string &cl::program::build_exception::get_build_output() const
{
	return build_output;
}

static std::string __get_kernel_compilation_info(cl_program program, cl_device_id device_id)
{
	size_t length;
	clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &length);
	char *buffer = (char*)malloc(sizeof(char)*length);
	clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, length, buffer, NULL);
	std::string ret(buffer);
	free(buffer);
	return ret;
}

static std::vector<std::string> __find_kernels_in_source(const std::string &source)
{
	std::string string;
	std::smatch match;
	std::regex expr;
	std::vector<std::string> result;

	string = source;
	expr = "\\b(kernel)([ ]*)([^ ]*)([ ]*)([^ \\(]*)";
	while(std::regex_search(string,match,expr))
	{
		result.push_back(match[5]);
		string = match.suffix().str();
	}
	
	string = source;
	expr = "\\b(__kernel)([ ]*)([^ ]*)([ ]*)([^ \\(]*)";
	while(std::regex_search(string,match,expr))
	{
		result.push_back(match[5]);
		string = match.suffix().str();
	}
	
	return result;
}

void cl::program::_free_inc() {
	if(_inc != nullptr) {
		delete static_cast<includer*>(_inc);
		_inc = nullptr;
	}
}

cl::program::program(cl_context context, cl_device_id device_id, const std::string &filename, const std::string &include_dir) throw(exception) {
	cl_int err;
	
	_free_inc();
	_inc = static_cast<void*>(new includer(filename, std::list<std::string>({include_dir})));
	includer &_includer = *static_cast<includer*>(_inc);
	
	const std::string &source = _includer.data();
	
	const char *source_data = source.c_str();
	size_t size = source.size();
	
	// create program
	_program = clCreateProgramWithSource(context, 1, &source_data, &size, &err);
	if(err != CL_SUCCESS)
		throw cl_exception("clCreateProgramWithSource",err);
	
#ifndef CL_NO_PARSING
	std::vector<std::string> kernel_names = __find_kernels_in_source(source);
#endif // CL_NO_PARSING
	
	// build program
	err = clBuildProgram(_program, 1, &device_id, nullptr, nullptr, nullptr);
	if(err != CL_SUCCESS)
	{
		std::string result;
		std::string string(__get_kernel_compilation_info(_program,device_id));
		std::regex expr("<kernel>:(\\d*):(\\d*):");//, line_expr("line ([0123456789]*);");
		std::smatch match;
		while(std::regex_search(string,match,expr))
		{
			int gpos = std::stoi(std::string(match[1]));
			int lpos;
			std::string name;
			if(_includer.locate(gpos, name, lpos)) {
				result += match.prefix().str() + name + ":" + std::to_string(lpos) + ":" + std::string(match[2]) + ":";
			} else {
				fprintf(stderr, "includer: cannot locate line %d\n", gpos);
				result += match.prefix().str() + std::string(match[0]);
			}
			
			string = match.suffix().str();
		}
		result += string;
		
		//std::string result(__get_kernel_compilation_info(_program,device_id));
		
		throw build_exception("compile errors:\n" + result);
	}
	
#ifndef CL_NO_PARSING
	for(const std::string &name : kernel_names)
		_kernel_map.insert(name,new cl::kernel(_program,name));
#endif
}

cl::program::~program()
{
	_free_inc();
	
	for(cl::kernel *k : _kernel_map) {
		delete k;
	}
	
	clReleaseProgram(_program);
}

cl_program cl::program::get_cl_program() const
{
	return _program;
}

cl::map<cl::kernel*> &cl::program::get_kernel_map()
{
	return _kernel_map;
}

cl::kernel *cl::program::get_kernel(const std::string &kname) throw(exception)
{
	return _kernel_map[kname];
}

void cl::program::bind_queue(cl_command_queue queue)
{
	for(kernel *k : _kernel_map)
	{
		k->bind_queue(queue);
	}
}

