## Makefile.am -- Process this file with automake to produce Makefile.in

com_srcdir		= $(fsrcdir)/com
libfirtex_a_SOURCES  += $(fincdir)/com/linux/*.h
libfirtex_a_SOURCES  += $(fincdir)/com/*.h
libfirtex_a_SOURCES  += $(com_srcdir)/bstr.cpp
libfirtex_a_SOURCES  += $(com_srcdir)/iid_cpp.cpp
libfirtex_a_SOURCES  += $(com_srcdir)/linux/_bstr.cpp
libfirtex_a_SOURCES  += $(com_srcdir)/linux/Com.cpp
libfirtex_a_SOURCES  += $(com_srcdir)/linux/errorinfo.cpp
libfirtex_a_SOURCES  += $(com_srcdir)/linux/log.cpp
libfirtex_a_SOURCES  += $(com_srcdir)/linux/malloc.cpp
libfirtex_a_SOURCES  += $(com_srcdir)/linux/compobj.cpp
libfirtex_a_SOURCES  += $(com_srcdir)/linux/marshal.cpp
libfirtex_a_SOURCES  += $(com_srcdir)/linux/registered_class_objects.cpp
libfirtex_a_SOURCES  += $(com_srcdir)/linux/createinstance.cpp
libfirtex_a_SOURCES  += $(com_srcdir)/linux/database.cpp
libfirtex_a_SOURCES  += $(com_srcdir)/linux/string.cpp
libfirtex_a_SOURCES  += $(com_srcdir)/linux/dllregisterserver.cpp
libfirtex_a_SOURCES  += $(com_srcdir)/linux/dllserver.cpp
libfirtex_a_SOURCES  += $(com_srcdir)/linux/thread.cpp
libfirtex_a_SOURCES  += $(com_srcdir)/linux/git.cpp
libfirtex_a_SOURCES  += $(com_srcdir)/linux/guid.cpp
libfirtex_a_SOURCES  += $(com_srcdir)/linux/zstr.cpp
libfirtex_a_SOURCES  += $(com_srcdir)/linux/inproc32.cpp
libfirtex_a_SOURCES  += $(com_srcdir)/linux/loadlibrary.cpp

libcom_a_SOURCES  = $(com_srcdir)/bstr.cpp
libcom_a_SOURCES  += $(com_srcdir)/iid_cpp.cpp
libcom_a_SOURCES  += $(com_srcdir)/linux/_bstr.cpp
libcom_a_SOURCES  += $(com_srcdir)/linux/Com.cpp
libcom_a_SOURCES  += $(com_srcdir)/linux/errorinfo.cpp
libcom_a_SOURCES  += $(com_srcdir)/linux/log.cpp
libcom_a_SOURCES  += $(com_srcdir)/linux/malloc.cpp
libcom_a_SOURCES  += $(com_srcdir)/linux/compobj.cpp
libcom_a_SOURCES  += $(com_srcdir)/linux/marshal.cpp
libcom_a_SOURCES  += $(com_srcdir)/linux/registered_class_objects.cpp
libcom_a_SOURCES  += $(com_srcdir)/linux/createinstance.cpp
libcom_a_SOURCES  += $(com_srcdir)/linux/database.cpp
libcom_a_SOURCES  += $(com_srcdir)/linux/string.cpp
libcom_a_SOURCES  += $(com_srcdir)/linux/dllregisterserver.cpp
libcom_a_SOURCES  += $(com_srcdir)/linux/dllserver.cpp
libcom_a_SOURCES  += $(com_srcdir)/linux/thread.cpp
libcom_a_SOURCES  += $(com_srcdir)/linux/git.cpp
libcom_a_SOURCES  += $(com_srcdir)/linux/guid.cpp
libcom_a_SOURCES  += $(com_srcdir)/linux/zstr.cpp
libcom_a_SOURCES  += $(com_srcdir)/linux/inproc32.cpp
libcom_a_SOURCES  += $(com_srcdir)/linux/loadlibrary.cpp

libcom_a_LIBADD	= $(fsrcdir)/libfirtex_a-XML.o $(fsrcdir)/libfirtex_a-Misc.o $(fsrcdir)/libfirtex_a-StringUtils.o $(fsrcdir)/libfirtex_a-FirteXCOM.o
libcom_a_CXXFLAGS = @CXXFLAGS@ -fPIC -shared
libcom_a_CFLAGS = @CFLAGS@ -fPIC -shared

EXTRA_DIST += \
	$(com_srcdir)/iid_c.c\
	$(com_srcdir)/win32/Com.cpp\
	$(com_srcdir)/win32/DllMain.cpp\
	$(com_srcdir)/win32/DllRegisterServerImpl.cpp\
	$(com_srcdir)/win32/DllServer.cpp
	



