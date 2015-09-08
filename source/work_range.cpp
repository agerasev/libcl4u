#include <cl/work_range.hpp>

#include <cmath>

void cl::work_range::init(size_t size)
{
	dim = size;
	offset.resize(dim);
	global_size.resize(dim);
	local_size.resize(dim);
	
	for(int i = 0; i < int(dim); ++i)
	{
		offset[i] = 0;
		global_size[i] = 0;
		local_size[i] = 0;
	}
}

void cl::work_range::create(std::vector<size_t> &vec)
{
	init(vec.size());
	set_global_size(vec.data());
}

size_t cl::work_range::get_dim() const
{
	return dim;
}

const std::vector<size_t> &cl::work_range::get_offset() const
{
	return offset;
}

const std::vector<size_t> &cl::work_range::get_global_size() const
{
	return global_size;
}

const std::vector<size_t> &cl::work_range::get_local_size() const
{
	return local_size;
}

void cl::work_range::set_offset(const size_t *data)
{
	for(int i = 0; i < int(dim); ++i)
	{
		offset[i] = data[i];
	}
}

void cl::work_range::set_global_size(const size_t *data)
{
	for(int i = 0; i < int(dim); ++i)
	{
		global_size[i] = data[i];
	}
}

void cl::work_range::set_local_size(const size_t *data)
{
	for(int i = 0; i < int(dim); ++i)
	{
		local_size[i] = data[i];
	}
}
