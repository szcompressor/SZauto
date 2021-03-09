AUTOMAKE_OPTIONS=foreign
AM_CPPFLAGS = -I../include -I../zstd

bin_PROGRAMS=sz_autotuning_double sz_autotuning_test sz_compress_test

sz_autotuning_double_SOURCES=sz_autotuning_double.cpp
sz_autotuning_double_LDADD=../.libs/libSZauto.a ../zstd/.libs/libzstd.a -lm
sz_autotuning_test_SOURCES=sz_autotuning_test.cpp
sz_autotuning_test_LDADD=../.libs/libSZauto.a ../zstd/.libs/libzstd.a -lm
sz_compress_test_SOURCES=sz_compress_test.cpp
sz_compress_test_LDADD=../.libs/libSZauto.a ../zstd/.libs/libzstd.a -lm
