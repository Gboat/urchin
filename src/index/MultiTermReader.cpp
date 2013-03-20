//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: 郭瑞杰(GuoRuijie)
// Email	: ruijieguo@software.ict.ac.cn
// Created	: 2006/1/4
//
#include "index/MultiTermReader.h"
#include "index/MultiTermDocs.h"
#include "index/MultiTermPositions.h"
#include "index/MultiIndexBarrelReader.h"
#include "index/MultiTermIterator.h"
#include "utility/Config.h"

namespace firtex
{
	namespace index
	{
		CMultiTermReader::CMultiTermReader(void)
		{
		}

		CMultiTermReader::CMultiTermReader(CMultiIndexBarrelReader* pBarrelReader)
			: m_pBarrelReader(pBarrelReader)
			, m_pCurReader(NULL)
		{   			
		}
		CMultiTermReader::~CMultiTermReader(void)
		{
			m_pBarrelReader = NULL;
			close();
		}
		
		void CMultiTermReader::open(CDirectory* pDirectory,const tchar* barrelname,CFieldInfo* pFieldInfo,CBitVector* pDocFilter)
		{
			throw CUnsupportedOperationException(_T("Unsupported Operation : CMultiTermReader::open."));
		}

		CTermIterator* CMultiTermReader::termIterator(CTerm* pLowerTerm,CTerm* pUpperTerm)
		{
			if(!pLowerTerm && !pUpperTerm)
				FIRTEX_THROW3(ILLEGALARGUMENT_ERROR,_T("At least one term must be non-null."));
			if(pLowerTerm && pUpperTerm && _tcscmp(pLowerTerm->getField(),pUpperTerm->getField()))
				FIRTEX_THROW3(ILLEGALARGUMENT_ERROR,_T("Both terms must be for the same field"));
			tstring field = pLowerTerm?pLowerTerm->getField():pUpperTerm->getField();
			
			CReaderCache* pSeList = NULL;
			map<tstring,CReaderCache*>::iterator iter = m_ReaderCache.find(field);
			if(iter != m_ReaderCache.end())
			{
				pSeList = iter->second;
			}
			else 
			{
				//装入field域的搜索器
				pSeList = loadReader(field.c_str());
			}
			CMultiTermIterator* pTermIterator = new CMultiTermIterator();
			CTermIterator* pIt;
			while(pSeList)
			{
				pIt = pSeList->m_pTermReader->termIterator(pLowerTerm,pUpperTerm);
				if(pIt)
					pTermIterator->addIterator(pIt);
				pSeList = pSeList->m_next;
			}
			return pTermIterator;
		}		

		CTermIterator* CMultiTermReader::termIterator(const tchar* field)
		{			
			CReaderCache* pSeList = NULL;
			map<tstring,CReaderCache*>::iterator iter = m_ReaderCache.find(field);
			if(iter != m_ReaderCache.end())
			{
				pSeList = iter->second;
			}
			else 
			{
				//装入field域的搜索器
				pSeList = loadReader(field);
			}
			CMultiTermIterator* pTermIterator = new CMultiTermIterator();
			CTermIterator* pIt;
			while(pSeList)
			{
				pIt = pSeList->m_pTermReader->termIterator(field);
				if(pIt)
					pTermIterator->addIterator(pIt);
				pSeList = pSeList->m_next;
			}
			return pTermIterator;
		}

		bool CMultiTermReader::seek(CTerm* term)
		{
			bool bSuc = false;
			tstring field = term->getField();
			CReaderCache* pSeList = NULL;
			
			m_pCurReader = NULL;
			//先在cache里找
			map<tstring,CReaderCache*>::iterator iter = m_ReaderCache.find(field);
			if(iter != m_ReaderCache.end())
			{
				m_pCurReader = pSeList = iter->second;
			}
			else 
			{
				//装入field域的搜索器
				m_pCurReader = pSeList = loadReader(field.c_str());
			}			
			//挨个搜索器搜索
			while (pSeList)
			{
				bSuc = (pSeList->m_pTermReader->seek(term) || bSuc);
				pSeList = pSeList->m_next;
			}

			return bSuc;
		}
		
		CTermDocFreqs* CMultiTermReader::termDocFreqs()
		{			
			if(!m_pCurReader)
				return NULL;

			CMultiTermDocs* pTermDocs = new CMultiTermDocs();
			bool bAdd = false;
			CTermDocFreqs* pTmpTermDocs = NULL;
			CReaderCache* pList = m_pCurReader;
			while(pList)
			{
				pTmpTermDocs = pList->m_pTermReader->termDocFreqs();
				if(pTmpTermDocs)
				{
					pTermDocs->add(pList->m_barrelInfo,pTmpTermDocs);
					bAdd = true;
				}
				pList = pList->m_next;
			}
			if(bAdd == false)
			{
				delete pTermDocs;
				return NULL;
			}			
			return pTermDocs;
		}
		
		CTermPositions*	CMultiTermReader::termPositions()
		{
			if(!m_pCurReader)
				return NULL;

			CMultiTermPositions* pTermPositions = new CMultiTermPositions();
			bool bAdd = false;
			CTermPositions* pTmpTermPositions = NULL;
			CReaderCache* pList = m_pCurReader;
			while(pList)
			{
				pTmpTermPositions = pList->m_pTermReader->termPositions();
				if(pTmpTermPositions)
				{
					pTermPositions->add(pList->m_barrelInfo, pTmpTermPositions);
					bAdd = true;
				}
				pList = pList->m_next;
			}
			if(bAdd == false)
			{
				delete pTermPositions;
				return NULL;
			}			
			return pTermPositions;
		}

		freq_t CMultiTermReader::docFreq(CTerm* term)
		{
			freq_t df = 0;
			tstring field = term->getField();
			CReaderCache* pSeList = NULL;
			
			m_pCurReader = NULL;
			//先在cache里找
			map<tstring,CReaderCache*>::iterator iter = m_ReaderCache.find(field);
			if(iter != m_ReaderCache.end())
			{
				m_pCurReader = pSeList = iter->second;
			}
			else 
			{
				//装入field域的搜索器
				m_pCurReader = pSeList = loadReader(field.c_str());
			}			

			//挨个搜索器搜索
			while (pSeList)
			{
				df += pSeList->m_pTermReader->docFreq(term);
				pSeList = pSeList->m_next;
			}

			return df;
		}

		void CMultiTermReader::close()
		{		
			map<tstring,CReaderCache*>::iterator citer = m_ReaderCache.begin();
			while (citer != m_ReaderCache.end())
			{
				delete citer->second;
				citer++;
			}
			m_ReaderCache.clear();		
			m_pCurReader = NULL;
		}

		CTermReader* CMultiTermReader::clone()
		{
			return new CMultiTermReader(this->m_pBarrelReader);
		}

		CReaderCache* CMultiTermReader::loadReader(const tchar* field)
		{
			CReaderCache* pTailList = NULL;
			CReaderCache* pPreList = NULL;
			CReaderCache* pHeadList = NULL;
			CTermReader* pSe = NULL;
			m_pBarrelReader->startIterator();
			CBarrelReaderEntry* pEntry = NULL;
			while (m_pBarrelReader->hasNext())
			{
				pEntry = m_pBarrelReader->nextEntry();
				pSe =  (CTermReader*)pEntry->m_pBarrel->termReader(field);
				if(pSe)
				{
					pTailList = new CReaderCache(pEntry->m_pBarrelInfo,pSe);
					if(pHeadList == NULL)
					{
						pHeadList = pTailList;
						pPreList = pTailList;
					}
					else 
					{
						pPreList->m_next = pTailList;
						pPreList = pTailList;
					}
				}				
			}
			if(pHeadList)
			{
				m_ReaderCache.insert(pair<tstring,CReaderCache*>(field,pHeadList));
			}
			else FIRTEX_CLOG(firtex::utility::level::warn) << _T("No such field:") << field << FIRTEX_ENDL;
			return pHeadList;
		}		
	}
}
