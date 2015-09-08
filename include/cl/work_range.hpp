#pragma once

#include <CL/cl.h>

#include <vector>
#include <initializer_list>
#include <type_traits>

namespace cl
{
class work_range
{
private:
	size_t dim;
	std::vector<size_t> offset;
	std::vector<size_t> global_size;
	std::vector<size_t> local_size;
	
	void init(size_t dim);
	
	void create(std::vector<size_t> &vec);
	template <typename T>
	void create(std::initializer_list<T> list)
	{
		static_assert(std::is_integral<T>::value, "initializer list components must be an integral type");
		std::vector<size_t> vec;
		for(T n : list)
		{
			vec.push_back(static_cast<size_t>(n));
		}
		create(vec);
	}
	
	template <typename T>
	void append(std::vector<size_t> &vec, T arg)
	{
		static_assert(std::is_integral<T>::value, "argument must be an integral type");
		vec.push_back(static_cast<size_t>(arg));
	}
	
	template <typename T>
	void unroll(std::vector<size_t> &vec, T arg)
	{
		append(vec, arg);
	}
	
	template <typename T, typename ... Args>
	void unroll(std::vector<size_t> &vec, T arg, Args ... args)
	{
		append(vec, arg);
		unroll(vec, args ...);
	}
	
	template <typename T, typename ... Args>
	typename std::enable_if<!std::is_integral<T>::value, void>::type 
	  probe(T var)
	{
		create(var);
	}
	
	template <typename T, typename ... Args>
	typename std::enable_if<std::is_integral<T>::value, void>::type
	  probe(T var)
	{
		std::vector<size_t> vec;
		unroll(vec, var);
		create(vec);
	}
	
	template <typename T, typename ... Args>
	void probe(T var, Args ... args)
	{
		std::vector<size_t> vec;
		unroll(vec, var, args ...);
		create(vec);
	}
	
public:
	template <typename ... Args>
	work_range(Args ... args)
	{
		probe(args ...);
	}
	
	work_range(const work_range &range) = default;
	~work_range() = default;
	
	work_range &operator = (const work_range &range) = default;
	
	size_t get_dim() const;
	const std::vector<size_t> &get_offset() const;
	const std::vector<size_t> &get_global_size() const;
	const std::vector<size_t> &get_local_size() const;
	
	void set_offset(const size_t *data);
	void set_global_size(const size_t *data);
	void set_local_size(const size_t *data);
};
}
