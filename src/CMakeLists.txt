add_library (sz_cpp SHARED 
	sz_compress_3d.cpp
	sz_compression_utils.cpp
	sz_decompress_3d.cpp
	sz_decompression_utils.cpp)
target_include_directories(sz_cpp PRIVATE "/Users/xin/utils/sz/include")
target_include_directories(sz_cpp PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(sz_cpp ${SZ_LIBS} ${ZSTD_LIBS} ${GZIP_LIBS})
file(GLOB header_files ${PROJECT_SOURCE_DIR}/include/*.hpp)

install(FILES ${header_files} DESTINATION ${PROJECT_BINARY_DIR}/include)
install (TARGETS sz_cpp
  LIBRARY DESTINATION ${PROJECT_BINARY_DIR}/lib
  ARCHIVE DESTINATION ${PROJECT_BINARY_DIR}/lib
  INCLUDES DESTINATION ${PROJECT_BINARY_DIR}/include
)
