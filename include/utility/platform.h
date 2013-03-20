//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: ¹ùÈð½Ü(GuoRuijie)
// Email	: ruijieguo@software.ict.ac.cn,ruijieguo@gmail.com
// Created	: 2005/10/20
//
#ifndef _PLATFORM_H
#define _PLATFORM_H


#ifndef WIN32

#ifndef _LARGEFILE_SOURCE
	#define _LARGEFILE_SOURCE
#endif

#ifndef _LARGEFILE64_SOURCE
	#define _LARGEFILE64_SOURCE
#endif

#ifndef _FILE_OFFSET_BITS
	#define _FILE_OFFSET_BITS 64
#endif



	/* G++ support */
	#include <stddef.h>
	#include <limits.h>
	#include <float.h>
	#include <unistd.h>
	#include <sys/socket.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <netdb.h>
	#include <arpa/inet.h>
	#include <fcntl.h>
	#include <errno.h>
	#include <stdio.h>
	#include <unistd.h>
	#include <string.h>
	#include <iostream>

	extern int errno;

	#include "tchar.h"

	#include "stdint.h"

	extern int64_t _filelength(int filehandle);
	extern int64_t _tcstoi64(const tchar* str, tchar**end, int radix);
	extern int GetModuleFileName( char* sModuleName, char* sFileName, int nSize);


	#define fileOpen		_topen
	#define fileClose		close
	#define fileRead		read
	#define fileWrite		write
	#define fileSeek		lseek64
	#define fileSize		_filelength	

	#ifdef FX_HAVE_TELL
		#define fileTell tell
	#else
	//ftell (and probably soon ftell64) are POSIX standard functions, but tell and
	//tell64 are not, so we define fileTell in terms of fileSeek.
		#define fileTell(fhandle) fileSeek(fhandle, 0, SEEK_CUR)
	#endif

	#define fileNo(X)		fileno((X))
	#define fileStat		stat64

	#define _time64			time
	#define _gmtime64		gmtime
	#define _localtime64	localtime

	#define _strtoi64		strtoll

	#define O_RANDOM		0
	#undef O_BINARY
	#define O_BINARY		0
	#define _S_IREAD		0444
	#define _S_IWRITE		0333  // write and execute permissions

	#define stricmp			strcasecmp
	#define strnicmp		strncasecmp
	#define wcsnicmp		wcsncasecmp
	#define wcsicmp			wcscasecmp

	#define _mkdir(x)		mkdir(x,0777)
	#define _unlink			unlink
	
	#if !defined(__MINGW32__)
		#define _close			::close
		#define _read			::read
	#endif

	#define PATH_DELIMITERA "/"
	#define FX_MAX_PATH		512

#else//for Win32

	#define HAVE_TCHAR_H

	#include <sys/types.h>
	#include <sys/stat.h>
	#include <direct.h>	
	#include <fcntl.h>
	#include <string>
	#include <io.h>
	#include "tchar.h"	
	#include <time.h>
	#include <errno.h>

	#include "stdint.h"
	
	#define O_LARGEFILE 0

	#define fileOpen	_topen
	#define fileClose	_close
	#define fileRead	_read
	#define fileWrite	_write
	#define fileSize	_filelengthi64
	#define fileSeek	_lseeki64
	#define fileTell	_telli64	
	#define fileNo(X)	_fileno((X))	
	#define fileStat	_stati64

	#define snprintf _snprintf
	#define vsnprintf _vsnprintf

	#define PATH_DELIMITERA "\\"
	#define FX_MAX_PATH		512

#endif //end for Win32

#endif /* _PLATFORM_H */
