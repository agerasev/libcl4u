#include <cl/work_range.hpp>

#include <cmath>

#define WORK_SIZE_1D 64//256
#define WORK_SIZE_2D 8//16
#define WORK_SIZE_3D 4//8

void cl::work_range::init(size_t size)
{
	dim = size;
	offset.resize(dim);
	global_size.resize(dim);
	local_size.resize(dim);
	
	size_t lsize = 1;
	switch(dim)
	{
	case 1:
		lsize = WORK_SIZE_1D;
		break;
	case 2:
		lsize = WORK_SIZE_2D;
		break;
	case 3:
		lsize = WORK_SIZE_3D;
		break;
	}
	
	for(int i = 0; i < int(dim); ++i)
	{
		offset[i] = 0;
		local_size[i] = lsize;
	}
}

void cl::work_range::set_global_size(const size_t *data)
{
	for(int i = 0; i < int(dim); ++i)
	{
		global_size[i] = size_t(ceil(double(data[i])/local_size[i]))*local_size[i];
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
