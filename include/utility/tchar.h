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
// Created	: 2006/5/20
//
#include <stdlib.h>
#ifndef HAVE_TCHAR_H
#if defined(_UCS2) || defined(_UNICODE)

	#undef _T //remove any previously defined _T - required for ppc os
	#define _T(x)       L ## x

	#define tchar wchar_t
	#define tstring	std::wstring

	#define tstringstream std::wstringstream
	
	//formatting functions
	#define _sntprintf swprintf //* make a formatted a string
	#define _tprintf wprintf //* print a formatted string

	#define _vsntprintf vsnwprintf //* print a formatted string using variable arguments
	
	#define _istalnum iswalnum //* alpha/numeric char check
	#define _istalpha iswalpha //* alpha char check
	#define _istspace iswspace //* space char check
	#define _istdigit iswdigit //* digit char check
	#define _totlower towlower //* convert char to lower case
	#define _totupper towupper //* convert char to lower case
	#define _tcslwr wcslwr //* convert string to lower case
	
	#define _tcscpy wcscpy //copy a string to another string
	#define _tcsncpy wcsncpy //copy a specified amount of one string to another string.
	#define _tcscat wcscat //copy a string onto the end of the other string
	#define _tcschr wcschr //find location of one character
	#define _tcsstr wcsstr //find location of a string
	#define _tcslen wcslen //get length of a string
	#define _tcscmp wcscmp //case sensitive compare two strings
	#define _tcsncmp wcsncmp //case sensitive compare two strings
	#define _tcsnicmp	wcsnicmp
	#define _tcscspn wcscspn //location of any of a set of character in a string
	#define _tcsrchr    wcsrchr

	#define _tcsicmp wcsicmp //* case insensitive compare two string
	#define _stprintf swprintf

	#define _tstof		_wtof
	#define _ttoi		_wtoi
	#define _ttoi64		wtoi64

	/* Stdio functions */		
	#define _tfopen     	_wfopen
	#define _tfreopen   	_wfreopen		
	
	/* Io functions */
	#define _taccess		_waccess
	#define _tchmod			_wchmod
	#define _tcreat			_wcreat
	#define _tfindfirst		_wfindfirst
	#define _tfindfirst64   _wfindfirst64
	#define _tfindfirsti64  _wfindfirsti64
	#define _tfindnext		_wfindnext
	#define _tfindnext64    _wfindnext64
	#define _tfindnexti64   _wfindnexti64
	#define _tmktemp    	_wmktemp
	#define _topen      	_wopen
	#define _tremove    	_wremove
	#define _trename    	_wrename
	#define _tsopen     	_wsopen
	#define _tunlink    	_wunlink

	#define _tfinddata_t    _wfinddata_t
	#define _tfinddata64_t  __wfinddata64_t
	#define _tfinddatai64_t _wfinddatai64_t


	/* Stat functions */

	#define _tstat			_wstat
	#define _tstat64		_wstat64
	#define _tstati64		_wstati64

	#define _stscanf		swscanf

	#define _tcsftime   wcsftime

	//conversion functions
	//#define _tcstod wcstod //convert a string to a double
	//#define _tcstoi64 wcstoll //* convers a string to an 64bit bit integer
	//#define _i64tot lltow //* converts a 64 bit integer to a string (with base)

#else //if defined(_ASCII)

	#undef _T //remove any previously defined _T - required for ppc os
	#define _T(x)      x

	#define tchar char
	#define  tstring std::string

	#define tstringstream std::stringstream

	//formatting functions
	#define _sntprintf snprintf    
	#define _tprintf printf
	#define _vsntprintf vsnprintf 


	#define _istalnum	isalnum
	#define _istalpha	isalpha
	#define _istspace	isspace
	#define _istdigit	isdigit
	#define _totlower	tolower
	#define _totupper	toupper
	#define _tcslwr		strlwr

	#define _tstof		atof
	#define _ttoi		atoi
#ifdef WIN32
	#define _ttoi64		_atoi64
#else
	#define _ttoi64		atoll
#endif
	

	//these are the string handling functions
	#define _tcscpy strcpy
	#define _tcsncpy strncpy
	#define _tcscat strcat
	#define _tcschr strchr
	#define _tcsstr strstr
	#define _tcslen strlen
	#define _tcscmp strcmp
	#define _tcsncmp strncmp
	#define _tcsicmp strcasecmp
	#define _tcsnicmp	strnicmp
	#define _tcscspn strcspn
	#define _tcsrchr    strrchr

	#define _stscanf sscanf
	#define _stprintf	sprintf


	/* Stdio functions */
	
	#define _tfopen			fopen
	#define _tfreopen		freopen

	/* Io functions */
	#define _tchmod			_chmod
	#define _tcreat			_creat
	#define _tfindfirst		_findfirst
	#define _tfindfirst64   _findfirst64
	#define _tfindfirsti64  _findfirsti64
	#define _tfindnext		_findnext
	#define _tfindnext64    _findnext64
	#define _tfindnexti64   _findnexti64
	#define _tmktemp		_mktemp

	#if (defined(_POSIX_) || !defined(WIN32))
		#define _topen		open
		#define _taccess	access
	#else
		#define _topen		_open
		#define _taccess	_access
	#endif

	#define _tremove		remove
	#define _trename		rename
	#define _tsopen			_sopen
	#define _tunlink		_unlink

	#define _tfinddata_t    _finddata_t
	#define _tfinddata64_t  __finddata64_t
	#define _tfinddatai64_t _finddatai64_t

	/* Stat functions */

	#define _tstat      _stat
	#define _tstat64    __stat64
	#define _tstati64   _stati64

	#define _tcsftime   strftime

	////converstion methods
	//#define _tcstod strtod
	//#define _tcstoi64 strtoll
	//#define _i64tot lltoa
#endif

#else //HAVE_TCHAR_H

	#include <tchar.h>

	#if defined(_UCS2) || defined(_UNICODE)
		#define tstring	std::wstring
		#define tchar wchar_t
	#else 
		#define tstring std::string
		#define tchar char
	#endif
	
#endif
