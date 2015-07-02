#pragma once

#include <CL/cl.h>

#include "globals.hpp"
#include "exception.hpp"
#include "queue.hpp"
#include "buffer_object.hpp"
#include "work_range.hpp"

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
	
	void set_mem_arg(cl_uint pos, cl_mem mem) throw(exception)
	{
		cl_int ret;
		ret = clSetKernelArg(_kernel,pos,sizeof(cl_mem),reinterpret_cast<void*>(&mem));
		if(ret != CL_SUCCESS)
		{
			throw cl_exception("clSetKernelArg",ret);
		}
	}
	
	template <typename T>
	void set_var_arg(cl_uint pos, T var) throw(exception)
	{
		cl_int ret;
		ret = clSetKernelArg(_kernel,pos,sizeof(T),reinterpret_cast<void*>(&var));
		if(ret != CL_SUCCESS)
		{
			throw cl_exception("clSetKernelArg",ret);
		}
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
	
	template <typename ... Args>
	static void unroll_args(kernel *self, cl_uint count, buffer_object *buf_obj, Args ... args) throw(exception)
	{
		self->set_mem_arg(count,buf_obj->get_cl_mem());
		unroll_args(self,count+1,args...);
	}
	
	template <typename ... Args>
	static void unroll_args(kernel *self, cl_uint count, cl_mem mem, Args ... args) throw(exception)
	{
		self->set_mem_arg(count,mem);
		unroll_args(self,count+1,args...);
	}
	
	template <typename T, typename ... Args>
	static void unroll_args(kernel *self, cl_uint count, T var, Args ... args) throw(exception)
	{
		self->set_var_arg(count,var);
		unroll_args(self,count+1,args...);
	}
	
	static void unroll_args(kernel *self, cl_uint count, buffer_object *buf_obj) throw(exception)
	{
		self->set_mem_arg(count,buf_obj->get_cl_mem());
		self->check_args_count(count+1);
	}
	
	static void unroll_args(kernel *self, cl_uint count, cl_mem mem) throw(exception)
	{
		self->set_mem_arg(count,mem);
		self->check_args_count(count+1);
	}
	
	template <typename T>
	static void unroll_args(kernel *self, cl_uint count, T var) throw(exception)
	{
		self->set_var_arg(count,var);
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
	void evaluate(const work_range &range, Args ... args) throw(exception)
	{
		cl_int ret;
		if(_queue == 0)
		{
			char strbuf[0x100];
			snprintf(strbuf,0xff,"kernel '%s' wasn't attached to a valid command queue",_name.data());
			throw exception(strbuf);
		}
		
		unroll_args(this,0,args...);
		ret = 
			clEnqueueNDRangeKernel(
				_queue,_kernel,
				range.get_dim(),range.get_offset(),
				range.get_global_size(),range.get_local_size(),
				0,NULL,&_event
			);
		if(ret != CL_SUCCESS)
			throw cl_exception("clEnqueueNDRangeKernel",ret);
	}
	
	cl_ulong measure_time()
	{
		cl_ulong time_start, time_end;
		
		clWaitForEvents(1,&_event);
		clGetEventProfilingInfo(_event, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
		clGetEventProfilingInfo(_event, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
		
		return time_end - time_start;
	}
};
}
