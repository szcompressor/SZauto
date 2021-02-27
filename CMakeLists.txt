cmake_minimum_required (VERSION 3.0)
project(SZauto)
set (SZauto_VERSION_MAJOR 1)
set (SZauto_VERSION_MINOR 0)
set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_FLAGS "-g -O3")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
add_definitions(-DPOLY_REGRESSION_COEF_FILE="${CMAKE_CURRENT_SOURCE_DIR}/data/PolyRegressionCoefAux3D.f32")
#add_definitions(-DDUMP_PREDICTION_ERR_FOLDER="$ENV{HOME}/Documents/Workspace/sz-kai/autotunning/highorder/prediction_err/")

find_library(ZSTD_LIBS zstd HINTS "$ENV{HOME}/tools/zstd-1.4.4/install/lib")
set(ZSTD_INCLUDES "$ENV{HOME}/tools/zstd-1.4.4/install/include")
add_subdirectory (src)
add_subdirectory (test)

file(GLOB header_files ${PROJECT_SOURCE_DIR}/include/*.hpp)
install(FILES ${header_files} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/szauto)
install (TARGETS sz_cpp EXPORT szauto
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        )

install (EXPORT szauto DESTINATION ${CMAKE_INSTALL_PREFIX}/share/szauto/cmake)
