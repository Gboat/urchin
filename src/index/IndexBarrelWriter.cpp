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
// Email	: ruijieguo@software.ict.ac.cn ruijieguo@gmail.com
// Created	: 2005/11/27
//

#include "index/IndexBarrelWriter.h"
#include "utility/StdHeader.h"
#include "store/IndexOutput.h"
#include "store/IndexInput.h"
#include "index/DictHashIndexer.h"
#include "store/Directory.h"
#include "index/FieldFactory.h"
#include "utility/Config.h"
#include "index/InMemoryIndexBarrelReader.h"

#include <fstream>

using namespace std;
using namespace firtex::store;

namespace firtex
{
	namespace index
	{		
		CIndexBarrelWriter::CIndexBarrelWriter(CDirectory* pDirectory,CMemCache* pMemCache,const tchar* name)
			:m_pDirectory(pDirectory)
			,m_barrelName(name)
			,m_fieldIndexers(NULL)
			,m_numIndexers(0)
			,m_pFieldFactories(NULL)
			,m_pDocFilter(NULL)
			,m_pMemCache(pMemCache)
		{				
			
			m_pFieldsInfo = new CFieldsInfo();
			m_pStoredFieldsWriter = new CStoredFieldsWriter(pDirectory,name,m_pFieldsInfo);
			m_pNormsWriter = new CNormsWriter(pDirectory,name);
			m_pTermVectorWriter = NULL;
		}
		CIndexBarrelWriter::CIndexBarrelWriter(CDirectory* pDirectory,CMemCache* pMemCache)
			:m_pDirectory(pDirectory)
			,m_fieldIndexers(NULL)
			,m_numIndexers(0)
			,m_pFieldFactories(NULL)
			,m_pDocFilter(NULL)
			,m_pMemCache(pMemCache)
		{					
			m_pFieldsInfo = new CFieldsInfo();
			m_pStoredFieldsWriter = new CStoredFieldsWriter();
		}

		CIndexBarrelWriter::~CIndexBarrelWriter(void)
		{
			m_pMemCache = NULL;			

			m_pFieldFactories = NULL;

			if(m_pStoredFieldsWriter)
			{
				delete m_pStoredFieldsWriter;
				m_pStoredFieldsWriter = NULL;
			}
			if(m_pFieldsInfo)
			{
				delete m_pFieldsInfo;
				m_pFieldsInfo = NULL;
			}

			if(m_pNormsWriter)
			{
				delete m_pNormsWriter;
				m_pNormsWriter = NULL;
			}
			if(m_pTermVectorWriter)
			{
				delete m_pTermVectorWriter;
				m_pTermVectorWriter = NULL;
			}

			if(m_pDocFilter)
			{
				delete m_pDocFilter;
				m_pDocFilter = NULL;
			}
			for (int32_t i = 0;i<m_numIndexers;i++)
			{
				if(m_fieldIndexers[i])
				{
					delete m_fieldIndexers[i];
					m_fieldIndexers[i] = NULL;
				}
			}
			delete[] m_fieldIndexers;			
		}		
		void CIndexBarrelWriter::open(const tchar* barrelName)
		{
			m_barrelName = barrelName;
			m_pStoredFieldsWriter->open(m_pDirectory,barrelName,m_pFieldsInfo);
			m_pNormsWriter->open(barrelName);
			m_pFieldsInfo->reset();
			if(m_pTermVectorWriter)
			{
				m_pTermVectorWriter->open(barrelName);	//前向索引
			}
		}
		void CIndexBarrelWriter::close()
		{
			if(cacheEmpty() == false)
			{
				writeCache();
				closeFiles();
				resetCache();
			}
		}
		void CIndexBarrelWriter::rename(const tchar* newName)
		{
			closeFiles();	

			m_pDirectory->batRenameFiles(m_barrelName,newName);
			m_barrelName = newName;
		}

		void CIndexBarrelWriter::addDocument(document::CDocument* pDoc)
		{		
			CFieldIndexer* pFieldIndexer = NULL;
						
			fieldid_t fid;
			bool bHasTermVector = false;			

			pDoc->startIterator();
			CField* pField = NULL;
			while(pDoc->hasNext())
			{
				pField = pDoc->next();

				m_pFieldsInfo->updateField(pField);//添加域信息
				if( pField->isIndexed())
				{			
					fid = pField->getID();					
					
					FIRTEX_ASSERT(m_fieldIndexers[fid],_T("CIndexBarrelWriter::addDocument():No indexer for the field."));												
					m_fieldIndexers[fid]->addField(pDoc->getDocID(),pField);//对应进行索引

					m_pNormsWriter->addField(pDoc->getDocID(),pField);
				}				
				if( (m_pTermVectorWriter == NULL) && pField->isStoreForwardIndex())
				{
					m_pTermVectorWriter = new CForwardIndexWriter(m_pDirectory);
					m_pTermVectorWriter->setFactoryFinder(m_pFieldFactories);
					m_pTermVectorWriter->open(m_barrelName.c_str());
				}
			}
			m_pStoredFieldsWriter->addDocument(pDoc);
			if(m_pTermVectorWriter)
			{
				m_pTermVectorWriter->addDocument(pDoc);	//前向索引
			}		
		}
		int32_t CIndexBarrelWriter::addDocuments(document::CDocument** ppDocs,int32_t numDocs)
		{
			if(numDocs <= 0)
				return 0;

			CFieldIndexer* pFieldIndexer = NULL;

			fieldid_t fid;
			bool bHasTermVector = false;
			document::CDocument* pDoc = NULL;			
			
			int32_t indexedDocs = 0;
			for (indexedDocs = 0;indexedDocs<numDocs;indexedDocs++)
			{
				if(this->cacheFull())
					return indexedDocs;

				pDoc = ppDocs[indexedDocs];

				pDoc->startIterator();
				CField* pField = NULL;
				while(pDoc->hasNext())
				{
					pField = pDoc->next();

					m_pFieldsInfo->updateField(pField);//添加域信息
					if( pField->isIndexed())
					{			
						fid = pField->getID();					
						FIRTEX_ASSERT(m_fieldIndexers[fid],_T("CIndexBarrelWriter::addDocument():No indexer for the field."));												
						m_fieldIndexers[fid]->addField(pDoc->getDocID(),pField);//对应进行索引
						
						m_pNormsWriter->addField(pDoc->getDocID(),pField);
					}				
					if( (m_pTermVectorWriter == NULL) && pField->isStoreForwardIndex())
					{
						m_pTermVectorWriter = new CForwardIndexWriter(m_pDirectory);
						m_pTermVectorWriter->open(m_barrelName.c_str());
					}
				}
				m_pStoredFieldsWriter->addDocument(pDoc);
				if(m_pTermVectorWriter)
				{
					m_pTermVectorWriter->addDocument(pDoc);	//前向索引
				}
			}
			return indexedDocs;
		}
					
		bool CIndexBarrelWriter::cacheFull()
		{
			return m_pMemCache->isGrowed();
		}
		
		void CIndexBarrelWriter::resetCache(bool bResetPosting /* = false */)
		{			
			if(bResetPosting)
			{
				CFieldIndexer* pFieldIndexer;
				for (fieldid_t i = 0;i<(fieldid_t)m_numIndexers;i++)
				{				
					pFieldIndexer = m_fieldIndexers[i];
					if(pFieldIndexer == NULL)
						continue;
					pFieldIndexer->reset();
				}					
			}

			m_pMemCache->flushMem();
		}
		void CIndexBarrelWriter::closeFiles()
		{
			m_pStoredFieldsWriter->close();
			m_pNormsWriter->close();
			if(m_pTermVectorWriter)
			{
				m_pTermVectorWriter->close();
			}
			if(m_pDocFilter && m_pDocFilter->any())
			{
				tstring str = m_barrelName + _T(".del");
				m_pDocFilter->write(m_pDirectory,str.c_str());
				delete m_pDocFilter;
				m_pDocFilter = NULL;
			}
		}
		
		void CIndexBarrelWriter::writeCache()
		{
			try
			{
				FIRTEX_CLOG(level::dbg) << _T("CIndexBarrelWriter:Begin write cache...") << FIRTEX_ENDL;

				tstring s = m_barrelName +_T(".voc");
				CIndexOutput* pVocOutput = m_pDirectory->createOutput(s.c_str());				
				s = m_barrelName + _T(".dfp");
				CIndexOutput* pDOutput = m_pDirectory->createOutput(s.c_str());
				s = m_barrelName + _T(".pop");
				CIndexOutput* pPOutput = m_pDirectory->createOutput(s.c_str());				

				COutputDescriptor desc(pVocOutput,pDOutput,pPOutput,true);
				
			
				fileoffset_t vocOff1,vocOff2,dfiOff1,dfiOff2,ptiOff1,ptiOff2;
				fileoffset_t vocOffset;
				CFieldIndexer* pFieldIndexer;				
				for (fieldid_t i = 0;i<(fieldid_t)m_numIndexers;i++)
				{				
					pFieldIndexer = m_fieldIndexers[i];
					if(pFieldIndexer == NULL)
						continue;
					vocOff1 = pVocOutput->getFilePointer();
					dfiOff1 = pDOutput->getFilePointer();
					ptiOff1 = pPOutput->getFilePointer();					
					
					m_pFieldsInfo->setDistinctNumTerms((fieldid_t)i,pFieldIndexer->distinctNumTerms());///set distinct term numbers

					vocOffset = pFieldIndexer->write(&desc);///write field index data

					m_pFieldsInfo->setFieldOffset((fieldid_t)i,vocOffset);///set offset of vocabulary descriptor

					vocOff2 = pVocOutput->getFilePointer();
					dfiOff2 = pDOutput->getFilePointer();
					ptiOff2 = pPOutput->getFilePointer();

					m_pFieldsInfo->getField(i)->setLength(vocOff2-vocOff1,dfiOff2-dfiOff1,ptiOff2-ptiOff1);
				}
				FIRTEX_CLOG(level::dbg) << _T("CIndexBarrelWriter:End write cache...") << FIRTEX_ENDL;
				//写域信息
				s = m_barrelName + _T(".fdi");
				CIndexOutput* fdiOutput = m_pDirectory->createOutput(s.c_str());				
				m_pFieldsInfo->write(fdiOutput);
				delete fdiOutput;								
			}
			catch (const CFileIOException& e) 
			{
				FIRTEX_RETHROW(e);
			}			
			catch (std::bad_alloc& be)
			{
				tstring serror = be.what();
				FIRTEX_THROW2(OUTOFMEM_ERROR,"CIndexBarrelWriter::writeCache():alloc memory failed.-" + serror);
			}
		}	
	
		CFieldIndexer* CIndexBarrelWriter::getFieldIndexer(const tchar* field)
		{
			fieldid_t fid =	m_pFieldsInfo->getFieldID(field);
			FIRTEX_ASSERT((fid<m_numIndexers),_T("field id is out of range."));
			return m_fieldIndexers[fid];
		}
		CFieldIndexer* CIndexBarrelWriter::getFieldIndexer(fieldid_t fid)
		{
			FIRTEX_ASSERT((fid<m_numIndexers),_T("field id is out of range."));
			return m_fieldIndexers[fid];
		}
		void CIndexBarrelWriter::setSchema(CDocumentSchema* pSchema)
		{
			m_numIndexers = pSchema->getCount();
			FIRTEX_ASSERT((m_numIndexers>0),_T("no schema."));
			m_fieldIndexers = new CFieldIndexer*[m_numIndexers];
			memset(m_fieldIndexers,0,m_numIndexers*sizeof(CFieldIndexer*));
			return m_pFieldsInfo->setSchema(pSchema);
		}
		void CIndexBarrelWriter::setFactoryFinder(CFactoryFinder* pFactories)
		{
			m_pFieldFactories = pFactories;

			FIRTEX_ASSERT(m_fieldIndexers,_T("setFactoryFinder():m_fieldIndexers is null."));			

			fieldid_t fid = 0;
			CFieldFactory* pFactory = NULL;
			for (fid = 0;fid < m_numIndexers;fid++)
			{
				pFactory = m_pFieldFactories->getFactory(fid);
				if(pFactory)
				{
					m_fieldIndexers[fid] = pFactory->createIndexer(m_pMemCache);
					if(m_fieldIndexers[fid] == NULL)
						FIRTEX_THROW3(UNSUPPORTED_ERROR,_T("CIndexBarrelWriter::setFactoryFinder():can't create specified field indexer."));
					m_fieldIndexers[fid]->setIndexLevel(m_pFieldsInfo->getField(fid)->getIndexLevel());
					m_fieldIndexers[fid]->setField(m_pFieldsInfo->getField(fid)->getName());
				}
			}
			if(m_pTermVectorWriter)
				m_pTermVectorWriter->setFactoryFinder(m_pFieldFactories);
		}
		CIndexBarrelReader* CIndexBarrelWriter::inMemoryReader()
		{			
			CInMemoryIndexBarrelReader * pReader = new CInMemoryIndexBarrelReader(this);
			return pReader;			
		}
	}
}

