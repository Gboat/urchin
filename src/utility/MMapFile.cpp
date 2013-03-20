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
#include "utility/MMapFile.h"

#ifndef WIN32
#include <sys/mman.h>
#endif

namespace firtex
{
	namespace utility
	{
		CMMapFile::CMMapFile(const tchar* _path)
			:m_nPos(0)
			,m_pData(NULL)
			,m_nLength(0)
		{			

#ifdef WIN32
			m_hMap = NULL;
			m_hFile = NULL;
#else
			m_hFile = 0;
#endif
			open(_path);
		}

		CMMapFile::~CMMapFile(void)
		{
			close();
		}

		void CMMapFile::open(const tchar* _path)
		{
			close();
#ifdef WIN32
			m_hMap = NULL;
			m_hFile = CreateFile(_path,GENERIC_READ,FILE_SHARE_READ, 0,OPEN_EXISTING,0,0);

			if (m_hFile < 0)
			{
				DWORD err = GetLastError();
				if ( err == ERROR_FILE_NOT_FOUND )
					FIRTEX_THROW3(FILEIO_ERROR, _T("File does not exist"));
				else if ( err == EACCES )
					FIRTEX_THROW3(FILEIO_ERROR,_T("File Access denied"));
				else if ( err == ERROR_TOO_MANY_OPEN_FILES )
					FIRTEX_THROW3(FILEIO_ERROR,_T("Too many open files"));
				else
					FIRTEX_THROW3(FILEIO_ERROR, _T("File IO Error"));
			}

			DWORD nHigh = 0;
			m_nLength = GetFileSize(m_hFile,&nHigh);
			if(nHigh > 0)
				m_nLength |= (nHigh<<32);

			if ( m_nLength > 0 )
			{
				m_hMap = CreateFileMapping(m_hFile,NULL,PAGE_READONLY,0,0,NULL);
				if (m_hMap != NULL )
				{
					void* address = MapViewOfFile(m_hMap,FILE_MAP_READ,0,0,0);
					if ( address != NULL )
					{
						m_pData = (byte*)address;
						return; ///OK!
					}
				}
				CloseHandle(m_hMap);

				tchar* lpMsgBuf = 0;
				DWORD dw = GetLastError(); 

				FormatMessage(
					FORMAT_MESSAGE_ALLOCATE_BUFFER | 
					FORMAT_MESSAGE_FROM_SYSTEM,
					NULL,
					dw,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					lpMsgBuf,
					0, NULL );

				tchar* errstr = new tchar[_tcslen(lpMsgBuf) + 40];
				_stprintf(errstr, _T("MMapIndexInput::MMapIndexInput failed with error %d: %s"), dw, lpMsgBuf); 
				LocalFree(lpMsgBuf);

				FIRTEX_THROW3(FILEIO_ERROR,errstr);
			}

#else //WIN32

			m_hFile = ::fileOpen(_path, O_BINARY | O_RDONLY | O_RANDOM | O_LARGEFILE, _S_IREAD );

			if (m_hFile < 0)
			{
				int err = errno;
				tstring sFileName = _path;
				if ( err == ENOENT )
					FIRTEX_THROW3(FILEIO_ERROR,_T("File does not exist: ") + sFileName);
				else if ( err == EACCES )
					FIRTEX_THROW3(FILEIO_ERROR,_T("File Access denied: ") + sFileName);
				else if ( err == EMFILE )
					FIRTEX_THROW3(FILEIO_ERROR,_T("Too many open files: ") + sFileName);
			}				
			else 
			{
				m_nLength = fileSize(m_hFile);			
				void* address = ::mmap(0, m_nLength, PROT_READ, MAP_SHARED, m_hFile, 0);
				if (address == MAP_FAILED)
				{
					FIRTEX_THROW3(FILEIO_ERROR,_T("Map failed."));
				}
				else
				{
					m_pData = (byte*)address;
				}			
			}
#endif
			m_filename = _path;
		}
		
		void CMMapFile::close()
		{			
#ifdef WIN32
			if ( m_pData != NULL )
			{
				if ( ! UnmapViewOfFile(m_pData) )
				{
					FIRTEX_ASSERT( false, _T("UnmapViewOfFile(m_pData) failed"));
				}
			}

			if (m_hMap != NULL )
			{
				if ( ! CloseHandle(m_hMap) )
				{
					FIRTEX_ASSERT( false, _T("CloseHandle(mmaphandle) failed"));
				}
			}
			if ( m_hFile != NULL )
			{
				if ( !CloseHandle(m_hFile) )
				{
					FIRTEX_ASSERT( false, _T("CloseHandle(fhandle) failed"));
				}
			}
			m_hMap = NULL;
			m_hFile = NULL;
#else
			if ( m_pData != NULL )
				::munmap(m_pData, m_nLength);
			if ( m_hFile > 0 )
				::fileClose(m_hFile);
			m_hFile = 0;
#endif			
			m_pData = NULL;
			m_nPos = 0;
		}			
	}
}

