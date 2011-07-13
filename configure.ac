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

# check for SpiderMonkey 1.8.5
AC_SEARCH_LIBS(JS_NewCompartmentAndGlobalObject, js mozjs185)

# check for liberation font
AC_CHECK_FILE(/usr/share/fonts/liberation/LiberationSerif-Regular.ttf,[AC_SUBST(LIBERATON_TTF,/usr/share/fonts/liberation)])
AC_CHECK_FILE(/usr/share/fonts/truetype/ttf-liberation/LiberationSerif-Regular.ttf,[AC_SUBST(LIBERATON_TTF,/usr/share/fonts/truetype/ttf-liberation)])

# check for IPA fonts on Fedora
AC_CHECK_FILE(/usr/share/fonts/ipa-gothic/ipag.ttf,[AC_SUBST(HAVE_IPA_GOTHIC,/usr/share/fonts/ipa-gothic/ipag.ttf)])
AC_CHECK_FILE(/usr/share/fonts/ipa-mincho/ipam.ttf,[AC_SUBST(HAVE_IPA_MINCHO,/usr/share/fonts/ipa-mincho/ipam.ttf)])
AC_CHECK_FILE(/usr/share/fonts/ipa-pgothic/ipagp.ttf,[AC_SUBST(HAVE_IPA_PGOTHIC,/usr/share/fonts/ipa-pgothic/ipagp.ttf)])
AC_CHECK_FILE(/usr/share/fonts/ipa-pmincho/ipamp.ttf,[AC_SUBST(HAVE_IPA_PMINCHO,/usr/share/fonts/ipa-pmincho/ipamp.ttf)])

# check for IPA fonts on Ubuntu
AC_CHECK_FILE(/usr/share/fonts/truetype/takao/TakaoGothic.ttf,[AC_SUBST(HAVE_IPA_GOTHIC,/usr/share/fonts/truetype/takao/TakaoGothic.ttf)])
AC_CHECK_FILE(/usr/share/fonts/truetype/takao/TakaoMincho.ttf,[AC_SUBST(HAVE_IPA_MINCHO,/usr/share/fonts/truetype/takao/TakaoGothic.ttf)])
AC_CHECK_FILE(/usr/share/fonts/truetype/takao/TakaoPGothic.ttf,[AC_SUBST(HAVE_IPA_PGOTHIC,/usr/share/fonts/truetype/takao/TakaoPGothic.ttf)])
AC_CHECK_FILE(/usr/share/fonts/truetype/takao/TakaoPMincho.ttf,[AC_SUBST(HAVE_IPA_PMINCHO,/usr/share/fonts/truetype/takao/TakaoPMincho.ttf)])

AC_OUTPUT

