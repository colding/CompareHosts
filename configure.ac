#  
# Copyright (C) 2014 Jules Colding <jcolding@gmail.com>.
#
# All Rights Reserved.
#
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.
#

AC_PREREQ([2.68])
AC_INIT([CompareHosts],[0.0.1],[jcolding@gmail.com])
AC_COPYRIGHT([Copyright (C) 2014 Jules Colding. All Rights Reserved.])

# For GCC extensions
AC_USE_SYSTEM_EXTENSIONS

AM_INIT_AUTOMAKE([subdir-objects])
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])

AC_CONFIG_SRCDIR([bootstrap])
AC_CONFIG_HEADERS([ac_config.h])
AC_CONFIG_MACRO_DIR([m4])

dnl Standard checks
AC_LANG([C])

dnl Initialize libtool
LT_INIT([disable-shared])
AC_SUBST([LIBTOOL_DEPS])

# unset -g -O3 in xxxFLAGS unless the user specified otherwise
AS_IF([test "x${ac_cv_env_CFLAGS_set}" = "x"], [CFLAGS=""])

# Checks for programs.
AC_PROG_CC
AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

# Checks for libraries.

# This macro is defined in check.m4 and tests if check.h and
# libcheck.a are installed in your system.
PKG_CHECK_MODULES([CHECK], [check >= 0.9.8])

# Checks for header files.
AC_CHECK_HEADERS([stdint.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_CHECK_FUNCS([malloc])
AC_FUNC_STRERROR_R

#
# Check implementation details of build host
#

# Check for indianess
AC_C_BIGENDIAN(
	[
		AC_DEFINE([HAVE_BIG_ENDIAN], [1], [machine is big endian])
		endianess="Big Endian"
	],
	[
		AC_DEFINE([HAVE_LITTLE_ENDIAN], [1], [machine is little endian])
		endianess="Little Endian"
	],
	[
		AC_MSG_ERROR([unknown endianness])
	]
)

# Check for (4 <= sizeof(int)) - Assumed in many places
AC_MSG_CHECKING([for (4 <= sizeof(int))])
AC_RUN_IFELSE(
	[AC_LANG_PROGRAM(
		[[
			#include <unistd.h>
		]],
		[[
			return (4 <= sizeof(int)) ? 0 : 1;
		]]
	)],
	[
		AC_MSG_RESULT([ok])
	],
	[
		AC_MSG_RESULT([no])
		AC_MSG_FAILURE([sizeof(int) has to be at least 4 bytes])
	],
	[
		AC_MSG_RESULT([no])
		AC_MSG_FAILURE([sizeof(int) has to be at least 4 bytes])
	]
)

dnl
dnl CompareHosts versions, names and default configuration values
dnl
PROGRAM_NAME=AC_PACKAGE_NAME
AC_SUBST(PROGRAM_NAME)

PROGRAM_VERSION=AC_PACKAGE_VERSION
AC_SUBST(PROGRAM_VERSION)

dnl top-level source directory
PROGRAM_top_dir="`pwd`"
AC_SUBST(PROGRAM_top_dir)

target=`uname`
case "x$target" in
	xDarwin)
	AM_CONDITIONAL(OS_DARWIN, true)
	AM_CONDITIONAL(OS_LINUX, false)
	;;
	xLinux)
	AM_CONDITIONAL(OS_DARWIN, false)
	AM_CONDITIONAL(OS_LINUX, true)
	;;
	*)
	AC_MSG_ERROR([CompareHosts is not supported on this operating system])
	;;
esac

dnl
dnl Configure options
dnl

AC_ARG_ENABLE(debug,
	      [AS_HELP_STRING([--enable-debug[[[[=no/yes]]]]], [Enable CompareHosts debugging [default=yes]])],
	      [],
	      enable_debug=yes)
if test "x$enable_debug" = "xyes"; then
	AC_DEFINE([DEBUG], [1], [Define if you want to make a debug enabled build])
	msg_debug=yes
else
	msg_debug=no
fi
AM_CONDITIONAL(PROGRAM_DEBUG, test "x$enable_debug" = "xyes")


dnl
dnl Program availability checks
dnl


dnl
dnl CPPFLAGS, CFLAGS, LIBS and stuff
dnl

dnl Check for pthread support
AX_PTHREAD(
	[
		AX_NOOP()
	],
	[
		AC_MSG_ERROR([pthreads are required])
	]
)

dnl
dnl LIBS
dnl
LIBS="$PTHREAD_LIBS $LIBS"
AC_SUBST(LIBS)

dnl
dnl Check if the compiler supports -iquote DIR (from ltrace)
dnl
ac_cv_have_iquote=no
AC_MSG_CHECKING(whether compiler accepts -iquote dir)
save_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS -iquote ."
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]], [[]])],[ac_cv_have_iquote=yes],[])
CPPFLAGS="$save_CPPFLAGS"
AC_MSG_RESULT($ac_cv_have_iquote)
if test $ac_cv_have_iquote = yes; then
  iquote="-iquote "
else
  iquote="-I"
fi

dnl Should only be searchable in "" include statements
PROGRAM_INCLUDES="$iquote$PROGRAM_top_dir"

dnl Now set the preprocessor arguments
PROGRAM_CPPFLAGS="$PROGRAM_INCLUDES"
AC_SUBST(PROGRAM_CPPFLAGS)

dnl
dnl CFLAGS
dnl
PROGRAM_CFLAGS="$PTHREAD_CFLAGS"

dnl Workaround for https://bugzilla.redhat.com/show_bug.cgi?id=218377
CC_VERSION=`$CC -dumpversion`
if test "x$CC_VERSION" = "x3.4.6"; then
   AX_CFLAGS_GCC_OPTION(-fno-var-tracking, PROGRAM_CFLAGS)
fi

AX_CFLAGS_GCC_OPTION(-Wall, PROGRAM_CFLAGS)
AX_CFLAGS_GCC_OPTION(-Wextra, PROGRAM_CFLAGS)
AX_CFLAGS_GCC_OPTION(-Wfloat-conversion, PROGRAM_CFLAGS)
AX_CFLAGS_GCC_OPTION(-Werror, PROGRAM_CFLAGS)
AX_CFLAGS_GCC_OPTION(-Wundef, PROGRAM_CFLAGS)
AX_CFLAGS_GCC_OPTION(-Wpointer-arith, PROGRAM_CFLAGS)
AX_CFLAGS_GCC_OPTION(-Wimplicit, PROGRAM_CFLAGS)
AX_CFLAGS_GCC_OPTION(-Wbad-function-cast, PROGRAM_CFLAGS)
AX_CFLAGS_GCC_OPTION(-Wcast-align, PROGRAM_CFLAGS)
AX_CFLAGS_GCC_OPTION(-pipe, PROGRAM_CFLAGS)
AX_CFLAGS_GCC_OPTION(-fprefetch-loop-arrays, PROGRAM_CFLAGS)
AX_CFLAGS_GCC_OPTION(-std=gnu11, PROGRAM_CFLAGS)
AX_CFLAGS_GCC_OPTION(-fno-strict-aliasing, PROGRAM_CFLAGS)

if test "x$enable_debug" = "xyes"; then
	AX_CFLAGS_GCC_OPTION(-ggdb , PROGRAM_CFLAGS)
	AX_CFLAGS_GCC_OPTION(-Og, PROGRAM_CFLAGS)
	
dnl https://code.google.com/p/address-sanitizer/wiki/LeakSanitizer
	AX_CFLAGS_GCC_OPTION(-fsanitize=leak, PROGRAM_CFLAGS)
	AX_CFLAGS_GCC_OPTION(-fsanitize=shift, PROGRAM_CFLAGS)
	AX_CFLAGS_GCC_OPTION(-fsanitize=integer-divide-by-zero, PROGRAM_CFLAGS)
	AX_CFLAGS_GCC_OPTION(-fsanitize=unreachable, PROGRAM_CFLAGS)
	AX_CFLAGS_GCC_OPTION(-fsanitize=vla-bound, PROGRAM_CFLAGS)
	AX_CFLAGS_GCC_OPTION(-fsanitize=null, PROGRAM_CFLAGS)
	AX_CFLAGS_GCC_OPTION(-fsanitize=return, PROGRAM_CFLAGS)
	AX_CFLAGS_GCC_OPTION(-fsanitize=signed-integer-overflow, PROGRAM_CFLAGS)
else
	AX_CFLAGS_GCC_OPTION(-g , PROGRAM_CFLAGS)
	AX_CFLAGS_GCC_OPTION(-O3, PROGRAM_CFLAGS)
fi
if test "x$target" = "xLinux"; then
	PROGRAM_CFLAGS="$PROGRAM_CFLAGS -D__STDC_LIMIT_MACROS"
fi
AC_SUBST(PROGRAM_CFLAGS)

dnl
dnl LDFLAGS
dnl
if test "x$target" = "xDarwin"; then
	LDFLAGS="-flat_namespace $LDFLAGS"
fi
LDFLAGS="-pthread $LDFLAGS"
LDFLAGS="-L$PROGRAM_top_dir/ $LDFLAGS"
AC_SUBST(LDFLAGS)

dnl
dnl Check if this is a distribution
dnl
AM_CONDITIONAL(THIS_IS_NOT_A_DISTRIBUTION, [test -f ./nodist])


dnl ******************************
dnl Makefiles
dnl ******************************

AC_CONFIG_FILES([
	Makefile
	src/Makefile
	cmdline/Makefile
	 ])
AC_OUTPUT

echo "\

	Target Platform:           $target
	Endianess:                 $endianess
	Making a debug build:      $msg_debug
        $PROGRAM_NAME version:      $PACKAGE_VERSION
	PROGRAM_CFLAGS:            $PROGRAM_CFLAGS
	PROGRAM_CPPFLAGS:          $PROGRAM_CPPFLAGS
	$PROGRAM_NAME LIBS:         $LIBS
"
