#AM_CFLAGS = -I./include -I../zlib
#LDFLAGS=-fPIC -shared

AUTOMAKE_OPTIONS=foreign
include_HEADERS=include/sz_autotuning_3d.hpp include/sz_compression_utils.hpp include/sz_decompression_utils.hpp include/sz_optimize_quant_intervals.hpp \
		include/sz_compress_3d.hpp include/sz_decompress_3d.hpp include/sz_def.hpp include/sz_prediction.hpp \
		include/sz_compress_block_processing.hpp include/sz_decompress_block_processing.hpp include/sz_huffman.hpp include/sz_regression_utils.hpp \
		include/sz_compress_block_processing_knl.hpp include/sz_decompress_block_processing_knl.hpp include/sz_lossless.hpp include/sz_utils.hpp

lib_LTLIBRARIES=libSZauto.la
libSZauto_la_CPPFLAGS=-I./include -I../zlib -I../zstd/ 
libSZauto_la_LDFLAGS = -version-info  0:0:0
libSZauto_la_LIDADD=../zstd/.libs/libzstd.a
libSZauto_la_SOURCES=src/sz_autotuning_3d.cpp src/sz_compress_3d.cpp src/sz_compression_utils.cpp src/sz_decompress_3d.cpp \
		 src/sz_decompression_utils.cpp src/sz_huffman.cpp src/sz_lossless.cpp

#libSZauto_la_LINK= $(AM_V_CC)$(LIBTOOL) --tag=CC --mode=link $(CCLD) $(libSZauto_la_CFLAGS) $(libSZauto_la_LDFLAGS) -o $(lib_LTLIBRARIES)

SUBDIRS = zstd
