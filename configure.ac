dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_INIT([libdi],[0.1.0-dev])

AC_CONFIG_HEADERS([config.h])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([aux-bin])

AM_INIT_AUTOMAKE([1.11])

dnl AM_SILENT_RULES([yes])

AC_PROG_CXX

AX_CXX_COMPILE_STDCXX_11

AC_PROG_LIBTOOL

LT_INIT([dlopen])
dnl LTDL_INIT


BOOST_REQUIRE
BOOST_SYSTEM
BOOST_PROGRAM_OPTIONS
BOOST_FILESYSTEM


AC_CONFIG_FILES([
Makefile
src/Makefile
tests/Makefile
])
AC_OUTPUT
