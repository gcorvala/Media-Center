AC_PREREQ(2.65)
AC_INIT([GMC], [1.0.0], [just.the.boss@gmail.com])

GMC_CURRENT=1
GMC_AGE=0
GMC_REVISION=0

AC_SUBST(GMC_CURRENT)
AC_SUBST(GMC_AGE)
AC_SUBST(GMC_REVISION)

GLIB_REQUIRED=2.22.0
CLUTTER_REQUIRED=1.1.0

AC_CONFIG_MACRO_DIR(m4)
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_AUX_DIR(.)

AM_INIT_AUTOMAKE(-Wall -Werror foreign)

PKG_CHECK_MODULES(GMC,
[
	clutter-1.0 >= $CLUTTER_REQUIRED
	glib-2.0 >= $GLIB_REQUIRED
	sqlite3
])

AC_PROG_CC
AC_CONFIG_FILES(
[
	Makefile
	data/Makefile
	data/img/Makefile
	data/icons/Makefile
 	src/Makefile
])
AC_OUTPUT
