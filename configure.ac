#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(SZauto, 0.0, sdi1@anl.gov)
AM_INIT_AUTOMAKE([foreign -Wall -Werror subdir-objects])
AC_CONFIG_SRCDIR([src/sz_autotuning_3d.cpp])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_LANG(C++)
AC_PROG_CXX
AC_PROG_CC

AC_PROG_RANLIB
AM_PROG_AR
AC_PROG_LIBTOOL
#m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
: ${CFLAGS=-O3 -std=c99 -Wall}
: ${CXXFLAGS=-O3 -Wall}

AM_MAINTAINER_MODE([disable])

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h limits.h malloc.h stddef.h stdlib.h string.h sys/time.h unistd.h])

#

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memchr memset strchr strtol])

AC_CONFIG_FILES([Makefile
		 zstd/Makefile
		 test/Makefile])
AC_OUTPUT
