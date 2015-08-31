#pragma once

#include "exception.hpp"
#include "proto_buffer_object.hpp"

// #ifdef CL_GL_INTEROP

#include <CL/cl_gl.h>
#include <GL/glew.h>

namespace cl
{
class gl_buffer_object : public proto_buffer_object
{
private:
	GLuint _gl_buffer_id;
	
	static cl_mem create_cl_gl_buffer(GLuint *_gl_buffer_id, cl_context context, size_t size) throw(cl::exception)
	{
		// Create a buffer object in OpenGL and allocate space
		glGenBuffers(1, _gl_buffer_id);
		glBindBuffer(GL_ARRAY_BUFFER_ARB, *_gl_buffer_id);
		
		// Note: specify GL_STATIC_DRAW_ARB to modify outside of GL
		glBufferData(GL_ARRAY_BUFFER_ARB, size, NULL, GL_STATIC_DRAW_ARB);
		
		// Note: could use colors, normals, etc
		glVertexPointer(4, GL_FLOAT, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);
		
		// Create a reference cl_mem object from GL buffer object
		cl_int err;
		cl_mem mem = clCreateFromGLBuffer(context, CL_MEM_READ_WRITE, *_gl_buffer_id, &err);
		if(err != CL_SUCCESS)
			throw cl_exception("clCreateFromGLBuffer",err);
		
		return mem;
	}
	
public:
	gl_buffer_object(cl_context context, size_t size) throw(cl::exception)
	  : proto_buffer_object(create_cl_gl_buffer(&_gl_buffer_id,context,size),size)
	{
		
	}
	gl_buffer_object(const context &c_context, size_t c_size) throw(cl::exception)
	  : gl_buffer_object(c_context.get_cl_context(),c_size)
	{
		
	}
	virtual ~gl_buffer_object()
	{
		clReleaseMemObject(get_cl_mem());
		
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &_gl_buffer_id);
	}
	
	GLuint get_gl_buffer() const
	{
		return _gl_buffer_id;
	}
	
	virtual void acquire() const override
	{
		if(get_queue() == 0)
			throw exception("GL Image hasn't bound to a valid queue");
		cl_mem mem = get_cl_mem();
		clEnqueueAcquireGLObjects(get_queue(), 1, &mem, 0, 0, 0);
	}

	virtual void release() const override
	{
		if(get_queue() == 0)
			throw exception("GL Image hasn't bound to a valid queue");
		cl_mem mem = get_cl_mem();
		clEnqueueReleaseGLObjects(get_queue(), 1, &mem, 0, 0, 0);
	}
};
}

// #endif // CL_GL_INTEROP
