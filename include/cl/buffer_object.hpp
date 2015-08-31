#pragma once

#include <string>
#include <exception>

#include "exception.hpp"
#include "context.hpp"
#include "proto_buffer_object.hpp"

namespace cl
{
class buffer_object : public proto_buffer_object
{
private:
	static cl_mem create_cl_buffer(cl_context context, size_t size) throw(cl::exception);

public:
	buffer_object(cl_context context, size_t size) throw(cl::exception);
	buffer_object(const context &c_context, size_t c_size) throw(cl::exception);
	virtual ~buffer_object();
	
	virtual void acquire() const override;
	virtual void release() const override;
};
}
