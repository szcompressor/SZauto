add_executable (sz_test sz_compress_test.cpp)
target_include_directories (sz_test PRIVATE "/Users/xin/utils/sz/include")
target_include_directories (sz_test PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries (sz_test sz_cpp)
