## Makefile.am -- Process this file with automake to produce Makefile.in

INCLUDES	= -I../../include
appsrcdir 	= $(top_srcdir)/app/fxist4c

bindir = ${exec_prefix}/bin

bin_PROGRAMS      = fxist4c

fxist4c_SOURCES   = $(appsrcdir)/main.cpp

fxist4c_LDADD     = $(top_builddir)/src/libfirtex.a $(top_builddir)/contrib/snowball/libsnowball.a  -lpthread -ldl
fxist4c_LDFLAGS = $(LIBADD_DL)
