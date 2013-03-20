## Process this file with Automake to create Makefile.in.

INCLUDES	= -I../../include -I./HTMLParser

bindir = ${exec_prefix}/bin/plugins

bin_PROGRAMS = htmlparserplugin.so

htmlparserplugin_so_SOURCES = *.h
htmlparserplugin_so_SOURCES += HTMLParser/*.h
htmlparserplugin_so_SOURCES += HTMLParser/HTArray.c
htmlparserplugin_so_SOURCES += HTMLParser/HTAssoc.c
htmlparserplugin_so_SOURCES += HTMLParser/HTAtom.c
htmlparserplugin_so_SOURCES += HTMLParser/HTBTree.c
htmlparserplugin_so_SOURCES += HTMLParser/HTChunk.c
htmlparserplugin_so_SOURCES += HTMLParser/HTHash.c
htmlparserplugin_so_SOURCES += HTMLParser/HTList.c
htmlparserplugin_so_SOURCES += HTMLParser/HTMemory.c
htmlparserplugin_so_SOURCES += HTMLParser/HTString.c
htmlparserplugin_so_SOURCES += HTMLParser/HTTrace.c
htmlparserplugin_so_SOURCES += HTMLParser/HTUU.c
htmlparserplugin_so_SOURCES += HTMLParser/HTAlert.c
htmlparserplugin_so_SOURCES += HTMLParser/HTAnchor.c
htmlparserplugin_so_SOURCES += HTMLParser/HTChannl.c
htmlparserplugin_so_SOURCES += HTMLParser/HTDNS.c
htmlparserplugin_so_SOURCES += HTMLParser/HTError.c
htmlparserplugin_so_SOURCES += HTMLParser/HTEscape.c
htmlparserplugin_so_SOURCES += HTMLParser/HTEvent.c
htmlparserplugin_so_SOURCES += HTMLParser/HTFormat.c
htmlparserplugin_so_SOURCES += HTMLParser/HTHost.c
htmlparserplugin_so_SOURCES += HTMLParser/HTInet.c
htmlparserplugin_so_SOURCES += HTMLParser/HTLib.c
htmlparserplugin_so_SOURCES += HTMLParser/HTLink.c
htmlparserplugin_so_SOURCES += HTMLParser/HTMemLog.c
htmlparserplugin_so_SOURCES += HTMLParser/HTMethod.c
htmlparserplugin_so_SOURCES += HTMLParser/HTNet.c
htmlparserplugin_so_SOURCES += HTMLParser/HTNoFree.c
htmlparserplugin_so_SOURCES += HTMLParser/HTParse.c
htmlparserplugin_so_SOURCES += HTMLParser/HTProt.c
htmlparserplugin_so_SOURCES += HTMLParser/HTReqMan.c
htmlparserplugin_so_SOURCES += HTMLParser/HTResponse.c
htmlparserplugin_so_SOURCES += HTMLParser/HTStream.c
htmlparserplugin_so_SOURCES += HTMLParser/HTTCP.c
htmlparserplugin_so_SOURCES += HTMLParser/HTTimer.c
htmlparserplugin_so_SOURCES += HTMLParser/HTTrans.c
htmlparserplugin_so_SOURCES += HTMLParser/HTUTree.c
htmlparserplugin_so_SOURCES += HTMLParser/HTUser.c
htmlparserplugin_so_SOURCES += HTMLParser/HTWWWStr.c
htmlparserplugin_so_SOURCES += HTMLParser/HTMLPDTD.c
htmlparserplugin_so_SOURCES += HTMLParser/SGML.c
htmlparserplugin_so_SOURCES += HTMLParser/HTMLGen.c
htmlparserplugin_so_SOURCES += HTMLParser/HTTeXGen.c
htmlparserplugin_so_SOURCES += HTMLParser/HTPlain.c
htmlparserplugin_so_SOURCES += HTMLParser/HTML.c
htmlparserplugin_so_SOURCES += HTMLParser/HText.c
htmlparserplugin_so_SOURCES += HTMLParser/HTHInit.c
htmlparserplugin_so_SOURCES += HTMLParser/HTStyle.c

htmlparserplugin_so_SOURCES += HTMLParserPlugin.cpp
htmlparserplugin_so_SOURCES += HTMLParserPluginFactory.cpp
htmlparserplugin_so_SOURCES += HtmlTextStripper.cpp
htmlparserplugin_so_SOURCES += main.cpp

htmlparserplugin_so_LDADD   = $(top_builddir)/src/libcom.a -ldl 
htmlparserplugin_so_LDFLAGS = $(LIBADD_DL) -fPIC -shared -rdynamic
htmlparserplugin_so_CXXFLAGS = @CXXFLAGS@ -fPIC -shared
htmlparserplugin_so_CFLAGS = @CFLAGS@ -fPIC -shared

EXTRA_DIST = \
	HTMLParserPlugin_7.1.vcproj\
	HTMLParserPlugin_8.0.vcproj\
	HTMLParserPlugin.def\
	HTMLParser/windows/config.h
