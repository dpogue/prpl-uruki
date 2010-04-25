AC_PREREQ([2.61])


AC_INIT([prpl-uruki], [0.0.1], [dvpdiner2@gmail.com])

AC_CONFIG_MACRO_DIR([m4])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

if test `pwd | wc -w` -ne 1; then
	AC_MSG_ERROR([
You are attempting to build in a path that contains spaces.  This
will fail.  Relocate this source tree to a path that does not contain
spaces and run configure again.
])
fi

AM_INIT_AUTOMAKE([-Wall -Werror -Wno-portability])
AM_CONFIG_HEADER(config.h)

AC_CONFIG_SRCDIR([src/kiDefines.h])

AC_DEFINE_UNQUOTED(CONFIG_ARGS, "$ac_configure_args", [configure arguments])

# Checks for programs.
AC_PROG_CXX
AC_PROG_LIBTOOL
LIBTOOL="$LIBTOOL --silent"
AC_PROG_INSTALL
PKG_PROG_PKG_CONFIG

# Checks for header files.
AC_HEADER_STDC

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T
AC_C_BIGENDIAN


PKG_CHECK_MODULES(GLIB, [glib-2.0 >= 2.12.0 gthread-2.0], , [
	AC_MSG_RESULT(no)
	AC_MSG_ERROR([

You must have GLib 2.12.0 or newer development headers installed to build.

If you have these installed already you may need to install pkg-config so
I can find them.
])])
AC_SUBST(GLIB_CXXFLAGS)
AC_SUBST(GLIB_LIBS)

GLIB_GENMARSHAL=`$PKG_CONFIG --variable=glib_genmarshal glib-2.0`
AC_SUBST(GLIB_GENMARSHAL)

PKG_CHECK_MODULES(PURPLE, [purple >= 2.6.0], , [
	AC_MSG_RESULT(no)
	AC_MSG_ERROR([

You must have libpurple 2.6.0 or newer development headers installed to build.

If you have these installed already you may need to install pkg-config so
I can find them.
])])
AC_SUBST(PURPLE_CXXFLAGS)
AC_SUBST(PURPLE_LIBS)

PKG_CHECK_MODULES(HSPLASMA, [libhsplasma >= 0.1], , [
	AC_MSG_RESULT(no)
	AC_MSG_ERROR([

You must have libhsplasma 0.1 or newer development headers installed to build.

If you have these installed already you may need to install pkg-config so
I can find them.
])])
AC_SUBST(HSPLASMA_CXXFLAGS)
AC_SUBST(HSPLASMA_LIBS)

PKG_CHECK_MODULES(HSPLASMANET, [libhsplasma-net >= 0.1], , [
	AC_MSG_RESULT(no)
	AC_MSG_ERROR([

You must have libhsplasma-net 0.1 or newer development headers installed to build.

If you have these installed already you may need to install pkg-config so
I can find them.
])])
AC_SUBST(HSPLASMANET_CXXFLAGS)
AC_SUBST(HSPLASMANET_LIBS)


AC_ARG_ENABLE(nls, AC_HELP_STRING([--disable-nls], [disable installation of translation files]), enable_i18n="$enableval", enable_i18n=yes)

if test x$enable_i18n = xyes; then
	AC_PROG_INTLTOOL
	GETTEXT_PACKAGE=prpl-uruki
	AC_SUBST(GETTEXT_PACKAGE)
fi

AC_ARG_ENABLE(pixmaps-install, AC_HELP_STRING([--disable-pixmaps-install], [disable installation of pixmap files - Pidgin still needs them!]), enable_pixmaps="$enableval", enable_pixmaps=yes)

AM_CONDITIONAL(INSTALL_PIXMAPS, test "x$enable_pixmaps" = "xyes")

AM_CONDITIONAL(INSTALL_I18N, test "x$enable_i18n" = "xyes")

AC_ARG_ENABLE(debug, [AC_HELP_STRING([--enable-debug],
	[compile with debugging support])], , enable_debug=no)

if test "x$enable_debug" = "xyes" ; then
	AC_DEFINE(DEBUG, 1, [Define if debugging is enabled.])
fi

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT

echo
echo $PACKAGE $VERSION

echo
echo Install pixmaps............... : $enable_pixmaps
echo Install translations.......... : $enable_i18n
echo
echo Print debugging messages...... : $enable_debug
echo
if test "x$enable_pixmaps" = "xno" ; then
	echo
	echo Warning: You have disabled the installation of pixmap data, but Pidgin
	echo still requires installed pixmaps.  Be sure you know what you are doing.
fi
if test "x$enable_i18n" = "xno" ; then
	echo
	echo Warning: You have disabled the building and intallation of translation
	echo data.  Be sure you know what you are doing.
fi
echo
echo configure complete, now type \'make\'
echo
