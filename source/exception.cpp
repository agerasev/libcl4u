#include <cl/context.hpp>
#include <cl/utility.hpp>

cl::exception::exception(const char *message) noexcept
	: msg(message)
{
	
}

cl::exception::exception(const std::string &message) noexcept
	: msg(message)
{
	
}

const char *cl::exception::get_message() const noexcept
{
	return msg.data();
}

const char *cl::exception::what() const noexcept
{
	return msg.data();
}

cl::cl_exception::cl_exception(const char *func_name, cl_int error_code) noexcept
  : cl_exception(std::string(func_name), error_code)
{
	
}

cl::cl_exception::cl_exception(const std::string &func_name, cl_int error_code) noexcept
  : exception(((func_name + " : ") + get_code_name(error_code)).data()), 
		ret(error_code), func(func_name)
{
	
}

cl_int cl::cl_exception::get_code() const noexcept
{
	return ret;
}

const char *cl::cl_exception::get_func_name() noexcept
{
	return func.data();
}

