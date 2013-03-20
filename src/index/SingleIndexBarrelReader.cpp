//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: ¹ùÈğ½Ü(GuoRuijie)
// Email	: ruijieguo@software.ict.ac.cn
// Created	: 2005/11/27
//
#include "index/SingleIndexBarrelReader.h"
#include "index/FieldFactory.h"
#include "index/MultiFieldTermReader.h"
#include "document/Field.h"
#include "index/FieldsInfo.h"
#include "index/Index.h"

using namespace firtex::document;

namespace firtex
{
	namespace index
	{
		CSingleIndexBarrelReader::CSingleIndexBarrelReader(CIndex* pIndex, CBarrelInfo* pBarrelInfo)
			: CIndexBarrelReader(pIndex)
			, m_pBarrelInfo(pBarrelInfo)
			, m_pTermReader(NULL)
			, m_pTermVectorReader(NULL)			
		{			
			m_pFieldsInfo = new CFieldsInfo();
			m_pStoredFieldsReader = new CStoredFieldsReader(pIndex->getDirectory(),pBarrelInfo->getName(),m_pFieldsInfo);
			m_pNormsReader = new CNormsReader(pIndex->getDirectory());
			m_pDeletedDocs = NULL;
			m_bDeletedDocsDirty = false;
			open(pBarrelInfo->getName().c_str());
		}

		CSingleIndexBarrelReader::~CSingleIndexBarrelReader(void)
		{
			close();
			if(m_pStoredFieldsReader)
			{
				delete m_pStoredFieldsReader;
				m_pStoredFieldsReader = NULL;
			}
			if(m_pFieldsInfo)
			{
				delete m_pFieldsInfo;
				m_pFieldsInfo = NULL;
			}
			if(m_pNormsReader)
			{
				delete m_pNormsReader;
				m_pNormsReader = NULL;
			}
			if(m_pTermReader)
			{
				delete m_pTermReader;
				m_pTermReader = NULL;
			}			
			if(m_pTermVectorReader)
			{
				delete m_pTermVectorReader;
				m_pTermVectorReader = NULL;
			}

			if(m_pDeletedDocs)
			{
				delete m_pDeletedDocs;
				m_pDeletedDocs = NULL;
			}			
		}
		document::CDocument* CSingleIndexBarrelReader::document(docid_t docID)
		{
			if(m_pDeletedDocs && m_pDeletedDocs->test((size_t)docID))
				FIRTEX_THROW2(ILLEGALARGUMENT_ERROR,_T("attempt to access a deleted document"));
			document::CDocument* pDoc = m_pStoredFieldsReader->document(docID);
			if(pDoc)
				pDoc->setDocID(docID);
			return pDoc;
		}
		document::CField* CSingleIndexBarrelReader::field(docid_t docID,fieldid_t fid)
		{
			if(m_pDeletedDocs && m_pDeletedDocs->test((size_t)docID))
				FIRTEX_THROW2(ILLEGALARGUMENT_ERROR,_T("attempt to access a deleted document"));
			return m_pStoredFieldsReader->field(docID,fid);			
		}

		bool CSingleIndexBarrelReader::deleteDocument(docid_t docID)
		{
			if(m_pDeletedDocs == NULL)
			{
				m_pDeletedDocs = new CBitVector((size_t)(docID + 1));
				if(m_pTermReader)
					m_pTermReader->setFilter(m_pDeletedDocs);
			}
			else if(m_pDeletedDocs->test(docID))
				return false;
			m_pDeletedDocs->set(docID);			
			m_bDeletedDocsDirty = true;
			return true;
		}
		void CSingleIndexBarrelReader::undeleteAll()
		{
			if(m_pDeletedDocs)
			{
				delete m_pDeletedDocs;
				m_pDeletedDocs = NULL;

				if(m_pIndex->getDirectory()->fileExists(m_name + _T(".del")))
					m_pIndex->getDirectory()->deleteFile(m_name + _T(".del"));
				if(m_pTermReader)
					m_pTermReader->setFilter(NULL);
				m_bDeletedDocsDirty = true;
			}
		}
		bool CSingleIndexBarrelReader::isDeleted(docid_t docID)
		{
			if(m_pDeletedDocs)
			{
				return m_pDeletedDocs->test((size_t)docID);
			}
			else return false;
		}		
		bool CSingleIndexBarrelReader::hasDeletions()
		{
			if(m_pDeletedDocs == NULL)
				return false;

			return m_pDeletedDocs->any();
		}
		
		count_t CSingleIndexBarrelReader::numDeletedDocs()
		{
			if(m_pDeletedDocs == NULL)
				return 0;
			return (count_t)m_pDeletedDocs->count();
		}

		void CSingleIndexBarrelReader::open(const tchar* name)
		{
			m_name = name;

			CDirectory* pDirectory = m_pIndex->getDirectory();
			tstring s = m_name + _T(".fdi");
			CIndexInput* pIndexInput = pDirectory->openInput(s.c_str());
			m_pFieldsInfo->read(pIndexInput);
			pIndexInput->close();
			delete pIndexInput;

			m_pStoredFieldsReader->open(pDirectory,name,m_pFieldsInfo);
			m_pNormsReader->open(name,m_pFieldsInfo);

			if(pDirectory->fileExists(m_name + _T(".del")))
			{
				CIndexInput* pDelInput = pDirectory->openInput(m_name + _T(".del"));
				m_pDeletedDocs = new CBitVector(pDelInput);
				delete pDelInput;
			}
			else m_pDeletedDocs = new CBitVector();

			if(m_pTermReader)
			{
				delete m_pTermReader;
				m_pTermReader = NULL;
			}
			if(m_pFieldsInfo->numIndexFields() > 1)
			{
				m_pTermReader = new CMultiFieldTermReader(pDirectory,name,m_pFieldsInfo,m_pDeletedDocs,getFactoryFinder());
				m_pTermReader->setFilter(m_pDeletedDocs);				
			}
			else if(m_pFieldsInfo->numIndexFields() == 1)
			{
				m_pFieldsInfo->startIterator();
				CFieldInfo* pFieldInfo;
				while (m_pFieldsInfo->hasNext())
				{
					pFieldInfo = m_pFieldsInfo->next();
					if(pFieldInfo->isIndexed())
					{
						m_pTermReader = getFactoryFinder()->getFactory(pFieldInfo->getID())->createTermReader();
						m_pTermReader->setFilter(m_pDeletedDocs);
						m_pTermReader->open(pDirectory,name,pFieldInfo,m_pDeletedDocs);
						break;
					}
				}					
			}
			else 
			{
				FIRTEX_THROW2(INDEX_COLLAPSE_ERROR,_T("the field number is 0."));
			}			
		}
		CTermReader* CSingleIndexBarrelReader::termReader(const tchar* field)
		{
			if(m_pTermReader == NULL)
				return NULL;
			if(m_pFieldsInfo->numIndexFields() > 1)
				return ((CMultiFieldTermReader*)m_pTermReader)->termReader(field);
			else 
			{
				m_pFieldsInfo->startIterator();
				CFieldInfo* pFieldInfo;
				while (m_pFieldsInfo->hasNext())
				{
					pFieldInfo = m_pFieldsInfo->next();
					if(pFieldInfo->isIndexed())
					{							
						if(!_tcsicmp(field,pFieldInfo->getName()))
							return m_pTermReader->clone();
					}
				}						
			}
			return NULL;
		}

		CTermReader* CSingleIndexBarrelReader::termReader()
		{
			return m_pTermReader->clone();
		}

		CForwardIndexReader* CSingleIndexBarrelReader::getForwardIndexReader()
		{
			if(!m_pTermVectorReader)
			{
				m_pTermVectorReader = new CForwardIndexReader(m_pIndex->getDirectory(),m_pFieldsInfo);
				m_pTermVectorReader->setFactoryFinder(getFactoryFinder());
				m_pTermVectorReader->open(m_name.c_str());
			}
			return m_pTermVectorReader;
		}
		CNormsBytes*	CSingleIndexBarrelReader::norms(const tchar* field)
		{
			return m_pNormsReader->getNorms(field);
		}
		
		CNormsBytes* CSingleIndexBarrelReader::norms(fieldid_t fid)
		{
			return m_pNormsReader->getNorms(fid);
		}
		void CSingleIndexBarrelReader::close()
		{				
			m_pNormsReader->close();
			m_pStoredFieldsReader->close();
			m_pTermReader->close();			
			if(m_bDeletedDocsDirty)
			{
				if(m_pDeletedDocs)
				{
					tstring str = m_name + _T(".del");
					m_pDeletedDocs->write(m_pIndex->getDirectory(),str.c_str());
				}
				m_bDeletedDocsDirty = false;
			}
		}
		uint64_t CSingleIndexBarrelReader::numTerms()
		{
			uint64_t totalTerms = 0;
			CFieldInfo* pFieldInfo;
			m_pFieldsInfo->startIterator();
			while (m_pFieldsInfo->hasNext())
			{
				pFieldInfo = m_pFieldsInfo->next();
				if(FIELDBITS(pFieldInfo->getFlag())&BIT_INDEX_MASK)
					totalTerms += pFieldInfo->numTerms();
			}
			return totalTerms;
		}
	}
}
