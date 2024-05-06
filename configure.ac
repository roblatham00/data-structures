#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([data-structures], [0.1], [robl@mcs.anl.gov])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects silent-rules])
AM_SILENT_RULES([yes])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/rbtree.h])
LT_PREREQ([2.2])
# LT_INIT needs to know about AR
AM_PROG_AR
LT_INIT([dlopen])

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lblosc':

AC_CHECK_LIB([blosc], [blosc_init])
# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdint.h stdlib.h string.h unistd.h])
AC_CHECK_HEADER([blosc.h],
		AC_DEFINE(HAVE_BLOSC_H),
		AC_MSG_WARN([unable to find blosc header])
)

AM_CONDITIONAL([COMPARRAY], [test "x$ac_cv_header_blosc_h" = xyes ])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_INT64_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([clock_gettime])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
