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
// Created	: 2005/11/9
//

#include "store/FSIndexInput.h"

namespace firtex
{
	namespace store
	{
		CFSIndexInput::CFSIndexInput(const tchar* filename)
		{				
			m_fileHandle = ::fileOpen(filename, O_BINARY | O_RDONLY | O_RANDOM | O_LARGEFILE, _S_IREAD );

			if (m_fileHandle < 0)
			{
				int err = errno;
				tstring sFileName = filename;
				if ( err == ENOENT )
					FIRTEX_THROW2(FILEIO_ERROR,"File does not exist: " + sFileName);
				else if ( err == EACCES )
					FIRTEX_THROW2(FILEIO_ERROR,"File Access denied: " + sFileName);
				else if ( err == EMFILE )
					FIRTEX_THROW2(FILEIO_ERROR,"Too many open files: " + sFileName);
			}				
			m_length = fileSize(m_fileHandle);
			m_filename = filename;
		}
		CFSIndexInput::CFSIndexInput(const tchar* filename,char* buffer,size_t buffsize)
			: CIndexInput(buffer,buffsize)
		{
			m_fileHandle = ::fileOpen(filename, O_BINARY | O_RDONLY | O_RANDOM | O_LARGEFILE, _S_IREAD );

			if (m_fileHandle < 0)
			{
				int err = errno;
				tstring sFileName = filename;
				if ( err == ENOENT )
					FIRTEX_THROW2(FILEIO_ERROR,"File does not exist: " + sFileName);
				else if ( err == EACCES )
					FIRTEX_THROW2(FILEIO_ERROR,"File Access denied: " + sFileName);
				else if ( err == EMFILE )
					FIRTEX_THROW2(FILEIO_ERROR,"Too many open files: " + sFileName);
			}				
			m_length = fileSize(m_fileHandle);
			m_filename = filename;
		}
		CFSIndexInput::CFSIndexInput(const tchar* filename,size_t buffsize)
			:CIndexInput(buffsize)
		{
			m_fileHandle = ::fileOpen(filename, O_BINARY | O_RDONLY | O_RANDOM | O_LARGEFILE, _S_IREAD );

			if (m_fileHandle < 0)
			{
				int err = errno;
				tstring sFileName = filename;
				if ( err == ENOENT )
					FIRTEX_THROW2(FILEIO_ERROR,"File does not exist: " + sFileName);
				else if ( err == EACCES )
					FIRTEX_THROW2(FILEIO_ERROR,"File Access denied: " + sFileName);
				else if ( err == EMFILE )
					FIRTEX_THROW2(FILEIO_ERROR,"Too many open files: " + sFileName);
			}				
			m_length = fileSize(m_fileHandle);
			m_filename = filename;
		}

		CFSIndexInput::~CFSIndexInput()
		{
			close(); 
		}

		void	CFSIndexInput::readInternal(char* b,size_t length,bool bCheck/* = true*/)
		{
			if(bCheck)
			{
				int64_t position = getFilePointer();
				if (position != fileTell(m_fileHandle))
				{
					if(fileSeek(m_fileHandle,position,SEEK_SET) != position)
						FIRTEX_THROW2(FILEIO_ERROR,"CFSIndexInput::readInternal():file IO seek error: " + m_filename);
				}
			}
			size_t ret = ::fileRead(m_fileHandle,b,(unsigned int)length);
			if(ret == 0)
				FIRTEX_THROW2(FILEIO_ERROR,"CFSIndexInput::readInternal():read past EOF:" + m_filename);
			else if (ret == -1)
			{
				int err = errno;
				cout << err;
				FIRTEX_THROW2(FILEIO_ERROR,"CFSIndexInput::readInternal():file IO read error:" + m_filename);
			}
		}

		void  CFSIndexInput::close()
		{
			if(m_fileHandle >= 0)
			{
				::fileClose(m_fileHandle);					
				m_fileHandle = -1;
			}				
		}			


		CIndexInput* CFSIndexInput::clone(char* buffer,size_t buffsize)
		{				
			CFSIndexInput* pClone = new CFSIndexInput(m_filename.c_str(),buffer,buffsize);
			return pClone;
		}

		CIndexInput* CFSIndexInput::clone()
		{
			CFSIndexInput* pClone = new CFSIndexInput(m_filename.c_str(),m_bufferSize);
			return pClone;
		}	
		void CFSIndexInput::seekInternal(int64_t position)
		{
			if(fileSeek(m_fileHandle,position,SEEK_SET) != position)
				FIRTEX_THROW2(FILEIO_ERROR,"CFSIndexInput::seekInternal():file IO seek error: " + m_filename);
		}		
	}
}
