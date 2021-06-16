# Minimum CMake required
cmake_minimum_required(VERSION 3.1.3)

set (projName "GL_R")

# Project
project(${projName} C CXX)

SET(NV_CMAKE_DIR "${NV_SOURCE_DIR}/cmake")
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${NV_CMAKE_DIR}")

# Add c++11 flags
if (CYGWIN) 
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
else()
  set(CMAKE_CXX_STANDARD 11)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
endif()

# Options
option(BUILD_EXAMPLES "Build examples" OFF)
option(SHARED_LIBS "Build shares" ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_DEBUG_POSTFIX _d)

#out dir
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}/Debug/bin)   
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR}/Release/bin)

include(CMakeDependentOption)

set(DEBUG_POSTFIX "d" CACHE STRING "Default debug postfix")
mark_as_advanced(DEBUG_POSTFIX)

# User options
include(cmake/options.cmake)

if (SHARED_LIBS)
  set(SHARED_OR_STATIC "SHARED")
else (SHARED_LIBS)
  set(SHARED_OR_STATIC "STATIC")
endif(SHARED_LIBS)  

if (MSVC) 
  # Build with multiple processes
  add_definitions(/MP)
  # MSVC warning suppressions
  add_definitions(
    /wd4018 # 'expression' : signed/unsigned mismatch
    /wd4065 # switch statement contains 'default' but no 'case' labels
    /wd4146 # unary minus operator applied to unsigned type, result still unsigned
    /wd4244 # 'conversion' conversion from 'type1' to 'type2', possible loss of data
    /wd4251 # 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
    /wd4267 # 'var' : conversion from 'size_t' to 'type', possible loss of data
    /wd4305 # 'identifier' : truncation from 'type1' to 'type2'
    /wd4307 # 'operator' : integral constant overflow
    /wd4309 # 'conversion' : truncation of constant value
    /wd4334 # 'operator' : result of 32-bit shift implicitly converted to 64 bits (was 64-bit shift intended?)
    /wd4355 # 'this' : used in base member initializer list
    /wd4506 # no definition for inline function 'function'
    /wd4800 # 'type' : forcing value to bool 'true' or 'false' (performance warning)
    /wd4996 # The compiler encountered a deprecated declaration.
	/wd4819  
  ) 

endif (MSVC)  
set(Engine_SOURCE_DIR ${PROJECT_SOURCE_DIR}/)

#set evn resource path 
include (cmake/resouce.cmake)
include_directories(
  ${Engine_SOURCE_DIR}/src
  )

include (cmake/marco.cmake)
include (cmake/FindFMOD.cmake)
include (cmake/FindOpenEXR.cmake)
include (cmake/FindAssimp.cmake)
include (cmake/FindGLEW.cmake)

find_package(GLEW REQUIRED)
find_package(zlib REQUIRED)  
find_package(jpeg REQUIRED)
find_package(png REQUIRED)
find_package(freetype REQUIRED)


include(cmake/engine.cmake)   
if(BUILD_EXAMPLES)
	include(cmake/examples.cmake)  
endif()
include(cmake/test.cmake)

