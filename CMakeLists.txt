cmake_minimum_required(VERSION 2.6)
project(cl4u)

include_directories(include)

set(SOURCE 
	source/utility.cpp
	source/exception.cpp
	source/context.cpp
	source/platform.cpp
	source/queue.cpp
)

add_library(${PROJECT_NAME} STATIC ${SOURCE})
