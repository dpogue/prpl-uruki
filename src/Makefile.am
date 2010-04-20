urukidir = $(LIBPURPLE_LIBDIR)/purple-2

liburuki_la_LDFLAGS = -module -avoid-version

if PLUGINS

uruki_LTLIBRARIES = liburuki.la

liburuki_la_SOURCES = \
	kiClient.cpp \
	kiFileClient.cpp \
	kiGateClient.cpp \
	kiPurple.cpp \
	liburuki.cpp

liburuki_la_LIBADD = $(LIBNOTIFY_LIBS) $(DBUS_LIBS) $(HSPLASMA_LIBS) $(HSPLASMANET_LIBS)

endif

AM_CPPFLAGS = \
	-DLIBDIR=\"$(LIBPURPLE_LIBDIR)/purple-2/\" \
	-DDATADIR=\"$(LIBPURPLE_DATADIR)\" \
	-DLOCALEDIR=\"$(LIBPURPLE_DATADIR)/locale\" \
	$(DEBUG_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(LIBPURPLE_CFLAGS) \
	$(DBUS_CFLAGS) \
	$(HSPLASMA_CFLAGS) \
	$(HSPLASMANET_CFLAGS)

