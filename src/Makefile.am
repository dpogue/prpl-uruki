EXTRA_DIST = \
	kiAuthClient.h \
	kiDefines.h \
	kiGateClient.h \
	kiClient.h \
	kiFileClient.h \
	kiPurple.h

INCLUDES = \
	-I$(top_srcdir) \
    -DLOCALEDIR=\""$(localedir)"\"

plugindir = $(libdir)/purple-2
liburuki_la_LDFLAGS = -module -avoid-version

if PLUGINS
plugin_LTLIBRARIES = liburuki.la

liburuki_la_SOURCES = \
	kiClient.cpp \
	kiAuthClient.cpp \
	kiFileClient.cpp \
	kiGateClient.cpp \
	kiPurple.cpp \
	liburuki.cpp

liburuki_la_LIBADD = \
	$(PURPLE_LIBS) $(GLIB_LIBS) $(HSPLASMA_LIBS) $(HSPLASMANET_LIBS)
endif

AM_CPPFLAGS = \
	-DLIBDIR=\"$(libdir)/purple-2/\" \
	-DDATADIR=\"$(datadir)\" \
	$(CFLAGS) \
	$(GLIB_CFLAGS) \
	$(PURPLE_CFLAGS) \
	$(DEBUG_CFLAGS)

AM_CFLAGS = -DPURPLE_PLUGINS
