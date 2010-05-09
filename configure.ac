m4_define(uruki_version, [0.0.1])

AC_PREREQ([2.61])
AC_INIT([prpl-uruki], [uruki_version], [Darryl Pogue <dvpdiner2@gmail.com>], [prpl-uruki], [http://prpl-uruki.googlecode.com/])
AC_CONFIG_SRCDIR([src/liburuki.cpp])


AM_INIT_AUTOMAKE([prpl-uruki], [uruki_version], "")

AC_PATH_PROG(sedpath, sed)
AC_CONFIG_MACRO_DIR([m4])

# Internationalisation support
AM_PROG_LIBTOOL
GETTEXT_PACKAGE="$PACKAGE"
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, ["$GETTEXT_PACKAGE"], [Define the gettext package to be used])
#ALL_LINGUAS="cz de ee es fr ro sk sv"
AM_GLIB_GNU_GETTEXT

# Config header
AH_TOP([
#ifndef _KI_CONFIG_H
#define _KI_CONFIG_H
])
AH_BOTTOM([#endif])
AC_DEFINE_UNQUOTED([KI_IDNAME], ["$PACKAGE"], [prpl-uruki plugin id string])
AC_DEFINE_UNQUOTED([KI_VERSION], ["uruki_version"], [prpl-uruki version string])
AC_DEFINE_UNQUOTED([KI_AUTHOR], ["$PACKAGE_BUGREPORT"], [prpl-uruki author])

# Check C compiler
CFLAGS_save="$CFLAGS"
AC_PROG_CC
CFLAGS="$CFLAGS_save"

CXXFLAGS_save="$CXXFLAGS"
AC_PROG_CXX
CXXFLAGS="$CXXFLAGS_save"

AC_ARG_ENABLE(debug, [  --enable-debug          Compile with debugging support.],, enable_debug=no)

if test $enable_debug = "yes"; then
	DEBUG_CFLAGS="$DEBUG_CFLAGS -DG_DISABLE_DEPRECATED"
	AC_DEFINE(DEBUG, 1, [Define if debugging is enabled.])
fi

AC_SUBST(DEBUG_CFLAGS)

if test "x$G++"="xyes"; then
	CFLAGS="$CFLAGS -Wall -g3"
fi
AC_SUBST(CFLAGS)


# Check for libpurple
PKG_CHECK_MODULES(PURPLE, purple, [AC_DEFINE(HAVE_PURPLE, 1, [Define if we've found libpurple.])])

AC_SUBST(PURPLE_CFLAGS)
AC_SUBST(PURPLE_LIBS)

AM_PATH_GLIB_2_0(2.12.0,, AC_MSG_ERROR([GLib 2.0 is required to build; please make sure you have the GLib development headers installed. The latest version of GLib is always available at http://www.gtk.org]), gthread)
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)

PKG_CHECK_MODULES(HSPLASMANET, libhsplasmanet)
AC_SUBST(HSPLASMANET_CFLAGS)
AC_SUBST(HSPLASMANET_LIBS)


# Plugins
want_plugins=yes
AM_CONDITIONAL(PLUGINS, test "$want_plugins"="yes")
if test "$want_plugins"="yes"; then
	AC_DEFINE([PURPLE_PLUGINS], 1, [build as plugin])
fi

# Setup libtool
AC_DISABLE_STATIC
AM_PROG_LIBTOOL
LIBTOOL="$LIBTOOL --silent"

AM_CONFIG_HEADER(config.h)

AC_CONFIG_FILES([Makefile src/Makefile pixmaps/Makefile])
AC_OUTPUT

echo
echo $PACKAGE $VERSION
echo "Configuration complete."
echo
echo "Debugging enabled.............: $enable_debug"
echo
echo configure complete, now type \'make\'
echo
