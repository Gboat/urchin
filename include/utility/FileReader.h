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
// Created	: 2005/12/16
//
#ifndef __FILEREADER_H
#define __FILEREADER_H

#include "../utility/StdHeader.h"
#include "Reader.h"

namespace firtex
{
	namespace utility
	{
		class CFileReader : public CReader
		{				
		public:
			CFileReader();
			CFileReader(char* buffer,size_t buffsize);
			CFileReader(size_t buffsize);
			CFileReader(const tstring& filename);
			CFileReader(const tstring& filename,char* buffer,size_t buffsize);
			CFileReader(const tstring& filename,size_t buffsize);
			virtual ~CFileReader();
		public:			
			bool	open(const tstring& filename,bool bThrowError = false);
			void	close();			

			CReader*	clone(char* buffer,size_t buffsize);
			CReader*	clone();

			const char* getReaderName() const{return "FileReader";}
		protected:
			void  readInternal(char* b, size_t offset, size_t len);						
			void  seekInternal(int64_t position);
		protected:			
			int				m_fileHandle;
			tstring			m_filename;
		};
	}
}

#endif
