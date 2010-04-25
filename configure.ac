AC_PREREQ([2.61])


AC_INIT([prpl-uruki], [0.0.1], [dvpdiner2@gmail.com])

AC_CONFIG_MACRO_DIR([m4])

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AM_INIT_AUTOMAKE([-Wall -Werror -Wno-portability])

AM_PROG_LIBTOOL
AM_CONFIG_HEADER(config.h)
AM_PROG_MKDIR_P

# Checks for programs.
AC_PROG_CC
AC_PROG_CC_STDC
AC_PROG_CXX
AC_PROG_INSTALL
PKG_PROG_PKG_CONFIG

# Checks for header files.
AC_HEADER_STDC
AC_C_INLINE

# Checks for required libraries
PKG_CHECK_MODULES(GLIB, [glib-2.0 >= 2.12.0 gthread-2.0], , [
	AC_MSG_RESULT(no)
	AC_MSG_ERROR([

You must have GLib 2.12.0 or newer development headers installed to build.

If you have these installed already you may need to install pkg-config so
I can find them.
])])
AC_SUBST(GLIB_CXXFLAGS)
AC_SUBST(GLIB_LIBS)

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


AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT

echo
echo $PACKAGE $VERSION
echo
echo configure complete, now type \'make\'
echo
