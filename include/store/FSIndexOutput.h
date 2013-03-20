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
#ifndef _FILECACHEWRITER_H
#define _FILECACHEWRITER_H

#include <stdio.h>
#include "../utility/Misc.h"
#include "IndexOutput.h"

using namespace std;
using namespace firtex::utility;

namespace firtex
{
	namespace store
	{
		class CFSIndexOutput :	public CIndexOutput
		{
		public:
			CFSIndexOutput(const tchar* filename):CIndexOutput(0)
			{
				if ( CMisc::dirExists(filename) )
					m_fileHandle = ::fileOpen( filename, O_BINARY | O_RDWR | O_RANDOM | O_TRUNC | O_LARGEFILE, _S_IREAD | _S_IWRITE);
				else 
					m_fileHandle = ::fileOpen( filename, O_BINARY | O_RDWR | O_RANDOM | O_CREAT | O_LARGEFILE, _S_IREAD | _S_IWRITE);

				if ( m_fileHandle < 0 )
				{
					int err = errno;
					tstring sFileName = filename;
					if ( err == ENOENT )
						FIRTEX_THROW2(FILEIO_ERROR,"File does not exist:" + sFileName);
					else if ( err == EACCES )
						FIRTEX_THROW2(FILEIO_ERROR,"File Access denied:" + sFileName);
					else if ( err == EMFILE )
						FIRTEX_THROW2(FILEIO_ERROR,"Too many open files:" + sFileName);
				}				
			}
			CFSIndexOutput(const tchar* filename,char* buffer,size_t buffsize):CIndexOutput(buffer,buffsize)
			{
				if ( CMisc::dirExists(filename) )
					m_fileHandle = ::fileOpen( filename, O_BINARY | O_RDWR | O_RANDOM | O_TRUNC | O_LARGEFILE, _S_IREAD | _S_IWRITE);
				else 
					m_fileHandle = ::fileOpen( filename, O_BINARY | O_RDWR | O_RANDOM | O_CREAT | O_LARGEFILE, _S_IREAD | _S_IWRITE);

				if ( m_fileHandle < 0 )
				{
					int err = errno;
					tstring sFileName = filename;
					if ( err == ENOENT )
						FIRTEX_THROW2(FILEIO_ERROR,"File does not exist:" + sFileName);
					else if ( err == EACCES )
						FIRTEX_THROW2(FILEIO_ERROR,"File Access denied:" + sFileName);
					else if ( err == EMFILE )
						FIRTEX_THROW2(FILEIO_ERROR,"Too many open files:" + sFileName);
				}				
			}
			CFSIndexOutput(const tchar* filename,size_t buffsize):CIndexOutput(buffsize)
			{
				if ( CMisc::dirExists(filename) )
					m_fileHandle = ::fileOpen( filename, O_BINARY | O_RDWR | O_RANDOM | O_TRUNC | O_LARGEFILE, _S_IREAD | _S_IWRITE);
				else 
					m_fileHandle = ::fileOpen( filename, O_BINARY | O_RDWR | O_RANDOM | O_CREAT | O_LARGEFILE, _S_IREAD | _S_IWRITE);

				if ( m_fileHandle < 0 )
				{
					int err = errno;
					tstring sFileName = filename;
					if ( err == ENOENT )
						FIRTEX_THROW2(FILEIO_ERROR,"File does not exist:" + sFileName);
					else if ( err == EACCES )
						FIRTEX_THROW2(FILEIO_ERROR,"File Access denied:" + sFileName);
					else if ( err == EMFILE )
						FIRTEX_THROW2(FILEIO_ERROR,"Too many open files:" + sFileName);
				}				
			}			

			CFSIndexOutput():CIndexOutput(),m_fileHandle(-1)
			{				
			}		

			virtual ~CFSIndexOutput(void)
			{
				close();
			}
				
		public:
			void flushBuffer(char* b, size_t len)
			{
				if(len > 0 && ::fileWrite(m_fileHandle,b,(unsigned int)len) != len)
				{
					int err = errno;
					if(err == ENOSPC)
						FIRTEX_THROW2(FILEIO_ERROR,"CFSIndexOutput::flushBuffer():no space in the disk.");
					else 
						FIRTEX_THROW2(FILEIO_ERROR,"CFSIndexOutput::flushBuffer():file IO write error:");
				}
			}
			void seek(int64_t pos)
			{
				CIndexOutput::seek(pos);				
				if(fileSeek(m_fileHandle,pos,SEEK_SET) != pos)
					FIRTEX_THROW2(FILEIO_ERROR,"CFSIndexOutput::seek():file IO seek error.");
			}
			int64_t length()
			{
				return m_bufferStart + (int64_t)m_bufferPosition;
			}
			void open(const tchar* filename)
			{
				if(m_fileHandle >= 0)
				{ 
					close();
				}
				if ( CMisc::dirExists(filename) )
					m_fileHandle = ::fileOpen( filename, O_BINARY | O_RDWR | O_RANDOM | O_TRUNC | O_LARGEFILE, _S_IREAD | _S_IWRITE);
				else 
					m_fileHandle = ::fileOpen( filename, O_BINARY | O_RDWR | O_RANDOM | O_CREAT | O_LARGEFILE, _S_IREAD | _S_IWRITE);

				if ( m_fileHandle < 0 )
				{
					int err = errno;
					if ( err == ENOENT )
						FIRTEX_THROW2(FILEIO_ERROR,"File does not exist");
					else if ( err == EACCES )
						FIRTEX_THROW2(FILEIO_ERROR,"File Access denied");
					else if ( err == EMFILE )
						FIRTEX_THROW2(FILEIO_ERROR,"Too many open files");
				}				
			}

			void close()
			{
				if(m_fileHandle >= 0)
				{
					CIndexOutput::close();
					::fileClose(m_fileHandle);					
					m_fileHandle = -1;					
				}				
			}			
		protected:			
			int		m_fileHandle;
		};
	}
}


#endif

