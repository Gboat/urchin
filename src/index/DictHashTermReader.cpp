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
// Created	: 2006/2/23
//
#include "index/DictHashTermReader.h"
#include "index/BarrelTermDocFreqs.h"
#include "index/BarrelTermPositions.h"
#include "index/InMemoryTermReader.h"
#include "index/DictHashIndexer.h"
#include "index/DictHashTermIterator.h"
#include "index/InputDescriptor.h"
#include "index/DictHashTermReaderImpl.h"

namespace firtex
{
	namespace index
	{
		CDictHashTermReader::CDictHashTermReader(void) 
			: m_pCurTermInfo(NULL)			
			, m_pTermReaderImpl(NULL)
			, m_bTermReaderImplOwner(false)
		{			
		}
		CDictHashTermReader::CDictHashTermReader(CDictHashTermReaderImpl* pImpl)
			: CTermReader(pImpl->getFieldInfo(),pImpl->getFilter())
			, m_pTermReaderImpl(pImpl)
			, m_pCurTermInfo(NULL)
			, m_bTermReaderImplOwner(false)
		{

		}
		CDictHashTermReader::CDictHashTermReader(CDirectory* pDirectory,const tchar* barrelname,CFieldInfo* pFieldInfo,CBitVector* pDocFilter)
			: CTermReader(pDocFilter)			
			, m_pTermReaderImpl(NULL)
			, m_pCurTermInfo(NULL)
			, m_bTermReaderImplOwner(false)
		{		
			open(pDirectory,barrelname,pFieldInfo,pDocFilter);
		}

		CDictHashTermReader::~CDictHashTermReader(void)
		{			
			close();		
			if(m_bTermReaderImplOwner && m_pTermReaderImpl)
			{
				delete m_pTermReaderImpl;				
			}
			m_pTermReaderImpl = NULL;
		}
		
		void CDictHashTermReader::open(CDirectory* pDirectory,const tchar* barrelname,CFieldInfo* pFieldInfo,CBitVector* pDocFilter)
		{
			CTermReader::open(pDirectory,barrelname,pFieldInfo,pDocFilter);

			close();
			
			if(!m_pTermReaderImpl)
			{
				m_pTermReaderImpl = new CDictHashTermReaderImpl(pDirectory,barrelname,pFieldInfo,pDocFilter);
				m_bTermReaderImplOwner = true;
			}
			m_pTermReaderImpl->open(pDirectory,barrelname,pFieldInfo,pDocFilter);
		}

		CTermIterator* CDictHashTermReader::termIterator(CTerm* pLowerTerm,CTerm* pUpperTerm)
		{			
			return m_pTermReaderImpl->termIterator(pLowerTerm,pUpperTerm);
		}
		
		CTermIterator* CDictHashTermReader::termIterator(const tchar* field)
		{
			return m_pTermReaderImpl->termIterator(field);
		}

		bool CDictHashTermReader::seek(CTerm* term)
		{			
			m_pCurTermInfo = m_pTermReaderImpl->termInfo(term);
			if(m_pCurTermInfo)
				return true;			
			return false;
		}

		CTermDocFreqs* CDictHashTermReader::termDocFreqs()
		{
			if(m_pCurTermInfo == NULL)
				return NULL;
			CInputDescriptor des(NULL,m_pTermReaderImpl->m_pInputDescriptor->getDPostingInput(),NULL,false);
			CTermDocFreqs* pTermDocs = new CBarrelTermDocFreqs(this,&des,*m_pCurTermInfo,m_pDocFilter);
			return pTermDocs;
		}

		CTermPositions*	CDictHashTermReader::termPositions()
		{			
			if(m_pCurTermInfo == NULL || m_pTermReaderImpl->m_pInputDescriptor->getPPostingInput() == NULL)
				return NULL;

			CInputDescriptor des(NULL,m_pTermReaderImpl->m_pInputDescriptor->getDPostingInput(),m_pTermReaderImpl->m_pInputDescriptor->getPPostingInput(),false);
			return new CBarrelTermPositions(this,&des,*m_pCurTermInfo,m_pDocFilter);
		}
		freq_t CDictHashTermReader::docFreq(CTerm* term)
		{
			return m_pTermReaderImpl->docFreq(term);
		}
		
		void CDictHashTermReader::close()
		{
			m_pCurTermInfo = NULL;
		}	

		CTermReader* CDictHashTermReader::clone()
		{
			return new CDictHashTermReader(m_pTermReaderImpl);
		}

		//////////////////////////////////////////////////////////////////////////
		///CDictHashInMemoryTermReader
		CDictHashInMemoryTermReader::CDictHashInMemoryTermReader(void)
			: m_pIndexer(NULL)			
			, m_pCurTermInfo(NULL)
			, m_pCurPosting(NULL)
			, m_pTermInfo(NULL)
		{			
		}
		CDictHashInMemoryTermReader::CDictHashInMemoryTermReader(const tchar* field,CDictHashIndexer* pIndexer,CBitVector* pDocFilter)
			: m_pIndexer(pIndexer)			
			, m_pCurTermInfo(NULL)
			, m_pCurPosting(NULL)
			, m_sField(field)
			, m_pTermInfo(NULL)
		{				
			setFilter(pDocFilter);
		}

		CDictHashInMemoryTermReader::~CDictHashInMemoryTermReader(void)
		{			
			close();						
		}
		
		CTermIterator* CDictHashInMemoryTermReader::termIterator(CTerm* pLowerTerm,CTerm* pUpperTerm)
		{
			//if(m_field != term->getField())
			//	return NULL;
			//TODO:
			return NULL;
		}

		CTermIterator* CDictHashInMemoryTermReader::termIterator(const tchar* field)
		{				
			if((field != NULL) && (m_sField != field))
				return NULL;			
			return new CDictHashInMemoryTermIterator(this);		
		}

		bool CDictHashInMemoryTermReader::seek(CTerm* term)
		{			
			m_pCurTermInfo = termInfo(term);
			if(m_pCurTermInfo)
				return true;			
			return false;
		}

		CTermDocFreqs* CDictHashInMemoryTermReader::termDocFreqs()
		{
			if(m_pCurTermInfo == NULL)
				return NULL;

			CInMemoryPosting_WL* pInMemWL = (CInMemoryPosting_WL*)m_pCurPosting;
			SCOPED_LOCK(m_pIndexer->LOCK);
			pInMemWL->flushLastDoc(false);
			CTermDocFreqs* pTermDocs = new CBarrelTermDocFreqs(this,m_pCurPosting,*m_pCurTermInfo,getFilter());
			return pTermDocs;
		}

		CTermPositions*	CDictHashInMemoryTermReader::termPositions()
		{
			if(m_pCurTermInfo == NULL)
				return NULL;

			CInMemoryPosting_WL* pInMemWL = (CInMemoryPosting_WL*)m_pCurPosting;
			SCOPED_LOCK(m_pIndexer->LOCK);
			pInMemWL->flushLastDoc(false);
			return new CBarrelTermPositions(this,m_pCurPosting,*m_pCurTermInfo,getFilter());
		}
		freq_t CDictHashInMemoryTermReader::docFreq(CTerm* term)
		{
			CTermInfo* ti = termInfo(term);
			if(ti)
			{
				return ti->docFreq();
			}
			return 0;
		}

		CTermInfo* CDictHashInMemoryTermReader::termInfo(CTerm* term)
		{			
			if(_tcscmp(term->getField(),m_sField.c_str()))
				return NULL;
			CTermX<termid_t>* pTermX = (CTermX<termid_t>*)term->asTermX();			

			termid_t tid = pTermX->getValue();
			if(tid < 0)
				return NULL;
			m_pCurPosting = m_pIndexer->m_array.getAt(tid);
			if(!m_pCurPosting || (m_pCurPosting->hasNoChunk() == true))
				return NULL;
			if(!m_pTermInfo)
				m_pTermInfo = new CTermInfo;
			m_pTermInfo->set(m_pCurPosting->docFreq(),-1);
			return m_pTermInfo;
		}
		CInMemoryPosting* CDictHashInMemoryTermReader::inMemoryPosting()
		{
			if(!m_pCurPosting || (m_pCurPosting->hasNoChunk() == true))
				return NULL;
			return m_pCurPosting;
		}

		void CDictHashInMemoryTermReader::close()
		{
			if(m_pTermInfo)
			{
				delete m_pCurTermInfo;
				m_pCurTermInfo = NULL;
			}
			m_pCurTermInfo = NULL;
		}	

		CTermReader* CDictHashInMemoryTermReader::clone()
		{
			return new CDictHashInMemoryTermReader(m_sField.c_str(),m_pIndexer,getFilter());
		}
	}
}
