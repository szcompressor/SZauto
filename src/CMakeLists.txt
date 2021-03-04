add_library (sz_cpp 
	sz_compression_utils.cpp
	sz_compress_3d.cpp
	sz_autotuning_3d.cpp
	sz_decompression_utils.cpp
	sz_decompress_3d.cpp
	sz_huffman.cpp
	sz_lossless.cpp)
target_compile_features(sz_cpp
  PUBLIC
  cxx_std_14
  )
target_compile_definitions(sz_cpp PUBLIC 
  $<BUILD_INTERFACE:POLY_REGRESSION_COEF_FILE="${PROJECT_SOURCE_DIR}/data/PolyRegressionCoefAux3D.f32">
  $<INSTALL_INTERFACE:POLY_REGRESSION_COEF_FILE="${CMAKE_INSTALL_DATADIR}/szauto/PolyRegressionCoefAux3D.f32">
  )
target_include_directories(
  sz_cpp 
  PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/szauto>
  )
target_compile_options(sz_cpp PRIVATE 
  $<$<CONFIG:Debug>: -Wall -Wextra -Wpedantic>
  $<$<CONFIG:RelWithDebInfo>: ${NO_OMIT_FRAME_POINTER_FLAG}>
  )

target_link_libraries(
  sz_cpp PUBLIC PkgConfig::ZSTD
  )

install(TARGETS sz_cpp EXPORT szautoConfig
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(FILES 
  ${PROJECT_SOURCE_DIR}/data/PolyRegressionCoefAux1D.f32
  ${PROJECT_SOURCE_DIR}/data/PolyRegressionCoefAux2D.f32
  ${PROJECT_SOURCE_DIR}/data/PolyRegressionCoefAux3D.f32
  DESTINATION ${CMAKE_INSTALL_DATADIR}/szauto)
install(EXPORT szautoConfig NAMESPACE szauto:: DESTINATION ${CMAKE_INSTALL_DATADIR}/szauto/cmake)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/szauto)
