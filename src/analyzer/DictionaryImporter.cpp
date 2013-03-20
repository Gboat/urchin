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
// Created	: 2007/4/14
//

#include "analyzer/DictionaryImporter.h"
#include "utility/Misc.h"

namespace firtex
{
	namespace analyzer
	{
		CDictionaryImporter::CDictionaryImporter(void)
			:m_pAnteFilter(NULL)
			,m_pBuffers(NULL)
			,m_pTokenVector(NULL)
		{
		}

		CDictionaryImporter::~CDictionaryImporter(void)
		{
			if(m_pAnteFilter)
			{
				delete m_pAnteFilter;
				m_pAnteFilter = NULL;
			}
			if(m_pBuffers)
			{
				delete m_pBuffers;
				m_pBuffers = NULL;				
			}
			if(m_pTokenVector)
			{
				delete m_pTokenVector;
				m_pTokenVector = NULL;
			}			
			dictionary::iterator iter = m_dictionary.begin();
			while (iter != m_dictionary.end())
			{
				delete (*iter);				
				iter++;
			}
			m_dictionary.clear();
		}
		void CDictionaryImporter::attachAnteFilter(CTokenFilter* pAnteTokenFilter)
		{
			if(m_pAnteFilter)
				m_pAnteFilter->attachFilter(pAnteTokenFilter);
			else 
				m_pAnteFilter = pAnteTokenFilter;
		}

		CTokenFilter* CDictionaryImporter::detachAnteFilter()
		{
			CTokenFilter* pAnteFilter = m_pAnteFilter;
			m_pAnteFilter = NULL;
			return pAnteFilter;
		}					
		void CDictionaryImporter::import(const tchar* szDictionary)
		{
			int fileHandle = -1;
			try
			{
				fileHandle = ::fileOpen(szDictionary, O_BINARY | O_RDONLY | O_RANDOM, _S_IREAD );
				if (fileHandle < 0)
				{
					int err = errno;
					tstring sFileName = szDictionary;
					if ( err == ENOENT )
						FIRTEX_THROW2(FILEIO_ERROR,_T("File does not exist: ") + sFileName);
					else if ( err == EACCES )
						FIRTEX_THROW2(FILEIO_ERROR,_T("File Access denied: ") + sFileName);
					else if ( err == EMFILE )
						FIRTEX_THROW2(FILEIO_ERROR,_T("Too many open files: ") + sFileName);
				}				
				size_t nBufSize = (size_t)fileSize(fileHandle) + 1;
				if(nBufSize <= 1)
					return;
				char* pBuffer = NULL;
				if(!m_pBuffers)
				{
					m_pBuffers = new CBuffers(nBufSize);				
					pBuffer = m_pBuffers->m_pBuffer;
				}
				else 
				{
					pBuffer = m_pBuffers->newBuffer(nBufSize);
				}
				size_t ret = ::fileRead(fileHandle,pBuffer,(unsigned int)nBufSize - 1);
				if(ret == 0)
				{
					tstring sFileName = szDictionary;
					FIRTEX_THROW3(FILEIO_ERROR,_T("read past EOF:") + sFileName);
				}
				else if (ret == -1)
				{
					int err = errno;					
					tstring sFileName = szDictionary;
					FIRTEX_THROW3(FILEIO_ERROR,_T("file IO read error:") + sFileName);
				}
				::fileClose(fileHandle);
				pBuffer[ret] = 0;
				if(!m_pTokenVector)
				{
					m_pTokenVector = new CTokens(pBuffer,ret);
				}
				else
				{
					m_pTokenVector->setTokenBuffer(pBuffer,ret);
				}
				if(m_pAnteFilter)
				{
					CTokens* pTokenVector = m_pAnteFilter->filter(m_pTokenVector);
					
					char* pBuf = pTokenVector->getTokenBuffer();
					size_t leng = pTokenVector->getTokenBufferSize();

					nBufSize = leng + 1;
					pBuffer = m_pBuffers->newBuffer(nBufSize);
					memcpy(pBuffer,pBuf,leng);
					pBuffer[leng] = 0;
				}				
				_import(pBuffer,nBufSize - 1);
			}
			catch (std::bad_alloc& e)
			{				
				if(fileHandle > 0)
					::fileClose(fileHandle);
				FIRTEX_THROW3(OUTOFMEM_ERROR,e.what());
			}
		}

#define DICT_SKIP_BLANK(p)\
	while(*p == '\n' || *p == '\r' || *p == ' ' || *p == '\t')\
	p++;
#define DICT_SKIP_BLANK_BACK(b,p)\
	while( p > b && (*p == '\n' || *p == '\r' || *p == ' ' || *p == '\t'))\
		p--;
		void CDictionaryImporter::_import(char* buf,size_t len)
		{
			char* pWordBeg;
			char* pWordEnd = buf;
			while (pWordEnd < buf + len)
			{
				pWordBeg = pWordEnd;
				DICT_SKIP_BLANK(pWordBeg);
				pWordEnd = pWordBeg;
				while (*pWordEnd != '\n' && (pWordEnd < buf + len))
				{
					pWordEnd++;
				}
				if(pWordEnd >= buf + len)
					pWordEnd--;

				DICT_SKIP_BLANK_BACK(pWordBeg,pWordEnd);
				if(*pWordBeg && pWordEnd >= pWordBeg)
				{
					str_t* pWord = new str_t;
					pWord->str = pWordBeg;
					pWord->length = pWordEnd - pWordBeg + 1;					
					m_dictionary.insert(pWord);
				}
				pWordEnd++;
			}			
		}

		void CDictionaryImporter::exportAs(const tchar* szDict)
		{
			int fileHandle;
			if(CMisc::dirExists(szDict))
				fileHandle = ::fileOpen( szDict, O_BINARY | O_RDWR | O_RANDOM | O_TRUNC, _S_IREAD | _S_IWRITE);
			else 
				fileHandle = ::fileOpen( szDict, O_BINARY | O_RDWR | O_RANDOM | O_CREAT, _S_IREAD | _S_IWRITE);			
			if ( fileHandle < 0 )
			{
				int err = errno;
				tstring sFileName = szDict;
				if ( err == ENOENT )
					FIRTEX_THROW2(FILEIO_ERROR,_T("File does not exist:") + sFileName);
				else if ( err == EACCES )
					FIRTEX_THROW2(FILEIO_ERROR,_T("File Access denied:") + sFileName);
				else if ( err == EMFILE )
					FIRTEX_THROW2(FILEIO_ERROR,_T("Too many open files:") + sFileName);
			}	
			dictionary::iterator iter = m_dictionary.begin();
			char szEnter[] = "\r\n";
			const str_t* pWord = NULL;
			while (iter != m_dictionary.end())
			{
				pWord = (*iter);				
				::fileWrite(fileHandle,pWord->str,(unsigned int)pWord->length);
				::fileWrite(fileHandle,szEnter,2);
				iter++;
			}
			::fileClose(fileHandle);
		}
	}
}

