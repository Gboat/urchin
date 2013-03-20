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
// Created	: 2005/11/27
//
#include "index/MultiIndexBarrelReader.h"
#include "index/MultiTermReader.h"
#include "index/MultiForwardIndexReader.h"
#include "index/Index.h"

namespace firtex
{
	namespace index
	{
		CMultiIndexBarrelReader::CMultiIndexBarrelReader(CIndex* pIndex,CBarrelsInfo* pBarrelsInfo)
			:CIndexBarrelReader(pIndex)
			,m_pBarrelsInfo(pBarrelsInfo)
			,m_pTermReader(NULL)
			,m_pTermVectorReader(NULL)
		{
			pBarrelsInfo->startIterator();
			CBarrelInfo* pBarrelInfo;
			while (pBarrelsInfo->hasNext())
			{
				pBarrelInfo = pBarrelsInfo->next();
				if(pBarrelInfo->getDocCount() > 0)
					addReader(pBarrelInfo);		
			}
		}

		CMultiIndexBarrelReader::~CMultiIndexBarrelReader(void)
		{
			m_pBarrelsInfo = NULL;
			close();
		}
		
		void CMultiIndexBarrelReader::open(const tchar* name)
		{
		}
		
		document::CDocument* CMultiIndexBarrelReader::document(docid_t docID)
		{
			CBarrelReaderEntry* pEntry;
			CBarrelInfo * pBarrelInfo;
			vector<CBarrelReaderEntry*>::iterator iter = m_readers.begin();
			document::CDocument* pDoc;
			while (iter != m_readers.end())
			{
				pEntry = (*iter);
				pBarrelInfo = pEntry->m_pBarrelInfo;
				if( (docID < pBarrelInfo->getMinDocID()) ||
					(docID >= (pBarrelInfo->getMinDocID() + pBarrelInfo->getDocCount())) )
                    iter++;				
				else 
				{
					pDoc = pEntry->m_pBarrel->document(docID - pBarrelInfo->getMinDocID());
					if(pDoc)
						pDoc->setDocID(docID);
					return pDoc;
				}
			}
			return NULL;
		}
		document::CField* CMultiIndexBarrelReader::field(docid_t docID,fieldid_t fid)
		{
			CBarrelReaderEntry* pEntry;
			CBarrelInfo * pBarrelInfo;
			vector<CBarrelReaderEntry*>::iterator iter = m_readers.begin();			
			while (iter != m_readers.end())
			{
				pEntry = (*iter);
				pBarrelInfo = pEntry->m_pBarrelInfo;
				if( (docID < pBarrelInfo->getMinDocID()) ||
					(docID >= (pBarrelInfo->getMinDocID() + pBarrelInfo->getDocCount())) )
					iter++;				
				else 
				{
					return pEntry->m_pBarrel->field(docID - pBarrelInfo->getMinDocID(),fid);					
				}
			}
			return NULL;
		}
		bool CMultiIndexBarrelReader::deleteDocument(docid_t docID)
		{
			CBarrelReaderEntry* pEntry;
			CBarrelInfo * pBarrelInfo;
			vector<CBarrelReaderEntry*>::iterator iter = m_readers.begin();
			while (iter != m_readers.end())
			{
				pEntry = (*iter);
				pBarrelInfo = pEntry->m_pBarrelInfo;
				if( (docID < pBarrelInfo->getMinDocID()) ||
					(docID >= (pBarrelInfo->getMinDocID() + pBarrelInfo->getDocCount())) )
				{
					iter++;				
				}
				else 
				{
					bool bSuc = pEntry->m_pBarrel->deleteDocument(docID - pBarrelInfo->getMinDocID());
					if(bSuc)
						pEntry->m_pBarrelInfo->deleteDocument();
					return bSuc;
				}
			}
			return false;
		}

		void CMultiIndexBarrelReader::undeleteAll()
		{
			CBarrelReaderEntry* pEntry;			
			vector<CBarrelReaderEntry*>::iterator iter = m_readers.begin();
			while (iter != m_readers.end())
			{
				pEntry = (*iter);
				pEntry->m_pBarrel->undeleteAll();
				iter++;
			}
		}
		
		bool CMultiIndexBarrelReader::isDeleted(docid_t docID)
		{
			CBarrelReaderEntry* pEntry;
			CBarrelInfo * pBarrelInfo;
			vector<CBarrelReaderEntry*>::iterator iter = m_readers.begin();
			while (iter != m_readers.end())
			{
				pEntry = (*iter);
				pBarrelInfo = pEntry->m_pBarrelInfo;
				if( (docID < pBarrelInfo->getMinDocID()) ||
					(docID >= (pBarrelInfo->getMinDocID() + pBarrelInfo->getDocCount())) )
					iter++;				
				else 
					return pEntry->m_pBarrel->isDeleted(docID - pBarrelInfo->getMinDocID());
			}
			return false;
		}
		
		bool CMultiIndexBarrelReader::hasDeletions()
		{
			CBarrelReaderEntry* pEntry;			
			vector<CBarrelReaderEntry*>::iterator iter = m_readers.begin();
			while (iter != m_readers.end())
			{
				pEntry = (*iter);
				if(pEntry->m_pBarrel->hasDeletions())
					return true;
				iter++;
			}
			return false;
		}

		count_t CMultiIndexBarrelReader::numDeletedDocs()
		{
			count_t totalDels = 0;
			CBarrelReaderEntry* pEntry;			
			vector<CBarrelReaderEntry*>::iterator iter = m_readers.begin();
			while (iter != m_readers.end())
			{
				pEntry = (*iter);
				totalDels += pEntry->m_pBarrel->numDeletedDocs();					
				iter++;
			}
			return totalDels;
		}
		CTermReader* CMultiIndexBarrelReader::termReader()
		{
			if(m_pTermReader == NULL)
			{
				try
				{
					m_pTermReader = new CMultiTermReader(this);
				}
				catch (std::bad_alloc& ba) 
				{
					string serror = ba.what();
					FIRTEX_THROW3(OUTOFMEM_ERROR,_T("CMultiIndexBarrelReader::termReader():") + serror);
				}
			}
			return m_pTermReader->clone();
		}

		CNormsBytes* CMultiIndexBarrelReader::norms(const tchar* field)
		{
			tstring s = field;
			map<tstring,CNormsBytes*>::iterator iter = m_normsByName.find(s);
			if(iter == m_normsByName.end())
			{
				if(loadNorms(field))
					iter = m_normsByName.find(field);
				else return NULL;
			}
			return iter->second;			
		}
		
		CNormsBytes*	CMultiIndexBarrelReader::norms(fieldid_t fid)
		{
			map<fieldid_t,CNormsBytes*>::iterator iter = m_normsByID.find(fid);
			if(iter == m_normsByID.end())
			{
				if(loadNorms(fid))
					iter = m_normsByID.find(fid);
				else return NULL;
			}
            return iter->second;			
		}
		
		void CMultiIndexBarrelReader::close()
		{
			if(m_pTermReader)
			{
				delete m_pTermReader;
				m_pTermReader = NULL;
			}			

			vector<CBarrelReaderEntry*>::iterator iter = m_readers.begin();
			while (iter != m_readers.end())
			{
				delete (*iter);
				iter++;
			}
			m_readers.clear();

			map<fieldid_t,CNormsBytes*>::iterator iter2 = m_normsByID.begin();
			while(iter2 != m_normsByID.end())
			{
				delete iter2->second;				
				iter2++;
			}
			m_normsByID.clear();
			m_normsByName.clear();

			if(m_pTermVectorReader)
			{
				delete m_pTermVectorReader;
				m_pTermVectorReader = NULL;
			}
		}	
		void CMultiIndexBarrelReader::addReader(CBarrelInfo* pBarrelInfo)
		{
			m_readers.push_back(new CBarrelReaderEntry(m_pIndex,pBarrelInfo));
		}
		bool CMultiIndexBarrelReader::loadNorms(const tchar* field)
		{			
			CBarrelReaderEntry* pEntry;
		
			fieldid_t fid = -1;
			CNormsBytes* pSubNormBytes;
			int32_t nSub = 0;
			CNormsBytes* pNormsBytes = new CNormsBytes(m_readers.size());
			vector<CBarrelReaderEntry*>::iterator iter = m_readers.begin();
			while (iter != m_readers.end())
			{	
				pEntry = (*iter);
				pSubNormBytes = pEntry->m_pBarrel->norms(field);
				if(pSubNormBytes)
				{
					if(fid == -1)
						fid = pEntry->m_pBarrel->getFieldsInfo().getFieldID(field);
					pNormsBytes->setNorms(nSub,pSubNormBytes);					
				}				
				iter++;
				nSub++;
			}						
			
			m_normsByID.insert(pair<fieldid_t,CNormsBytes*>(fid,pNormsBytes));
			m_normsByName.insert(pair<tstring,CNormsBytes*>(tstring(field),pNormsBytes));
			return true;
		}
		bool CMultiIndexBarrelReader::loadNorms(fieldid_t fid)
		{			
			CBarrelReaderEntry* pEntry;
			tstring field;
			CNormsBytes* pSubNormBytes;
			int32_t nSub = 0;
			CNormsBytes* pNormsBytes = new CNormsBytes(m_readers.size());
			vector<CBarrelReaderEntry*>::iterator iter = m_readers.begin();
			while (iter != m_readers.end())
			{	
				pEntry = (*iter);
				pSubNormBytes = pEntry->m_pBarrel->norms(field.c_str());
				if(pSubNormBytes)
				{
					if(field.empty())
						field = pEntry->m_pBarrel->getFieldsInfo().getFieldName(fid);
					pNormsBytes->setNorms(nSub,pSubNormBytes);					
				}				
				iter++;
				nSub++;
			}						

			m_normsByID.insert(pair<fieldid_t,CNormsBytes*>(fid,pNormsBytes));
			m_normsByName.insert(pair<tstring,CNormsBytes*>(tstring(field),pNormsBytes));			
			return true;
		}	
		uint64_t CMultiIndexBarrelReader::numTerms()
		{
			uint64_t totalTerms = 0;
			CBarrelReaderEntry* pEntry;			
			vector<CBarrelReaderEntry*>::iterator iter = m_readers.begin();
			while (iter != m_readers.end())
			{
				pEntry = (*iter);
				totalTerms += pEntry->m_pBarrel->numTerms();					
				iter++;
			}
			return totalTerms;
		}
		CFieldsInfo& CMultiIndexBarrelReader::getFieldsInfo()
		{
			if(m_readers.empty())
				FIRTEX_THROW2(INDEX_COLLAPSE_ERROR,"There is no index barrel.");
			return m_readers[m_readers.size()-1]->m_pBarrel->getFieldsInfo();
		}

		CForwardIndexReader* CMultiIndexBarrelReader::getForwardIndexReader()
		{
			if(!m_pTermVectorReader)
			{
				int cnt = 0;
				m_pTermVectorReader = new CMultiForwardIndexReader((int)m_readers.size());
				CBarrelReaderEntry* pEntry;
				CBarrelInfo * pBarrelInfo;
				vector<CBarrelReaderEntry*>::iterator iter = m_readers.begin();
				while (iter != m_readers.end())
				{
					pEntry = (*iter);
					pBarrelInfo = pEntry->m_pBarrelInfo;
					m_pTermVectorReader->addVectorReader(cnt,pBarrelInfo->getMinDocID(),pBarrelInfo->getDocCount(),pEntry->m_pBarrel->getForwardIndexReader());
					iter++;
					cnt++;
				}
			}
			return m_pTermVectorReader;
		}
		/*void CMultiIndexBarrelReader::setFactoryFinder(CFactoryFinder* pFactories)
		{
			CIndexBarrelReader::setFactoryFinder(pFactories);
			CBarrelReaderEntry* pEntry;
			vector<CBarrelReaderEntry*>::iterator iter = m_readers.begin();
			while (iter != m_readers.end())
			{
				pEntry = (*iter);
				pEntry->m_pBarrel->setFactoryFinder(pFactories);
				iter++;				
			}
		}*/
		void CMultiIndexBarrelReader::addBarrelReader(CBarrelInfo* pBarrelInfo,CIndexBarrelReader* pBarrelReader)
		{
			m_readers.push_back(new CBarrelReaderEntry(pBarrelInfo,pBarrelReader));
		}
	}
}
