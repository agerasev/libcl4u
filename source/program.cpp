#include <cl/program.hpp>

#include <regex>

#include <cstdlib>
#include <iostream>

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

static std::string __load_source(const std::string &fn) throw(cl::exception)
{
	FILE *fp;
	size_t size;
	char *source = NULL;
	
	fp = fopen(fn.data(),"r");
	if(!fp)
		throw cl::exception(std::string("Cannot open file '") + fn + std::string("'"));
	
	fseek(fp,0,SEEK_END);
	size = ftell(fp);
	fseek(fp,0,SEEK_SET);
	
	source = (char*)malloc(sizeof(char)*(size+1));
	fread(source,1,size,fp);
	source[size] = '\0';
	
	fclose(fp);
	
	std::string ret(source);
	free(source);
	
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

cl::program::program(const std::string &filename, cl_context context, cl_device_id device_id) throw(exception)
{
	cl_int err;
	
	std::string source = __load_source(filename);
	
	const char *source_data = source.data();
	size_t size = source.size();
	
	// create program
	_program = clCreateProgramWithSource(context, 1, &source_data, &size, &err);
	if(err != CL_SUCCESS)
		throw cl_exception("clCreateProgramWithSource",err);
	
#ifndef CL_NO_PARSING
	std::vector<std::string> kernel_names = __find_kernels_in_source(source);
#endif // CL_NO_PARSING
	
	source.clear();
	
	
	// build program
	err = clBuildProgram(_program, 1, &device_id, nullptr, nullptr, nullptr);
	if(err != CL_SUCCESS)
	{
		throw build_exception(__get_kernel_compilation_info(_program,device_id));
	}
	
#ifndef CL_NO_PARSING
	for(const std::string &name : kernel_names)
		_kernel_map.insert(name,new cl::kernel(_program,name));
#endif
}

cl::program::~program()
{
	for(kernel *k : _kernel_map)
		delete k;
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

