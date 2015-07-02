#pragma once

#include <CL/cl.h>

static void __printKernelCompilationInfo(cl_program program, cl_device_id device_id)
{
	fprintf(stderr,"clBuildProgram failed\n");
	size_t length;
	clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &length);
	char *buffer = (char*)malloc(sizeof(char)*length);
	clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, length, buffer, NULL);
	fprintf(stderr,"CL_PROGRAM_BUILD_LOG: \n%s\n",buffer);
	free(buffer);
}

static cl_mem __get_image(cl_context context)
{
	cl_int err;
#ifdef RAY_GL
	// Create shared image 
	GLenum 
	  gl_texture_target = GL_TEXTURE_2D,
	  gl_texture_internal = GL_RGBA32F,
	  gl_texture_format = GL_RGBA,
	  gl_texture_type = GL_FLOAT;
	
	// Create a texture in OpenGL and allocate space
	glGenTextures(1, &gl_texture_id);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(gl_texture_target, gl_texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(gl_texture_target, 0, gl_texture_internal, width, height, 0, gl_texture_format, gl_texture_type, NULL);
	glBindTexture(gl_texture_target, 0);
	
	// Create a reference mem object in OpenCL from GL texture
	return clCreateFromGLTexture2D(context, CL_MEM_READ_WRITE, gl_texture_target, 0, gl_texture_id, &err);
	if (!cl_image || err != CL_SUCCESS)
	{
		fprintf(stderr,"Failed to create OpenGL texture reference! %d\n", err);
		return (cl_mem)-1;
	}
#else // RAY_GL
	// create cl_image here
	return clCreateImage2D(context,CL_MEM_READ_WRITE,NULL,width,height,1,NULL,&err); //???
#endif // RAY_GL
}
