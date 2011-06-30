dnl Process this file with autoconf to produce a configure script
AC_INIT([es-www], [0.1.5])
AM_INIT_AUTOMAKE([no-define])
AC_PROG_YACC
AC_PROG_CXX
AC_PROG_CC
AM_PROG_AS
AC_PROG_RANLIB
AC_CONFIG_HEADER(config.h)
AC_CONFIG_FILES(Makefile)

AH_BOTTOM([#include <Object.h>])

# check for icu-config
AC_PATH_PROG(icu_config, icu-config, no)
if test "$icu_config" = "no"; then
	AC_MSG_ERROR([Cannot find icu-config. ICU library is needed.])
fi
ICU_CPPFLAGS=`$icu_config --cppflags`
ICU_LIBS=`$icu_config --ldflags`
AC_SUBST([ICU_CPPFLAGS])
AC_SUBST([ICU_LIBS])

# check for freetype-config
AC_PATH_PROG(freetype_config, freetype-config, no)
if test "$freetype_config" = "no"; then
	AC_MSG_ERROR([Cannot find freetype-config. FreeType library is needed.])
fi
FREETYPE_CPPFLAGS=`$freetype_config --cflags`
FREETYPE_LIBS=`$freetype_config --libs`
AC_SUBST([FREETYPE_CPPFLAGS])
AC_SUBST([FREETYPE_LIBS])

AC_OUTPUT

