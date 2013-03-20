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
// Created	: 2007/4/23
//

#ifndef __MMAPFILE_H
#define __MMAPFILE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "utility/Config.h"

#ifdef _WIN32
#include <windows.h>
#endif

namespace firtex
{
	namespace utility
	{
		class CMMapFile
		{
		public:			
			CMMapFile(const tchar* _path);
			virtual ~CMMapFile(void);
		public:			
			/**
			 * open a file
			 * @param _path path of file 
			 */
			void			open(const tchar* _path);

			/**
			 * close file
			 */
			void			close();

			/**
			 * data address of file
			 * @return address of data
			 */
			byte*			data(){return m_pData;}

			/** 
			 * length of file
			 * @return length of file
			 */
			int64_t			length(){return m_nLength;}
		protected:			
			tstring			m_filename;

			byte*			m_pData;
			int64_t			m_nPos;
			int64_t			m_nLength;

#ifdef WIN32
			HANDLE			m_hMap;
			HANDLE			m_hFile;
#else
			int				m_hFile;
#endif
		};
	}
}


#endif
