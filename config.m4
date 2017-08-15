dnl $Id$
dnl config.m4 for extension wing_process

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(wing_process, for wing_process support,
dnl Make sure that the comment is aligned:
dnl [  --with-wing_process             Include wing_process support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(wing_process, whether to enable wing_process support,
dnl Make sure that the comment is aligned:
dnl [  --enable-wing_process           Enable wing_process support])

if test "$PHP_WING_PROCESS" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-wing_process -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/wing_process.h"  # you most likely want to change this
  dnl if test -r $PHP_WING_PROCESS/$SEARCH_FOR; then # path given as parameter
  dnl   WING_PROCESS_DIR=$PHP_WING_PROCESS
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for wing_process files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       WING_PROCESS_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$WING_PROCESS_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the wing_process distribution])
  dnl fi

  dnl # --with-wing_process -> add include path
  dnl PHP_ADD_INCLUDE($WING_PROCESS_DIR/include)

  dnl # --with-wing_process -> check for lib and symbol presence
  dnl LIBNAME=wing_process # you may want to change this
  dnl LIBSYMBOL=wing_process # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $WING_PROCESS_DIR/$PHP_LIBDIR, WING_PROCESS_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_WING_PROCESSLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong wing_process lib version or lib not found])
  dnl ],[
  dnl   -L$WING_PROCESS_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(WING_PROCESS_SHARED_LIBADD)

  PHP_NEW_EXTENSION(wing_process, wing_process.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
