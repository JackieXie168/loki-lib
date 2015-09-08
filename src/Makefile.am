
# EXTRA_DIST = 
# SUBDIRS =

MAINTAINERCLEANFILES = Makefile.in

INCLUDES =                                              \
	@STLPORT_CFLAGS@                                \
	-I. 						\
	-I$(srcdir) -I$(top_srcdir) 			\
	-I$(top_builddir)				\
        -I$(includedir)                                 \
	-I$(srcdir)/../include/FerrisLoki		\
	-I/usr/local/include                            \
	@SIGC_CFLAGS@                                   \
	@CFLAGS@


lib_LTLIBRARIES      = libferrisloki.la

libferrisloki_la_SOURCES = Extensions.cpp  OrderedStatic.cpp  SafeFormat.cpp  Singleton.cpp  SmallObj.cpp  SmartPtr.cpp  StrongPtr.cpp
libferrisloki_la_LIBADD  = @SIGC_LIBS@
libferrisloki_la_LDFLAGS = @LIBLOKI_VERSION_LT_LINE@


libferrisloki_includedir=$(includedir)/FerrisLoki
libferrisloki_include_HEADERS = \
	$(srcdir)/*.hh

EXTRASRC_FILES = $(srcdir)/*.cpp
EXTRA_DIST = $(EXTRASRC_FILES)
EXTRASRC_INSTALL_DIR = $(includedir)/FerrisLoki/

extrasrcdir   = $(EXTRASRC_INSTALL_DIR)
extrasrc_DATA = $(EXTRASRC_FILES)

##########################################################################
##########################################################################
##########################################################################

