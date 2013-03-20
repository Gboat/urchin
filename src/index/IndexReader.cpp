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
// Created	: 2005/4/12
// Modified	: 2006/10/11 changed to access index data directly,not by CIndex object.-by GuoRuijie
//

#include "index/IndexReader.h"
#include "index/TermReader.h"
#include "index/Index.h"
#include "index/MultiIndexBarrelReader.h"
#include "index/NormBytes.h"

namespace firtex
{
	namespace index
	{
		CIndexReader::CIndexReader(void)
			:m_pTermReader(NULL)
			,m_pIndex(NULL)
			,m_pBarrelsInfo(NULL)
			,m_pBarrelReader(NULL)
		{
		}
		
		CIndexReader::CIndexReader(CIndex* pIndex)
			:m_pIndex(pIndex)
			,m_pTermReader(NULL)
			,m_pBarrelsInfo(NULL)
			,m_pBarrelReader(NULL)
		{			
			m_pBarrelsInfo = m_pIndex->m_pBarrelsInfo;
		}

		CIndexReader::~CIndexReader(void)
		{
			close();
		}
		
		bool CIndexReader::isDeleted(docid_t docID)
		{
			getBarrelReader();

			if((docID >= 0) && (docID < maxDoc()))
				return m_pBarrelReader->isDeleted(docID);
			else return false;
		}

		bool CIndexReader::hasDeletions()
		{			
			return (m_pBarrelsInfo->numDeletedDocs() > 0);			
		}
		bool CIndexReader::deleteDocument(docid_t docID)
		{
			getBarrelReader();
			
			if((docID >= 0) && (docID < maxDoc()))
			{
				bool bDel = m_pBarrelReader->deleteDocument(docID);
				if(bDel)
				{
					if(m_pBarrelsInfo->getBarrelCount() == 1)
                        m_pBarrelsInfo->getLastBarrel()->deleteDocument();
                    m_pIndex->setDirty(true);
				}
				return bDel;
			}
			return false;
		}
		
		void CIndexReader::undeleteAll()
		{
			getBarrelReader();

			if(m_pBarrelsInfo->numDeletedDocs() > 0)
			{
				m_pBarrelReader->undeleteAll();
				m_pBarrelsInfo->undeleteDocuments();
				m_pIndex->setDirty(true);
			}			
		}
		count_t CIndexReader::numDeletedDocs()
		{
			if(m_pBarrelsInfo)
				return m_pBarrelsInfo->numDeletedDocs();
			return 0;
		}

		count_t CIndexReader::numDocs()
		{			
			return m_pBarrelsInfo->getDocCount() - m_pBarrelsInfo->numDeletedDocs();	
		}

		uint64_t CIndexReader::getNumTerms()
		{
			return getBarrelReader()->numTerms();			
		}

		void CIndexReader::createBarrelReader()
		{
			FIRTEX_ASSERT((m_pIndex!=NULL),"The index has not opened yet.");
			
			if(m_pBarrelReader)
				delete m_pBarrelReader;

			int32_t bc = m_pBarrelsInfo->getBarrelCount();
			CBarrelInfo* pLastBarrel = m_pBarrelsInfo->getLastBarrel();
			if( (bc > 0) && pLastBarrel)
			{
				if(pLastBarrel->getDocCount() <= 0)///skip empty barrel
					bc--;
					pLastBarrel = (*m_pBarrelsInfo)[bc - 1];
			}

			if(bc == 1)
			{
				if(pLastBarrel && pLastBarrel->getWriter())
					m_pBarrelReader = pLastBarrel->getWriter()->inMemoryReader();
				else 
					m_pBarrelReader = new CSingleIndexBarrelReader(m_pIndex,pLastBarrel);
			}
			else if(bc > 1)
			{
				m_pBarrelReader = new CMultiIndexBarrelReader(m_pIndex,m_pBarrelsInfo);				
			}
			else 
				FIRTEX_THROW2(INDEX_COLLAPSE_ERROR,_T("the index barrel number is 0."));
			m_pIndex->setDirty(false);///clear dirty flag
		}
		
		CTermReader* CIndexReader::termReader()
		{
			if(m_pBarrelReader == NULL)
			{
				createBarrelReader();
			}
			if(m_pTermReader)
				return m_pTermReader->clone();
			m_pTermReader = m_pBarrelReader->termReader();
			return m_pTermReader->clone();
		}		
		
		CTermReader* CIndexReader::getTermReader()
		{

			if(m_pBarrelReader == NULL)
			{
				createBarrelReader();
			}

			if(m_pTermReader)
				return m_pTermReader;
			m_pTermReader = m_pBarrelReader->termReader();
			return m_pTermReader;
		}
		
		CNormsBytes* CIndexReader::norms(fieldid_t fieldID)
		{			
			getBarrelReader();
			return m_pBarrelReader->norms(fieldID);
		}
		CNormsBytes*	CIndexReader:: norms(const tchar* field)
		{			
			getBarrelReader();
			return m_pBarrelReader->norms(field);
		}
		
		void CIndexReader::close()
		{
			if(m_pBarrelReader)
			{
				delete m_pBarrelReader;
				m_pBarrelReader = NULL;
			}
			m_pTermReader = NULL;
		}
		
		
		int64_t CIndexReader::lastModified()
		{
			FIRTEX_ASSERT((m_pIndex!=NULL),_T("The index has not opened yet."));
			return m_pIndex->getDirectory()->fileModified(BARRELS_INFONAME);
		}
		int64_t CIndexReader::lastModified(CDirectory* pDirectory)
		{
			return pDirectory->fileModified(BARRELS_INFONAME);
		}
		tstring CIndexReader::getVersion()
		{			
			FIRTEX_ASSERT((m_pIndex!=NULL),_T("The index has not opened yet."));
			return m_pIndex->getVersion();
		}		

		count_t CIndexReader::maxDoc()
		{
			FIRTEX_ASSERT((m_pIndex!=NULL),_T("The index has not opened yet."));
			return m_pBarrelsInfo->getDocCount();
		}				

		freq_t CIndexReader::docFreq(CTerm* term)
		{			
			return getTermReader()->docFreq(term);
		}
		CTermInfo* CIndexReader::termInfo(CTerm* term)
		{			
			return getTermReader()->termInfo(term);
		}

		CIndexBarrelReader* CIndexReader::getBarrelReader()
		{
			if(m_pBarrelReader == NULL)
			{
				createBarrelReader();
			}
			return m_pBarrelReader;
		}
		CFieldsInfo& CIndexReader::getFieldsInfo()
		{
			getBarrelReader();
			FIRTEX_ASSERT((m_pBarrelReader!=NULL),_T("Create barrels reader error."));
			return m_pBarrelReader->getFieldsInfo();
		}
		CForwardIndexReader* CIndexReader::getForwardIndexReader()
		{			
			getBarrelReader();
			return m_pBarrelReader->getForwardIndexReader();
		}

		void CIndexReader::lastModified(CDateTime& dt)
		{
			dt = lastModified();
		}
		void CIndexReader::lastModified(CDirectory* pDirectory,CDateTime& dt)
		{
			dt = lastModified(pDirectory);
		}		
	}
}

