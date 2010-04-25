urukidir = $(LIBPURPLE_LIBDIR)/purple-2

liburuki_la_LDFLAGS = -module -avoid-version

uruki_LTLIBRARIES = liburuki.la

liburuki_la_SOURCES = \
	kiClient.cpp \
	kiAuthClient.cpp \
	kiFileClient.cpp \
	kiGateClient.cpp \
	kiPurple.cpp \
	liburuki.cpp

liburuki_la_LIBADD = $(GLIB_LIBS) $(PURPLE_LIBS) $(HSPLASMA_LIBS) $(HSPLASMANET_LIBS)

AM_CPPFLAGS = \
	-DLIBDIR=\"$(LIBPURPLE_LIBDIR)/purple-2/\" \
	-DDATADIR=\"$(LIBPURPLE_DATADIR)\" \
	-DLOCALEDIR=\"$(LIBPURPLE_DATADIR)/locale\" \
	$(GLIB_CXXFLAGS) \
	$(LIBPURPLE_CXXFLAGS) \
	$(HSPLASMA_CXXFLAGS) \
	$(HSPLASMANET_CXXFLAGS)

