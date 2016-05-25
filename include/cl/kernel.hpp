#pragma once

#include "opencl.hpp"

#include "exception.hpp"
#include "queue.hpp"
#include "memory_object.hpp"
#include "work_range.hpp"

#include <list>
#include <type_traits>

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
	
	cl_uint _count = 0;
	cl_ulong _time = 0;
	
	struct unroll_data
	{
		std::string kernel_name;
		std::list<const memory_object *> lock_list;
		int arg_count;
	};
	
	template <typename T>
	typename std::enable_if<!std::is_convertible<T, const memory_object *>::value, void>::type 
	  set_arg(cl_uint pos, unroll_data &data, T var)
	{
		cl_int ret;
		ret = clSetKernelArg(_kernel,pos,sizeof(T),reinterpret_cast<void*>(&var));
		if(ret != CL_SUCCESS)
			throw cl_exception("clSetKernelArg(" + data.kernel_name + "," + std::to_string(int(pos)) + ")", ret);
	}
	
	template <typename T>
	typename std::enable_if<std::is_convertible<T, const memory_object *>::value, void>::type 
	  set_arg(cl_uint pos, unroll_data &data, T var)
	{
		const memory_object *mem_obj = static_cast<const memory_object *>(var);
		data.lock_list.push_back(mem_obj);
		set_arg(pos,data,mem_obj->get_cl_mem());
	}
	
	void check_args_count(cl_uint count) throw(exception);
	
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
	
	void check_range(const work_range &range) const;
	void determine_range(work_range &range) const;
	
private:
	cl_ulong measure_time();
	
public:
	cl_ulong get_time() const;
	cl_uint get_count() const;
	void clear_counter();
	
public:
	kernel(cl_program program, const std::string &name) throw(cl::exception);
	virtual ~kernel() throw(cl::exception);
	
	const char *get_name() const;
	cl_kernel get_cl_kernel() const;
	cl_event get_cl_event() const;
	
	void bind_queue(cl_command_queue __queue);
	void bind_queue(const queue &__queue);
	
	size_t get_work_group_size() const;
	size_t get_work_group_multiple() const;
	
	template <typename ... Args>
	kernel *evaluate(work_range range, Args ... args) throw(exception)
	{
		cl_int ret;
		if(_queue == 0)
		{
			char strbuf[0x100];
			snprintf(strbuf,0xff,"kernel '%s' wasn't attached to a valid command queue",_name.data());
			throw exception(strbuf);
		}
		
		check_range(range);
		determine_range(range);
		
		unroll_data data;
		data.kernel_name = get_name();
		unroll_args(this,0,data,args...);
		for(const memory_object *mo : data.lock_list)
			mo->acquire();
		
		ret = 
			clEnqueueNDRangeKernel(
				_queue,_kernel,
				range.get_dim(),range.get_offset().data(),
				range.get_global_size().data(),range.get_local_size().data(),
				0,NULL,&_event
			);
		
		for(const memory_object *mo : data.lock_list)
			mo->release();
		
		if(ret != CL_SUCCESS)
			throw cl_exception("clEnqueueNDRangeKernel",ret);
		
		++_count;
		_time += measure_time();
		
		return this;
	}
};
}
