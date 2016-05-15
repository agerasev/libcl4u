cmake_minimum_required(VERSION 2.6)
project(cl4u)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall")

include_directories(include)
include_directories(c-includer)

set(SOURCE 
	source/utility.cpp
	source/exception.cpp
	source/context.cpp
	source/platform.cpp
	source/queue.cpp
	source/work_range.cpp
	source/buffer_object.cpp
	source/kernel.cpp
	source/program.cpp
	source/proto_buffer_object.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCE})
target_link_libraries(${PROJECT_NAME} OpenCL)
