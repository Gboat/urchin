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
// Email	: ruijieguo@software.ict.ac.cn
// Created	: 2006/6/10
//
#include "index/DateTimeTermReader.h"
#include "index/DateTimeTermReaderImpl.h"
#include "index/BarrelTermDocFreqs.h"
#include "index/BarrelTermPositions.h"
#include "index/DateTimeTermIterator.h"

#include "index/DictHashTermReader.h"
#include "index/BarrelTermDocFreqs.h"
#include "index/BarrelTermPositions.h"
#include "index/InMemoryTermReader.h"
#include "index/DictHashIndexer.h"
#include "index/DictHashTermIterator.h"
#include "index/InputDescriptor.h"

namespace firtex
{
	namespace index
	{
		CDateTimeTermReader::CDateTimeTermReader(void) 
			: m_pCurTermInfo(NULL)			
			, m_bReaderImplOwner(false)
			, m_pReaderImpl(NULL)
		{			
		}
		CDateTimeTermReader::CDateTimeTermReader(CDateTimeTermReaderImpl* pReaderImpl)
			: CTermReader(pReaderImpl->getFieldInfo(),pReaderImpl->getFilter())
			, m_pCurTermInfo(NULL)
			, m_bReaderImplOwner(false)
			, m_pReaderImpl(pReaderImpl)
		{

		}
		CDateTimeTermReader::CDateTimeTermReader(CDirectory* pDirectory,const tchar* barrelname,CFieldInfo* pFieldInfo,CBitVector* pDocFilter)
			: CTermReader(pDocFilter)			
			, m_pCurTermInfo(NULL)
			, m_bReaderImplOwner(false)
			, m_pReaderImpl(NULL)
		{		
			open(pDirectory,barrelname,pFieldInfo,pDocFilter);
		}

		CDateTimeTermReader::~CDateTimeTermReader(void)
		{			
			close();

			if(m_bReaderImplOwner && m_pReaderImpl)
			{
				delete m_pReaderImpl;
				m_pReaderImpl = NULL;
			}
		}

		void CDateTimeTermReader::open(CDirectory* pDirectory,const tchar* barrelname,CFieldInfo* pFieldInfo,CBitVector* pDocFilter)
		{
			CTermReader::open(pDirectory,barrelname,pFieldInfo,pDocFilter);

			close();

			if(!m_pReaderImpl)
			{
				m_pReaderImpl = new CDateTimeTermReaderImpl(pDirectory,barrelname,pFieldInfo,pDocFilter);
				m_bReaderImplOwner = true;
			}
			m_pReaderImpl->open(pDirectory,barrelname,pFieldInfo,pDocFilter);
		}

		CTermIterator* CDateTimeTermReader::termIterator(CTerm* pLowerTerm,CTerm* pUpperTerm)
		{
			return m_pReaderImpl->termIterator(pLowerTerm,pUpperTerm);
		}

		CTermIterator* CDateTimeTermReader::termIterator(const tchar* field)
		{
			return m_pReaderImpl->termIterator(field);
		}

		bool CDateTimeTermReader::seek(CTerm* term)
		{			
			m_pCurTermInfo = m_pReaderImpl->termInfo(term);
			if(m_pCurTermInfo)
				return true;			
			return false;
		}

		CTermReader* CDateTimeTermReader::clone()
		{
			return new CDateTimeTermReader(m_pReaderImpl);
		}

		CTermDocFreqs* CDateTimeTermReader::termDocFreqs()
		{
			if(m_pCurTermInfo == NULL)
				return NULL;
			CInputDescriptor des(NULL,m_pReaderImpl->m_pInputDescriptor->getDPostingInput(),NULL,false);
			CTermDocFreqs* pTermDocs = new CBarrelTermDocFreqs(this,&des,*m_pCurTermInfo,m_pDocFilter);
			return pTermDocs;
		}

		CTermPositions*	CDateTimeTermReader::termPositions()
		{
			return NULL;
		}
		freq_t CDateTimeTermReader::docFreq(CTerm* term)
		{
			return m_pReaderImpl->docFreq(term);
		}
		
		void CDateTimeTermReader::close()
		{
			m_pCurTermInfo = NULL;
		}	

		//////////////////////////////////////////////////////////////////////////
		///CDateTimeInMemoryTermReader
		CDateTimeInMemoryTermReader::CDateTimeInMemoryTermReader(void)
			: m_pMapDateTime(NULL)			
			, m_pCurTermInfo(NULL)
			, m_pCurPosting(NULL)
			, m_pTermInfo(NULL)
		{			
		}
		CDateTimeInMemoryTermReader::CDateTimeInMemoryTermReader(const tchar* field,DateTimeMap* pDTM,CBitVector* pDocFilter)
			: m_pMapDateTime(pDTM)			
			, m_pCurTermInfo(NULL)
			, m_pCurPosting(NULL)
			, m_sField(field)
			, m_pTermInfo(NULL)
		{				
			setFilter(pDocFilter);
		}

		CDateTimeInMemoryTermReader::~CDateTimeInMemoryTermReader(void)
		{			
			close();						
		}

		CTermIterator* CDateTimeInMemoryTermReader::termIterator(CTerm* pLowerTerm,CTerm* pUpperTerm)
		{
			if(!pLowerTerm && !pUpperTerm)
				FIRTEX_THROW3(ILLEGALARGUMENT_ERROR,_T("At least one term must be non-null."));
			if(pLowerTerm && pUpperTerm && _tcscmp(pLowerTerm->getField(),pUpperTerm->getField()))
				FIRTEX_THROW3(ILLEGALARGUMENT_ERROR,_T("Both terms must be for the same field"));
			CDateTimeInMemoryTermIterator* pTermIterator = new CDateTimeInMemoryTermIterator(this,pLowerTerm,pUpperTerm);
			if(pLowerTerm && pTermIterator->skipTo(pLowerTerm) == false)
			{
				delete pTermIterator;
				pTermIterator = NULL;
			}
			return pTermIterator;					
		}

		CTermIterator* CDateTimeInMemoryTermReader::termIterator(const tchar* field)
		{				
			if((field != NULL) && (m_sField != field))
				return NULL;			
			return new CDateTimeInMemoryTermIterator(this);		
		}

		bool CDateTimeInMemoryTermReader::seek(CTerm* term)
		{			
			m_pCurTermInfo = termInfo(term);
			if(m_pCurTermInfo)
				return true;			
			return false;
		}

		CTermDocFreqs* CDateTimeInMemoryTermReader::termDocFreqs()
		{
			if(m_pCurTermInfo == NULL)
				return NULL;			
			CTermDocFreqs* pTermDocs = new CBarrelTermDocFreqs(this,m_pCurPosting,*m_pCurTermInfo,getFilter());
			return pTermDocs;		
		}

		CTermPositions*	CDateTimeInMemoryTermReader::termPositions()
		{
			return NULL;	
		}
		freq_t CDateTimeInMemoryTermReader::docFreq(CTerm* term)
		{
			CTermInfo* ti = termInfo(term);
			if(ti)
			{
				return ti->docFreq();
			}
			return 0;
		}

		CTermInfo* CDateTimeInMemoryTermReader::termInfo(CTerm* term)
		{			
			if(_tcscmp(term->getField(),m_sField.c_str()))
				return NULL;
			CTermX<int64_t>* pTermX = (CTermX<int64_t>*)term->asTermX();			

			int64_t tid = pTermX->getValue();
			if(tid < 0)
				return NULL;
			DateTimeMap::iterator iter = m_pMapDateTime->find(tid);
			if(iter == m_pMapDateTime->end())
			{
				return NULL;
			}
			m_pCurPosting = iter->second;
			if(!m_pCurPosting || (m_pCurPosting->hasNoChunk() == true))
				return NULL;
			if(!m_pTermInfo)
				m_pTermInfo = new CTermInfo;
			m_pTermInfo->set(m_pCurPosting->docFreq(),-1);
			return m_pTermInfo;
		}

		CInMemoryPosting* CDateTimeInMemoryTermReader::inMemoryPosting()
		{
			if(!m_pCurPosting || (m_pCurPosting->hasNoChunk() == true))
				return NULL;
			return m_pCurPosting;
		}

		void CDateTimeInMemoryTermReader::close()
		{
			if(m_pTermInfo)
			{
				delete m_pCurTermInfo;
				m_pCurTermInfo = NULL;
			}
			m_pCurTermInfo = NULL;
		}
		CTermReader*	CDateTimeInMemoryTermReader::clone()
		{
			return new CDateTimeInMemoryTermReader(m_sField.c_str(),m_pMapDateTime,getFilter());
		}
	}
}


