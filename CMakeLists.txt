cmake_minimum_required (VERSION 3.0)
project(SZauto VERSION "1.0")
set (SZauto_VERSION_MAJOR ${CMAKE_PROJECT_VERSION_MAJOR})
set (SZauto_VERSION_MINOR ${CMAKE_PROJECT_VERSION_MINOR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

#correct was to set a default build type
# https://blog.kitware.com/cmake-and-the-default-build-type/
set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "No build type was set. Setting build type to ${default_build_type}.")
  set(CMAKE_BUILD_TYPE ${default_build_type} CACHE 
    STRING "Choose the type to build" FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()

include(GNUInstallDirs)
include(CheckCXXCompilerFlag)
include(CTest)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
option(BUILD_SHARED_LIBS "build testsz as a shared library" ON)

check_cxx_compiler_flag("-fno-omit-frame-pointer" HAVE_NO_OMIT_FRAME_POINTER)
set(NO_OMIT_FRAME_POINTER_FLAG "")
if(HAVE_NO_OMIT_FRAME_POINTER)
  set(NO_OMIT_FRAME_POINTER_FLAG "-fno-omit-frame-pointer")
endif()



find_package(PkgConfig)
pkg_search_module(ZSTD IMPORTED_TARGET libzstd)

add_subdirectory (src)

if(BUILD_TESTING)
  add_subdirectory (test)
endif()

