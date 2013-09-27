AC_DEFUN([AC_PROG_ESIDL], [
AC_CHECK_PROG(ESIDL, [esidl], [esidl], [no])
if test "$ESIDL" = "no"; then
  AC_MSG_ERROR([Cannot find esidl; esidl Web IDL compiler is needed.])
fi
ESIDL_VERSION=`esidl --version | head -n 1 | grep -Eo '[[0-9]]+\.[[0-9]]+\.[[0-9]]+'`
AX_COMPARE_VERSION([$ESIDL_VERSION], [lt], [$1],
  AC_MSG_ERROR([Cannot find esidl version $1 or later. A newer version is needed.]))
AC_SUBST(ESIDL,[$ESIDL])
])
