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
// Created	: 2006/9/5
//
#include "utility/StdHeader.h"
#include "utility/platform.h"

#ifndef WIN32
int64_t _filelength(int filehandle)
{
	struct fileStat info;
	if (fstat64(filehandle, &info) == -1)
		FIRTEX_THROW2(FILEIO_ERROR,_T("fileStat error" ));
	return info.st_size;
}

int64_t _tcstoi64(const tchar* str, tchar**end, int radix)
{
#define FX_TCSTOI64_RADIX(x,r) ((x>=_T('0') && x<=_T('9'))?x-_T('0'):((x>=_T('a') && x<=_T('z'))?x-_T('a')+10:((x>=_T('A') && x<=_T('Z'))?x-_T('A')+10:1000)))

	if (radix < 2 || radix > 36) 
		return 0;

	/* Skip white space.  */
	while (_istspace (*str))
		++str;

	int sign=1;
	if ( str[0] == _T('+') )
		str++;
	else if ( str[0] == _T('-') ){
		sign = -1;
		str++;
	}

	*end=(tchar*)str;
	long r = -1;
	while ( (r=FX_TCSTOI64_RADIX(*end[0],radix)) >=0 && r<radix )
		(*end)++;

	tchar* p = (*end)-1;
	int64_t ret = 0;
	int pos = 0;
	for ( ;p>=str;p-- ){
		int i=FX_TCSTOI64_RADIX(p[0],radix);
		if ( pos == 0 )
			ret=i;
		else
			ret += (int64_t)pow((float)radix,(float)pos) * i; //todo: might be quicker with a different pow overload

		pos++;
	}
	return sign*ret;
}

int GetModuleFileName( char* sModuleName, char* sFileName, int nSize)
{
	int ret = -1;
	char sLine[1024] = { 0 };
	void* pSymbol = (void*)"";
	FILE *fp;
	char *pPath;
	fp = fopen ("/proc/self/maps", "r");
	if ( fp != NULL )
	{
		while (!feof (fp))
		{
			unsigned long start, end;
			if ( !fgets (sLine, sizeof (sLine), fp))
				continue;
			if ( !strstr (sLine, " r-xp ") || !strchr (sLine, '/'))
				continue;
			sscanf (sLine, "%lx-%lx ", &start, &end);
			if (pSymbol >= (void *) start && pSymbol < (void *) end)
			{
				char *tmp;
				size_t len;
				/* Extract the filename; it is always an absolute path */
				pPath = strchr (sLine, '/');
				/* Get rid of the newline */
				tmp = strrchr (pPath, '\n');
				if (tmp) *tmp = 0;
				/* Get rid of "(deleted)" */
				//len = strlen (pPath);
				//if (len > 10 && strcmp (pPath + len - 10, " (deleted)") == 0)
				//{
				// tmp = pPath + len - 10;
				// *tmp = 0;
				//}
				ret = 0;
				strcpy( sFileName, pPath );
			}
		}
		fclose (fp);
	}
	return ret;
}
#endif
