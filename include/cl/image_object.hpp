#pragma once

#include <CL/cl.h>

#include "memory_object.hpp"

namespace cl
{
class image_object : public memory_object
{
public:
	image_object() = default;
	virtual ~image_object() = default;
};
}
