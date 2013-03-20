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
// Created	: 2006/11/29
//
#include "index/InMemoryIndexBarrelReader.h"
#include "index/IndexBarrelWriter.h"
#include "index/MultiFieldTermReader.h"
#include "index/NormsReader.h"
#include "index/Index.h"

namespace firtex
{
	namespace index
	{
		CInMemoryIndexBarrelReader::CInMemoryIndexBarrelReader(CIndexBarrelWriter*	pIndexBarrelWriter)
			:m_pDeletedDocs(pIndexBarrelWriter->getDocFilter())			
			,m_pIndexBarrelWriter(pIndexBarrelWriter)
			,m_pNormsReader(NULL)
		{
			if(m_pIndexBarrelWriter->m_pFieldsInfo->numIndexFields() == 1)
			{
				for (int32_t i = 0;i<m_pIndexBarrelWriter->m_numIndexers;i++)
				{
					if(m_pIndexBarrelWriter->m_fieldIndexers[i])
					{
						m_pTermReader = m_pIndexBarrelWriter->m_fieldIndexers[i]->termReader();
						m_pTermReader->setFilter(m_pDeletedDocs);
						break;
					}
				}
			}
			else if(m_pIndexBarrelWriter->m_pFieldsInfo->numIndexFields() > 1)
			{
				m_pTermReader = new CMultiFieldTermReader(m_pDeletedDocs);
				for (int32_t i = 0;i<m_pIndexBarrelWriter->m_numIndexers;i++)
				{
					if(m_pIndexBarrelWriter->m_fieldIndexers[i])
					{						
						((CMultiFieldTermReader*)m_pTermReader)->addTermReader( (*m_pIndexBarrelWriter->m_pFieldsInfo)[i]->getName(),m_pIndexBarrelWriter->m_fieldIndexers[i]->termReader());						
						m_pTermReader->setFilter(m_pDeletedDocs);
					}
				}
			}
			else 
			{
				FIRTEX_CLOG(level::warn) << _T("No in-memory field indexer.") << FIRTEX_ENDL;
			}
		}

		CInMemoryIndexBarrelReader::~CInMemoryIndexBarrelReader(void)
		{
			close();
			
			m_pDeletedDocs = NULL;
			
			if(m_pTermReader)
			{
				delete m_pTermReader;
				m_pTermReader = NULL;
			}
			if(m_pNormsReader)
			{
				delete m_pNormsReader;
				m_pNormsReader = NULL;
			}
			m_pIndexBarrelWriter = NULL;
		}

		document::CDocument* CInMemoryIndexBarrelReader::document(docid_t docID)
		{
			if(m_pDeletedDocs && m_pDeletedDocs->test((size_t)docID))
				FIRTEX_THROW2(ILLEGALARGUMENT_ERROR,_T("attempt to access a deleted document"));
			document::CDocument* pDoc = m_pIndexBarrelWriter->m_pStoredFieldsWriter->document(docID);
			pDoc->setDocID(docID);
			return pDoc;
		}
		
		document::CField* CInMemoryIndexBarrelReader::field(docid_t docID,fieldid_t fid)
		{
			if(m_pDeletedDocs && m_pDeletedDocs->test((size_t)docID))
				FIRTEX_THROW2(ILLEGALARGUMENT_ERROR,_T("attempt to access a deleted document"));
			return m_pIndexBarrelWriter->m_pStoredFieldsWriter->field(docID,fid);						
		}
		
		bool CInMemoryIndexBarrelReader::deleteDocument(docid_t docID)
		{
			if(m_pDeletedDocs == NULL)
			{
				m_pDeletedDocs = new CBitVector((size_t)(docID + 1));
				m_pIndexBarrelWriter->setDocFilter(m_pDeletedDocs);
				if(m_pTermReader)
					m_pTermReader->setFilter(m_pDeletedDocs);
			}
			else if(m_pDeletedDocs->test(docID))
				return false;
			m_pDeletedDocs->set(docID);
			return true;
		}		
		void CInMemoryIndexBarrelReader::undeleteAll()
		{
			if(m_pDeletedDocs)
			{
				delete m_pDeletedDocs;
				m_pDeletedDocs = NULL;
				m_pIndexBarrelWriter->setDocFilter(NULL);

				if(m_pTermReader)
					m_pTermReader->setFilter(NULL);

				tstring str = m_pIndexBarrelWriter->m_barrelName + _T(".del");
				if(m_pIndex->getDirectory()->fileExists(str.c_str()))
					m_pIndex->getDirectory()->deleteFile(str.c_str());
			}
		}		
		bool CInMemoryIndexBarrelReader::isDeleted(docid_t docID)
		{
			if(m_pDeletedDocs)
			{
				return m_pDeletedDocs->test((size_t)docID);
			}
			else return false;			
		}
		
		bool CInMemoryIndexBarrelReader::hasDeletions()
		{
			if(m_pDeletedDocs == NULL)
				return false;

			return m_pDeletedDocs->any();
			
		}
		count_t CInMemoryIndexBarrelReader::numDeletedDocs()
		{
			if(m_pDeletedDocs == NULL)
				return 0;
			return (count_t)m_pDeletedDocs->count();
		}
		
		CTermReader* CInMemoryIndexBarrelReader::termReader()
		{
			return m_pTermReader->clone();
		}
		CTermReader* CInMemoryIndexBarrelReader::termReader(const tchar* field)
		{
			if(m_pTermReader == NULL)
				return NULL;
			CFieldsInfo* pFieldsInfo = m_pIndexBarrelWriter->getFieldsInfo();
			if(pFieldsInfo->numIndexFields() > 1)
				return ((CMultiFieldTermReader*)m_pTermReader)->termReader(field);
			else 
			{
				pFieldsInfo->startIterator();
				CFieldInfo* pFieldInfo;
				while (pFieldsInfo->hasNext())
				{
					pFieldInfo = pFieldsInfo->next();
					if(pFieldInfo->isIndexed())
					{							
						if(!_tcsicmp(field,pFieldInfo->getName()))
							return m_pTermReader->clone();
					}
				}						
			}
			return NULL;
		}
		
		CForwardIndexReader* CInMemoryIndexBarrelReader::getForwardIndexReader()
		{
			return NULL;
		}
		
		CNormsBytes* CInMemoryIndexBarrelReader::norms(const tchar* field)
		{
			if(m_pNormsReader == NULL)
				m_pNormsReader = m_pIndexBarrelWriter->m_pNormsWriter->normsReader(m_pIndexBarrelWriter->getFieldsInfo());
			FIRTEX_ASSERT(m_pNormsReader,_T("Get norms reader failed."));
			return m_pNormsReader->getNorms(field);
		}
		
		CNormsBytes*	CInMemoryIndexBarrelReader::norms(fieldid_t fid)
		{
			if(m_pNormsReader == NULL)
				m_pNormsReader = m_pIndexBarrelWriter->m_pNormsWriter->normsReader(m_pIndexBarrelWriter->getFieldsInfo());
			FIRTEX_ASSERT(m_pNormsReader,_T("Get norms reader failed."));
			return m_pNormsReader->getNorms(fid);			
		}
		
		uint64_t CInMemoryIndexBarrelReader::numTerms()
		{
			CFieldsInfo* pFieldsInfo = m_pIndexBarrelWriter->m_pFieldsInfo;
			uint64_t totalTerms = 0;
			CFieldInfo* pFieldInfo;
			pFieldsInfo->startIterator();
			while (pFieldsInfo->hasNext())
			{
				pFieldInfo = pFieldsInfo->next();
				if(FIELDBITS(pFieldInfo->getFlag())&BIT_INDEX_MASK)
					totalTerms += pFieldInfo->numTerms();
			}
			return totalTerms;
		}
		
		CFieldsInfo& CInMemoryIndexBarrelReader::getFieldsInfo()
		{			
			FIRTEX_ASSERT(m_pIndexBarrelWriter->m_pFieldsInfo,_T("Get fields info failed."));
			return (*m_pIndexBarrelWriter->m_pFieldsInfo);
		}
		
		void CInMemoryIndexBarrelReader::close()
		{			
		}
	}
}

