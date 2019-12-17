cmake_minimum_required (VERSION 3.0)
project(sz)
set (sz_VERSION_MAJOR 1)
set (sz_VERSION_MINOR 0)
set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_FLAGS "-g -O3")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

find_library (ZSTD_LIBS zstd HINTS "/Users/kzhao/Documents/Workspace/zstd-1.4.2/install/lib")
set (ZSTD_INCLUDES "/Users/kzhao/Documents/Workspace/zstd-1.4.2/install/include")
add_subdirectory (src)
add_subdirectory (test)

