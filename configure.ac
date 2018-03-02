#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(GoboLight, 0.1, hisham@gobolinux.org)
AC_CONFIG_SRCDIR([gobolight.c])
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O

# Checks for libraries.
AC_CHECK_LIB([m], [ceil], [], [missing_libraries="$missing_libraries libm"])

# Checks for header files.
AC_HEADER_STDC

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

AC_ARG_ENABLE(setuid-install, 
   AS_HELP_STRING([--disable-setuid-install       do not set setuid flags during install]),
   [enable_setuid_install=$enableval], [enable_setuid_install="yes"])
AM_CONDITIONAL(SETUID_INSTALL, test x"$enable_setuid_install" = "xyes")

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
