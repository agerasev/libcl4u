#include <cl/kernel.hpp>

#include <cstdio>
#include <cmath>

void cl::kernel::check_args_count(cl_uint count) throw(exception)
{
	if(count != _arg_count)
	{
		char strbuf[0x100];
		snprintf(strbuf,0xff,"kernel '%s' takes %u arguments, %u given",_name.data(),_arg_count,count);
		throw(exception(strbuf));
	}
}

cl::kernel::kernel(cl_program program, const std::string &name) throw(cl::exception)
	: _name(name)
{
	cl_int ret;
	
	_kernel = clCreateKernel(program, name.data(), &ret);
	if(ret != CL_SUCCESS) 
		throw cl_exception("clCreateKernel",ret);
	
	ret = clGetKernelInfo(_kernel,CL_KERNEL_NUM_ARGS,sizeof(cl_uint),&_arg_count,NULL);
	if(ret != CL_SUCCESS) 
		throw cl_exception("clGetKernelInfo",ret);
}

cl::kernel::~kernel() throw(cl::exception)
{
	cl_int ret;
	ret = clReleaseKernel(_kernel);
	if(ret != CL_SUCCESS)
		throw cl_exception("clReleaseKernel",ret);
}

const char *cl::kernel::get_name() const
{
	return _name.data();
}

cl_kernel cl::kernel::get_cl_kernel() const
{
	return _kernel;
}

cl_event cl::kernel::get_cl_event() const
{
	return _event;
}

void cl::kernel::bind_queue(cl_command_queue __queue)
{
	_queue = __queue;
}

void cl::kernel::bind_queue(const queue &__queue)
{
	bind_queue(__queue.get_cl_command_queue());
}

size_t cl::kernel::get_work_group_size() const
{
	cl_int ret;
	size_t value;
	ret = clGetKernelWorkGroupInfo(
	      _kernel, 0, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &value, NULL
	      );
	if(ret != CL_SUCCESS) 
		throw cl_exception("clGetKernelWorkGroupInfo",ret);
	return value;
}

size_t cl::kernel::get_work_group_multiple() const
{
	cl_int ret;
	size_t value;
	ret = clGetKernelWorkGroupInfo(
	      _kernel, 0, CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, sizeof(size_t), &value, NULL
	      );
	if(ret != CL_SUCCESS)
		throw cl_exception("clGetKernelWorkGroupInfo",ret);
	return value;
}

void cl::kernel::check_range(const work_range &range) const
{
	// check local size
	size_t max_size = get_work_group_size();
	size_t size = 1;
	for(size_t n : range.get_local_size())
	{
		size *= n;
	}
	if(size > max_size)
		throw(cl::exception("local size > max local size = " + std::to_string(max_size)));
	
	// check global size
	for(size_t n : range.get_global_size())
		if(n == 0)
			throw(cl::exception("global size component == 0"));
}

void cl::kernel::determine_range(work_range &range) const
{
	int dim = range.get_dim();
	std::vector<size_t> wg = range.get_local_size();
	std::vector<size_t> gs = range.get_global_size();
	
	// determine work group size unless it specified
	size_t n = get_work_group_size();
	size_t d = 1;
	int fd = 0;
	for(int i = 0; i < dim; ++i)
	{
		if(wg[i] > 0)
		{
			++fd;
			d *= wg[i];
		}
	}
	
	fd = dim - fd;
	if(fd)
	{
		size_t q = round(pow(double(n)/d, 1.0/fd));
		
		size_t qg = 1;
		do
		{
			for(int i = 0; i < fd; ++i)
				qg *= q;
		}
		while(qg*d > n && (qg = 1, --q));
		
		for(int i = 0; i < dim; ++i)
			if(wg[i] == 0) wg[i] = q;
	}
	
	// minimize size set preferred multiple 
	//size_t m = get_work_group_multiple();
	for(int i = 0; i < dim; ++i)
	{
		if(gs[i] < wg[i])
		{
			wg[i] = gs[i];
		}
	}
	/*
	if(dim == 1)
	{
		wg[0] = ((wg[0] - 1)/m + 1)*m;
	}
	*/
	
	/*
	printf("local size = {");
	for(int i = 0; i < dim; ++i)
		printf("%ld, ", wg[i]);
	printf("}\n");
	*/
	
	range.set_local_size(wg.data());
	
	// ceil global size to be multiple of work group
	for(int i = 0; i < dim; ++i)
	{
		gs[i] = ceil(double(gs[i])/wg[i])*wg[i];
	}
	
	/*
	printf("global size = {");
	for(int i = 0; i < dim; ++i)
		printf("%ld, ", gs[i]);
	printf("}\n");
	*/
	
	range.set_global_size(gs.data());
	
	//fflush(stdout);
}

#ifndef CL4U_NO_PROFILING

cl_ulong cl::kernel::measure_time()
{
	cl_ulong time_start, time_end;
	
	clWaitForEvents(1,&_event);
	clGetEventProfilingInfo(_event, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
	clGetEventProfilingInfo(_event, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
	
	return time_end - time_start;
}

cl_ulong cl::kernel::get_time() const
{
	return _time;
}

void cl::kernel::clear_time()
{
	_time = 0;
}

#endif // CL4U_NO_PROFILING
