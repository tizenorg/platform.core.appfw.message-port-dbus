NULL = 
lib_LTLIBRARIES = libmessage-port.la
libmessage_port_la_SOURCES = \
    message-port.c \
    msgport-utils.h \
    msgport-utils.c \
    msgport-service.h \
    msgport-service.c \
    msgport-manager.h \
    msgport-manager.c \
    msgport-factory.h \
    msgport-factory.c \
    compatibility/message_port_wrapper.c \
    $(NULL)

libmessage_port_la_LDFLAGS = -version-info $(subst .,:,$(VERSION))

libmessage_port_la_includedir = $(includedir)/
libmessage_port_la_include_HEADERS = \
    message-port.h  \
    compatibility/message_port.h  \
    $(NULL)

libmessage_port_la_CPPFLAGS = \
    -I . \
    -I $(top_builddir) \
    -DLOG_TAG=\"MESSAGEPORT/LIB\" \
    $(GLIB_CFLAGS) $(GIO_CFLAGS) $(BUNDLE_CFLAGS) $(DLOG_CFLAGS) $(CAPIBASECOMMON_CFLAGS) \
    -Wall -error
    $(NULL)

libmessage_port_la_LIBADD = \
    ../common/libmessageport-common.la \
    $(GLIB_LIBS) $(GIO_LIBS) $(BUNDLE_LIBS) $(DLOG_LIBS) $(CAPIBASECOMMON_LIBS) \
    $(NULL)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = message-port.pc

CLEANFILES =
