# not a GNU package. You can remove this line, if
# have all needed files, that a GNU package needs
AUTOMAKE_OPTIONS = foreign 1.4
SUBDIRS = src contrib/snowball contrib/win32 app/fxist4c app/Fxist plugins/HTMLParserPlugin bin_release bin_debug win32 doc 

EXTRA_DIST =\
	autogen.sh\
	Doxygen

upload:
	make dist
	ncftpput -v upload.sourceforge.net /incoming @PACKAGE@-@VERSION@.tar.gz

doxygen:
	rm -fdr ./doc/html
	doxygen Doxygen
	cp ./doc/firtex_logo.png ./doc/html/firtex_logo.png
	
