AC_DEFUN([AC_PROG_BISON], [
BISON_REQUIRED_VERSION="$1"

AC_PROG_YACC
if test "$YACC" != "bison -y"; then
  AC_SUBST(BISON,[])
  AC_MSG_ERROR([Cannot find bison; bison parser generator is needed.])
fi

BISON_VERSION=`$YACC --version | head -n 1 | cut -f 4 -d ' '`
AX_COMPARE_VERSION([$BISON_VERSION], [lt], [$BISON_REQUIRED_VERSION],
  AC_MSG_ERROR([Cannot find bison version $BISON_REQUIRED_VERSION or later. A newer version is needed.]))
AC_SUBST(BISON,[$YACC])
])
