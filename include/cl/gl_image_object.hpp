#pragma once

#include "opencl.hpp"

#include "exception.hpp"
#include "context.hpp"
#include "image_object.hpp"

// #include <CL/cl_gl.h>
#include <GL/gl.h>

namespace cl
{
class gl_image_object : public image_object
{
private:
	cl_command_queue _queue = 0;
	cl_mem _image;
	GLuint _texture;
	
public:
	gl_image_object(cl_context context, int width, int height) throw(exception)
	{
		cl_int err;
		// Create shared image 
		GLenum 
			gl_texture_target = GL_TEXTURE_2D,
			gl_texture_internal = GL_RGBA32F,
			gl_texture_format = GL_RGBA,
			gl_texture_type = GL_FLOAT;
		
		// Create a texture in OpenGL and allocate space
		glGenTextures(1, &_texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(gl_texture_target, _texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(gl_texture_target, 0, gl_texture_internal, width, height, 0, gl_texture_format, gl_texture_type, nullptr);
		glBindTexture(gl_texture_target, 0);
		
		// TODO: free texture if error occured
		
		// Create a reference mem object in OpenCL from GL texture
		_image = clCreateFromGLTexture(context, CL_MEM_READ_WRITE, gl_texture_target, 0, _texture, &err);
		if(err != CL_SUCCESS)
			throw cl_exception("clCreateFromGLTexture",err);
		if(_image == nullptr)
			throw exception("Failed to create OpenGL texture reference");
	}
	gl_image_object(context c_context, int width, int height) throw(exception)
	  : gl_image_object(c_context.get_cl_context(),width,height)
	{
		
	}
	virtual ~gl_image_object()
	{
		clReleaseMemObject(_image);
		glDeleteTextures(1,&_texture);
	}
	
	GLuint get_texture() const
	{
		return _texture;
	}
	
	virtual void bind_queue(cl_command_queue queue) override
	{
		_queue = queue;
	}
	
	virtual cl_command_queue get_queue() const override
	{
		return _queue;
	}
	
	virtual cl_mem get_cl_mem() const override
	{
		return _image;
	}
	
	virtual void acquire() const override
	{
		if(_queue == 0)
			throw exception("GL Image hasn't bound to a valid queue");
		clEnqueueAcquireGLObjects(_queue, 1, &_image, 0, 0, 0);
	}

	virtual void release() const override
	{
		if(_queue == 0)
			throw exception("GL Image hasn't bound to a valid queue");
		clEnqueueReleaseGLObjects(_queue, 1, &_image, 0, 0, 0);
	}
};
}
