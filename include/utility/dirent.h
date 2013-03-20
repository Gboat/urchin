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
// Created	: 2005/12/9
//
#ifndef _DIRENT_H_
#define _DIRENT_H_

#ifdef WIN32

#include "StdHeader.h"

#include <stdio.h>
//#include <io.h>
 
#ifndef FILENAME_MAX
#define FILENAME_MAX 260
#endif

namespace firtex
{
	namespace utility
	{
        
		struct dirent
		{
			long            d_ino;					/* Always zero. */
			unsigned short  d_reclen;				/* Always zero. */
			unsigned short  d_namlen;				/* Length of name in d_name. */
			tchar           d_name[FILENAME_MAX];	/* File name. */
		};

		/**
		 * This is an internal data structure. Good programmers will not use it
		 * except as an argument to one of the functions below.
		 * dd_stat field is now int (was short in older versions).
		 */
		typedef struct
		{
			/* disk transfer area for this dir */
			struct _finddata_t     dd_dta;

			/* dirent struct to return from dir (NOTE: this makes this thread
			* safe as long as only one thread uses a particular DIR struct at
			* a time) */
			struct dirent           dd_dir;

			/* _findnext handle */
			long                    dd_handle;

			/**
			 * Status of search:
			 *   0 = not started yet (next entry to read is first entry)
			 *  -1 = off the end
			 *   positive = 0 based index of next entry
			 */
			int32_t                    dd_stat;

			/* given path for dir with search pattern (struct is extended) */
			tchar                    dd_name[FX_MAX_PATH];
		} DIR;


#define DIRENT_SEARCH_SUFFIX "*"
#define DIRENT_SLASH PATH_DELIMITERA

		DIR*			opendir(const tchar*);
		struct dirent*	readdir(DIR*);
		int				closedir(DIR*);
		/*void 			rewinddir(DIR*);
		int32_t 			telldir(DIR*);
		void 			seekdir(DIR*, long);*/
	}
}

#else //for linux
	# include <dirent.h>
#endif

#endif  /* Not _DIRENT_H_ */
