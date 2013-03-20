## Makefile.am -- Process this file with automake to produce Makefile.in

INCLUDES 			= -I../include -I../contrib
noinst_LIBRARIES        	= libfirtex.a libcom.a
fsrcdir			= $(top_srcdir)/src
fincdir			= $(top_srcdir)/include

EXTRA_DIST = \
	$(fincdir)/FirteX.h\
	thread/win32/Mutex.cpp

libfirtex_a_SOURCES  =
include analyzer/Makefile.am
include collection/Makefile.am
include com/Makefile.am
include document/Makefile.am
include index/Makefile.am
include parser/Makefile.am
include plugin/Makefile.am
include search/Makefile.am
include store/Makefile.am
include thread/Makefile.am
include utility/Makefile.am
include config/Makefile.am

libfirtex_a_CXXFLAGS = @CXXFLAGS@ -fPIC -shared
libfirtex_a_CFLAGS = @CFLAGS@ -fPIC -shared


