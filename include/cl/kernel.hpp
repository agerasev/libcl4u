#pragma once

#include <CL/cl.h>

#include "exception.hpp"
#include "queue.hpp"
#include "memory_object.hpp"
#include "work_range.hpp"

#include <list>
#include <type_traits>

#include <cstdio>
#include <string>

namespace cl
{
class kernel
{
private:
	cl_kernel _kernel;
	std::string _name;
	cl_uint _arg_count;
	cl_event _event;
	cl_command_queue _queue = 0;
	
	struct unroll_data
	{
		std::list<memory_object*> lock_list;
		int arg_count;
	};
	
	template <typename T>
	typename std::enable_if<!std::is_convertible<T,memory_object*>::value,void>::type 
	  set_arg(cl_uint pos, unroll_data &data, T var)
	{
		cl_int ret;
		ret = clSetKernelArg(_kernel,pos,sizeof(T),reinterpret_cast<void*>(&var));
		if(ret != CL_SUCCESS)
			throw cl_exception("clSetKernelArg",ret);
	}
	
	template <typename T>
	typename std::enable_if<std::is_convertible<T,memory_object*>::value,void>::type 
	  set_arg(cl_uint pos, unroll_data &data, T var)
	{
		memory_object *mem_obj = static_cast<memory_object*>(var);
		data.lock_list.push_back(mem_obj);
		set_arg(pos,data,mem_obj->get_cl_mem());
	}
	
	void check_args_count(cl_uint count) throw(exception)
	{
		if(count != _arg_count)
		{
			char strbuf[0x100];
			snprintf(strbuf,0xff,"kernel '%s' takes %u arguments, %u given",_name.data(),_arg_count,count);
			throw(exception(strbuf));
		}
	}
	
	template <typename T, typename ... Args>
	static void unroll_args(kernel *self, cl_uint count, unroll_data &data, T var, Args ... args) throw(exception)
	{
		self->set_arg(count,data,var);
		unroll_args(self,count+1,data,args...);
	}
	
	template <typename T>
	static void unroll_args(kernel *self, cl_uint count, unroll_data &data, T var) throw(exception)
	{
		self->set_arg(count,data,var);
		self->check_args_count(count+1);
	}
	
public:
	kernel(cl_program program, const std::string &name) throw(cl::exception)
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
	
	virtual ~kernel() throw(cl::exception)
	{
		cl_int ret;
		ret = clReleaseKernel(_kernel);
		if(ret != CL_SUCCESS)
			throw cl_exception("clReleaseKernel",ret);
	}
	
	const char *get_name() const
	{
		return _name.data();
	}
	
	cl_kernel get_cl_kernel() const
	{
		return _kernel;
	}
	
	cl_event get_cl_event() const
	{
		return _event;
	}
	
	void bind_queue(cl_command_queue __queue)
	{
		_queue = __queue;
	}
	
	void bind_queue(const queue &__queue)
	{
		bind_queue(__queue.get_cl_command_queue());
	}
	
	template <typename ... Args>
	kernel *evaluate(const work_range &range, Args ... args) throw(exception)
	{
		cl_int ret;
		if(_queue == 0)
		{
			char strbuf[0x100];
			snprintf(strbuf,0xff,"kernel '%s' wasn't attached to a valid command queue",_name.data());
			throw exception(strbuf);
		}
		
		unroll_data data;
		unroll_args(this,0,data,args...);
		for(memory_object *mo : data.lock_list)
			mo->acquire();
		
		ret = 
			clEnqueueNDRangeKernel(
				_queue,_kernel,
				range.get_dim(),range.get_offset(),
				range.get_global_size(),range.get_local_size(),
				0,NULL,&_event
			);
		
		for(memory_object *mo : data.lock_list)
			mo->release();
		
		if(ret != CL_SUCCESS)
			throw cl_exception("clEnqueueNDRangeKernel",ret);
		
		return this;
	}
	
	cl_ulong measure_time()
	{
		cl_ulong time_start, time_end;
		
		clWaitForEvents(1,&_event);
		clGetEventProfilingInfo(_event, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
		clGetEventProfilingInfo(_event, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
		
		return time_end - time_start;
	}
	
	void print_time()
	{
		printf("%s: %0.3f ms\n",get_name(),measure_time()/1000000.0);
	}
};
}
