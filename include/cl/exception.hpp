#pragma once

#include <CL/cl.h>

#include <string>
#include <exception>

#include "utility.hpp"

namespace cl
{

class exception : public std::exception
{
private:
	std::string msg;
	
public:
	exception(const char *message) noexcept;
	exception(const std::string &message) noexcept;
	const char *get_message() const noexcept;
	virtual const char *what() const noexcept override;
};

class cl_exception : public exception
{
private:
	cl_int ret;
	std::string func;
	
public:
	cl_exception(const char *func_name, cl_int error_code) noexcept;
	cl_exception(const std::string &func_name, cl_int error_code) noexcept;
	cl_int get_code() const noexcept;
	const char *get_func_name() noexcept;
};
}
