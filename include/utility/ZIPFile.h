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
// Created	: 2007/1/9
//

#ifndef _ZIPFILE_H
#define _ZIPFILE_H
#include "StdHeader.h"
#include "FXString.h"

namespace firtex
{
	namespace utility
	{
		class CZIPFile
		{
			// Construction
		public:
			CZIPFile(int hAttach = -1);
			~CZIPFile();

			// File Class
		public:
			class File
			{
			private:				
				inline File(){}				
			public:
				size_t		decompress(byte* pOutBuffer,size_t outLen);
				bool		extract(const tchar* pszFile);

			protected:
				bool		prepareToDecompress(void* pStream);	
			public:
				CFXString	m_sName;
				int64_t		m_nSize;
			protected:
				int64_t		m_nLocalOffset;
				int64_t		m_nCompressedSize;
				int32_t		m_nCompression;			
			protected:
				CZIPFile*	m_pZIP;					

				friend class CZIPFile;
			};	

			// Operations
		public:
			bool	open(const tchar* pszFile);
			bool	attach(int hFile);
			bool	isOpen() const;
			void	close();
		public:
			int32_t	getCount() const;
			File*	getFile(int32_t nFile) const;
			File*	getFile(const tchar* pszFile, bool bPartial = false) const;
		protected:
			bool	locateCentralDirectory();
			bool	parseCentralDirectory(byte* pDirectory, int32_t nDirectory);
			bool	seekToFile(File* pFile);
			// Attributes
		protected:
			bool	m_bAttach;
			int		m_fileHandle;
			File*	m_pFile;
			int		m_nFile;		
		};
	}
}


#endif
