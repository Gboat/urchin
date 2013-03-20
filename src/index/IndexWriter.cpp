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
// Created	: 2005/11/20
//

#include "index/IndexWriter.h"
#include "utility/Exception.h"
#include "index/IndexMerger.h"
#include "collection/CollectionFactory.h"
#include "parser/ParserFactory.h"
#include "analyzer/AnalyzerFactory.h"
#include "collection/Collection.h"
#include "index/Index.h"
#include "index/FieldFactory.h"
#include "index/InMemoryIndexBarrelReader.h"
#include "index/DBTIndexMerger.h"
#include "index/OptimizeMerger.h"
#include "index/GPIndexMerger.h"
#include "index/LogarithmicMerger.h"
#include "index/ImmediateMerger.h"
#include "utility/ParamParser.h"
#include <memory>


using namespace firtex::utility;
namespace firtex
{
	namespace index
	{		
		//////////////////////////////////////////////////////////////////////////
		//CIndexWriter
		
		CIndexWriter::CIndexWriter(CParser* pParser,CAnalyzer* pAnalyzer,CIndex* pIndex)
			: m_pAnalyzer(pAnalyzer)
			, m_pIndex(pIndex)			
			, m_nNumCacheUsed(0)		
			, m_ppAnalyzers(NULL)
			, m_nNumAnalyzers(0)		
			, m_pIndexBarrelWriter(NULL)
			, m_pCurDocCount(NULL)
			, m_pCurMinDocID(NULL)
			, m_pCurBarrelInfo(NULL)
			, m_pIndexMerger(NULL)
			, m_pMemCache(NULL)
			, m_pConfigurator(new firtex::utility::Configurator)
		{
			m_nNumCachedDocs = m_pConfigurator->index.cachedocs;

			m_pBarrelsInfo = m_pIndex->m_pBarrelsInfo;
			m_ppCachedDocs = NULL;		

			m_nNumParsers = firtex::document::CDocumentType::getInstance()->numTypes();
			m_ppParsers = new CParser*[m_nNumParsers];
			memset(m_ppParsers,0,m_nNumParsers*sizeof(CParser*));
			m_ppParsers[doc_type::UNKNOWN] = pParser;///默认文档格式解析器

		}
		
		CIndexWriter::CIndexWriter(CIndex* pIndex)
			: m_pAnalyzer(NULL)
			, m_pIndex(pIndex)					
			, m_nNumCacheUsed(0)			
			, m_ppAnalyzers(NULL)
			, m_nNumAnalyzers(0)
			, m_pIndexBarrelWriter(NULL)
			, m_pCurDocCount(NULL)
			, m_pCurMinDocID(NULL)
			, m_pCurBarrelInfo(NULL)
			, m_pIndexMerger(NULL)
			, m_pMemCache(NULL)
			, m_pConfigurator(new firtex::utility::Configurator)
		{
			m_nNumCachedDocs = m_pConfigurator->index.cachedocs;

			m_pBarrelsInfo = m_pIndex->m_pBarrelsInfo;

			m_ppCachedDocs = NULL;

			m_nNumParsers = firtex::document::CDocumentType::getInstance()->numTypes();
			m_ppParsers = new CParser*[m_nNumParsers];
			memset(m_ppParsers,0,m_nNumParsers*sizeof(CParser*));			
		}

		CIndexWriter::~CIndexWriter(void)
		{
			close();
			for(int32_t i = (int32_t)doc_type::UNKNOWN;i<m_nNumParsers;i++)
			{
				if(m_ppParsers[i])
				{
					delete m_ppParsers[i];
					m_ppParsers[i] = NULL;
				}	
			}
			delete[] m_ppParsers;

			m_pCurBarrelInfo = NULL;
			m_pCurDocCount = NULL;
			m_pCurMinDocID = NULL;
			
			if(m_pAnalyzer)
			{
				delete m_pAnalyzer;
				m_pAnalyzer = NULL;
			}
			if(m_ppAnalyzers)
			{
				delete[] m_ppAnalyzers;
				m_ppAnalyzers = NULL;
			}				
			if(m_pIndexBarrelWriter)
			{
				delete m_pIndexBarrelWriter;
				m_pIndexBarrelWriter = NULL;
			}

			destroyCache();

			map<tstring,CAnalyzer*>::iterator iter = m_analyzers.begin();
			while(iter != m_analyzers.end())
			{
				delete iter->second;
				iter++;
			}
			m_analyzers.clear();
            if(m_pConfigurator)
			{
				delete m_pConfigurator;
				m_pConfigurator = NULL;
			}
		}		
		
		void CIndexWriter::addDocument(const tchar* filename)
		{			
			m_ppCachedDocs[m_nNumCacheUsed]->setStatus(document::doc_status::UNPARSED);

			m_ppCachedDocs[m_nNumCacheUsed++]->setDocPath(-1,filename);

			if(m_nNumCacheUsed > 0 && m_nNumCacheUsed >= m_nNumCachedDocs)
				flushDocuments();
		}		
		
		void CIndexWriter::writeCachedIndex()
		{			
			m_pIndex->setDirty();
			m_pCurBarrelInfo->setWriter(NULL);			
			m_pBarrelsInfo->write(m_pIndex->getDirectory());
			m_pIndexBarrelWriter->close();			

			m_pBarrelsInfo->addBarrel(m_pBarrelsInfo->newBarrel().c_str(),*m_pCurMinDocID,0);
			m_pCurBarrelInfo = m_pBarrelsInfo->getLastBarrel();
			m_pCurBarrelInfo->setWriter(m_pIndexBarrelWriter);

			m_pCurMinDocID = &(m_pCurBarrelInfo->m_nMinDocID);
			*m_pCurMinDocID += *m_pCurDocCount;
			m_pCurDocCount = &(m_pCurBarrelInfo->m_nNumDocs);			
			*m_pCurDocCount = 0;
		}
		void CIndexWriter::mergeAndWriteCachedIndex()
		{
			m_pIndex->setDirty();
			//bool bGC = isGC();
			//m_pIndexMerger->setGC(bGC);
			m_pIndexMerger->merge(m_pBarrelsInfo);
			m_pBarrelsInfo->write(m_pIndex->getDirectory());
			if(m_pIndexBarrelWriter->cacheEmpty() == false)///memory index has not been written to database yet.
			{							
				m_pIndexBarrelWriter->close();							
			}			

			CBarrelInfo* pLastBarrel = m_pBarrelsInfo->getLastBarrel();
			docid_t minDocID = pLastBarrel->getMinDocID() + pLastBarrel->getDocCount();
			m_pBarrelsInfo->addBarrel(m_pBarrelsInfo->newBarrel().c_str(),minDocID,0);
			m_pCurBarrelInfo = m_pBarrelsInfo->getLastBarrel();
			m_pCurBarrelInfo->setWriter(m_pIndexBarrelWriter);
			
			m_pCurMinDocID = &(m_pCurBarrelInfo->m_nMinDocID);
			m_pCurDocCount = &(m_pCurBarrelInfo->m_nNumDocs);			
			*m_pCurDocCount = 0;
		}		

		void CIndexWriter::mergeAndWriteCachedIndex2()
		{
			m_pIndex->setDirty();
			//bool bGC = isGC();
			if(m_pIndexMerger)
			{
				//m_pIndexMerger->setGC(bGC);
				m_pIndexMerger->addToMerge(m_pBarrelsInfo,m_pBarrelsInfo->getLastBarrel());
			}
			//m_pBarrelsInfo->write(m_pIndex->getDirectory());
			if(m_pIndexBarrelWriter->cacheEmpty() == false)///memory index has not been written to database yet.
			{							
				m_pIndexBarrelWriter->close();
				if(m_pIndexMerger)					
					m_pIndexMerger->transferToDisk(m_pIndexBarrelWriter->m_barrelName.c_str());
			}			

			CBarrelInfo* pLastBarrel = m_pBarrelsInfo->getLastBarrel();
			pLastBarrel->setWriter(NULL);
			docid_t minDocID = pLastBarrel->getMinDocID() + pLastBarrel->getDocCount();
			m_pBarrelsInfo->addBarrel(m_pBarrelsInfo->newBarrel().c_str(),minDocID,0);
			m_pCurBarrelInfo = m_pBarrelsInfo->getLastBarrel();
			m_pCurBarrelInfo->setWriter(m_pIndexBarrelWriter);

			m_pCurMinDocID = &(m_pCurBarrelInfo->m_nMinDocID);
			m_pCurDocCount = &(m_pCurBarrelInfo->m_nNumDocs);			
			*m_pCurDocCount = 0;
		}		

		/** 索引文档 */
		inline void	CIndexWriter::indexDoc(document::CDocument* pAnalyzedDoc)
		{			
			if(!m_pIndexBarrelWriter)
			{				
				createBarrelWriter();
			}		
			if(m_pIndexBarrelWriter->cacheFull())
			{					
				///merge index
				if(m_pIndex->m_accessMode & CIndex::ACCESS_SEARCH)
				{			
					mergeAndWriteCachedIndex2();
				}
				else ///just write to disk
				{
					writeCachedIndex();
				}					
				m_pIndexBarrelWriter->open(m_pCurBarrelInfo->getName().c_str());
			}				
			//设置文档内部id编号
			pAnalyzedDoc->setDocID(*m_pCurDocCount);
			m_pIndexBarrelWriter->addDocument(pAnalyzedDoc);
			(*m_pCurDocCount)++;
			pAnalyzedDoc->clear();
		}				

		bool CIndexWriter::addExistIndex(CDirectory* pDirectory)
		{
			/*if(!m_pIndex)
			{				
				return false;
			}
			CIndexMerger merger;
			merger.merge(m_pIndex->getDirectory(),pDirectory);*/
			return true;
		}
		
		CDocumentSchema* CIndexWriter::getDocSchema()
		{
			return m_pIndex->getSchema();
		}
		void CIndexWriter::attachAnalyzer(CAnalyzer* pAnalyzer)
		{
			if(m_pAnalyzer)
				delete m_pAnalyzer;
			m_pAnalyzer = pAnalyzer;
		}
		void CIndexWriter::attachAnalyzer(const tchar* field,CAnalyzer* pAnalyzer)
		{
			CAnalyzer* pOldAna;
			map<tstring,CAnalyzer*>::iterator iter = m_analyzers.find(tstring(field));
			if(iter != m_analyzers.end())
			{
				pOldAna = iter->second;				
				delete pOldAna;
				m_analyzers.erase(iter);
			}
			m_analyzers.insert(make_pair(tstring(field),pAnalyzer));			
		}			
		CAnalyzer* CIndexWriter::attachAnalyzer(const tchar* field,const tchar* identifier,const tchar* szParam)
		{
			CAnalyzer* pNewAna = CAnalyzerFactory::createAnalyzer(identifier,szParam);
			if(!pNewAna)
				return NULL;
			CAnalyzer* pOldAna;
			map<tstring,CAnalyzer*>::iterator iter = m_analyzers.find(tstring(field));
			if(iter != m_analyzers.end())
			{
				pOldAna = iter->second;				
				delete pOldAna;
				m_analyzers.erase(iter);
			}
			m_analyzers.insert(make_pair(tstring(field),pNewAna));
			return pNewAna;
		}

		CAnalyzer* CIndexWriter::detachAnalyzer()
		{
			CAnalyzer* pAnalyzer = m_pAnalyzer;
			m_pAnalyzer = NULL;
			return pAnalyzer;
		}

		CAnalyzer* CIndexWriter::detachAnalyzer(const tchar* field)
		{
			CAnalyzer* pOldAna = NULL;
			map<tstring,CAnalyzer*>::iterator iter = m_analyzers.find(tstring(field));
			if(iter == m_analyzers.end())
			{
				return NULL;
			}
			else 
			{
				pOldAna = iter->second;
				m_analyzers.erase(iter);
			}
			return pOldAna;
		}
		
		void CIndexWriter::attachParser(CParser* pParser)
		{
			if(m_ppParsers[doc_type::UNKNOWN])
				delete m_ppParsers[doc_type::UNKNOWN];
			m_ppParsers[doc_type::UNKNOWN] = pParser;
		}
		void CIndexWriter::attachParser(doctype_t docType,CParser* pParser)
		{
			FIRTEX_ASSERT((docType >= doc_type::UNKNOWN && docType < m_nNumParsers),_T("attachParser():Invalid document type"));
			if(m_ppParsers[docType])
				delete m_ppParsers[docType];
			m_ppParsers[docType] = pParser;
		}
		
		CParser* CIndexWriter::detachParser()
		{
			CParser* pParser = m_ppParsers[doc_type::UNKNOWN];
			m_ppParsers[doc_type::UNKNOWN] = NULL;
			return pParser;
		}
		CParser* CIndexWriter::detachParser(doctype_t docType)
		{
			FIRTEX_ASSERT((docType>=doc_type::UNKNOWN && docType < m_nNumParsers),_T("detachParser():Invalid document type"));
			CParser* pParser = m_ppParsers[docType];
			m_ppParsers[docType] = NULL;
			return pParser;
		}

		void CIndexWriter::mergeIndex()
		{	
			if(!m_pIndexMerger)
				createMerger();
			if(m_pIndexMerger)
			{
				///there is a in-memory index
				if(m_pIndexBarrelWriter && m_pCurDocCount && ((*m_pCurDocCount) > 0) )
				{
					mergeAndWriteCachedIndex();					
				}
				else 
				{
					if(m_pCurBarrelInfo)
					{
						m_pBarrelsInfo->deleteLastBarrel();
						m_pCurBarrelInfo = NULL;
						m_pCurDocCount = NULL;
						m_pCurMinDocID = NULL;
					}
					m_pIndex->setDirty();					
					m_pIndexMerger->merge(m_pBarrelsInfo);
				}			
			}		
		}
		void CIndexWriter::mergeIndex(CIndexMerger* pIndexMerger)
		{
			pIndexMerger->setDirectory(m_pIndex->getDirectory());
			pIndexMerger->setFactoryFinder(m_pIndex->getFactoryFinder());

			///there is a in-memory index
			if(m_pIndexBarrelWriter && m_pCurDocCount && ((*m_pCurDocCount) > 0) )
			{
				CIndexMerger* pTmp = m_pIndexMerger;
				m_pIndexMerger = pIndexMerger;
				mergeAndWriteCachedIndex();	
				m_pIndexMerger = pTmp;
			}
			else 
			{
				if(m_pCurBarrelInfo)
				{
					m_pBarrelsInfo->deleteLastBarrel();
					m_pCurBarrelInfo = NULL;
					m_pCurDocCount = NULL;
					m_pCurMinDocID = NULL;
				}
				m_pIndex->setDirty();				
				pIndexMerger->merge(m_pBarrelsInfo);
			}						
		}		
		void CIndexWriter::optimizeIndex(bool bGC /* = false */)
		{				
            CIndexMerger* pIndexMerger = new COptimizeMerger(m_pIndex->m_pDirectory);
			pIndexMerger->setFactoryFinder(m_pIndex->getFactoryFinder());

			///there is a in-memory index
			if(m_pIndexBarrelWriter && m_pCurDocCount && ((*m_pCurDocCount) > 0) )
			{
				CIndexMerger* pTmp = m_pIndexMerger;
				m_pIndexMerger = pIndexMerger;
				mergeAndWriteCachedIndex();	
				m_pIndexMerger = pTmp;
			}
			else 
			{
				if(m_pCurBarrelInfo)
				{
					m_pBarrelsInfo->deleteLastBarrel();
					m_pCurBarrelInfo = NULL;
					m_pCurDocCount = NULL;
					m_pCurMinDocID = NULL;
				}
				m_pIndex->setDirty();				
				pIndexMerger->merge(m_pBarrelsInfo);
			}			
		}

		void CIndexWriter::flushDocuments()
		{			
			if(m_nNumCacheUsed <=0 )
				return;					
			FIRTEX_CLOG(level::dbg) << _T("flushing ...") <<FIRTEX_ENDL;		
			
			doctype_t curType;
			for(int32_t i=0;i<m_nNumCacheUsed;i++)
			{	
				if(m_ppCachedDocs[i]->getStatus() == doc_status::UNPARSED)
				{
					curType = m_ppCachedDocs[i]->getDocType();
					if(m_ppParsers[curType] == NULL)
					{
						m_ppParsers[curType] = CParserFactory::createParser(curType,NULL);
						if(m_ppParsers[curType])
							m_ppParsers[curType]->setSchema(m_pIndex->getSchema());
						else 
							FIRTEX_CLOG(level::warn) << _T("No parser for document type:") << firtex::document::CDocumentType::getInstance()->typeToName(curType) << FIRTEX_ENDL;
					}					
					if(m_ppParsers[curType]->parseDoc(m_ppCachedDocs[i]) == false)
					{
						m_ppCachedDocs[i]->setStatus(doc_status::UNPARSED);
						continue;///skip 
					}
				}				
				analyzeDoc(m_ppCachedDocs[i]);
				indexDoc(m_ppCachedDocs[i]);
				m_ppCachedDocs[i]->setStatus(doc_status::UNPARSED);
			}				
			m_nNumCacheUsed = 0;						
		}
		void CIndexWriter::flushToDisk()
		{
			flushDocuments();
			//TODO:
		}
		void CIndexWriter::analyzeCachedDocs()
		{
			for(int32_t i = 0;i<m_nNumCacheUsed;i++)
			{
				analyzeDoc(m_ppCachedDocs[i]);
			}
		}

		
		void CIndexWriter::defineSchema()
		{
			///set parser's schema
			for(int32_t i = 0;i<m_nNumParsers;i++)
			{
				if(m_ppParsers[i])
				{
					m_ppParsers[i]->setSchema(m_pIndex->getSchema());
				}
			}			

			if(m_ppCachedDocs)
				destroyCache();
			m_ppCachedDocs = new document::CDocument*[m_nNumCachedDocs];
			for(int32_t i = 0;i<m_nNumCachedDocs;i++)
			{
				m_ppCachedDocs[i] = new document::CDocument(m_pIndex->getSchema());
			}
		}

		void CIndexWriter::destroyCache()
		{
			if(m_ppCachedDocs)
			{
				for(int32_t i = 0;i<m_nNumCachedDocs;i++)
				{
					if(m_ppCachedDocs[i] != NULL)
						delete m_ppCachedDocs[i];
				}
			}
			delete[] m_ppCachedDocs;
			m_ppCachedDocs = NULL;
			m_nNumCacheUsed = 0;
		}

		CIndexBarrelReader* CIndexWriter::inMemoryReader()
		{
			if(m_pIndexBarrelWriter == NULL)
				createBarrelWriter();
			if(m_pIndexBarrelWriter == NULL)
				return NULL;
			return m_pIndexBarrelWriter->inMemoryReader();			
		}

		void CIndexWriter::prepareAnalyzer()
		{
			if(m_ppAnalyzers)
				delete[] m_ppAnalyzers;
			CDocumentSchema* pDocSchema = m_pIndex->getSchema();
			m_nNumAnalyzers = pDocSchema->getCount();
			m_ppAnalyzers = new CAnalyzer*[m_nNumAnalyzers];
			memset(m_ppAnalyzers,0,m_nNumAnalyzers*sizeof(CAnalyzer*));

			pDocSchema->startIterator();
			CSchemaItem* pItem = NULL;
			const tchar* szAnalyzer;
			while (pDocSchema->hasNext())
			{
				pItem = pDocSchema->next();
				if(pItem->getIndex() == Index::ANALYZE)
				{
					FIRTEX_ASSERT((pItem->getId()<m_nNumAnalyzers),_T("Schema id is out of order."));
					szAnalyzer = pItem->getComponent(_T("analyzer"));
					if(!szAnalyzer || (*szAnalyzer == 0))///not specify analyzer
					{
						map<tstring,CAnalyzer*>::iterator iter = m_analyzers.find(pItem->getName());
						if(iter == m_analyzers.end())
						{
							m_ppAnalyzers[pItem->getId()] = m_pAnalyzer;
							pItem->setComponent(_T("analyzer"),m_pAnalyzer->getIdentifier());
						}
						else 
						{
							m_ppAnalyzers[pItem->getId()] = iter->second;
							pItem->setComponent(_T("analyzer"),iter->second->getIdentifier());
						}
					}
					else
					{
						CAnalyzer* pAnalyzer = attachAnalyzer(pItem->getName(),szAnalyzer,pItem->getComponent(_T("analyzer_parameter")));
						if(!pAnalyzer)
						{
							FIRTEX_CLOG(level::warn) << _T("create analyzer:") << szAnalyzer << _T("failed.replace with default analyzer.") << FIRTEX_ENDL;
							m_ppAnalyzers[pItem->getId()] = m_pAnalyzer;
							pItem->setComponent(_T("analyzer"),m_pAnalyzer->getIdentifier());
						}
						else 
						{
							m_ppAnalyzers[pItem->getId()] = pAnalyzer;
						}
					}					
				}
			}			
		}

		CFactoryFinder* CIndexWriter::createFactoryFinder()
		{			
			CFactoryFinder* pFactoryFinder = new CFactoryFinder();
			CDocumentSchema* pDocSchema = m_pIndex->getSchema();
			pDocSchema->startIterator();
			CSchemaItem* pItem = NULL;
			while (pDocSchema->hasNext())
			{
				pItem = pDocSchema->next();
				if(pItem->getIndex() != Index::NO || pItem->getForwardIndex() != ForwardIndex::NO)
				{
					const tchar* _identifier = pItem->getComponent(_T("indexer"));
					if(!_identifier)
					{
						_identifier = m_ppAnalyzers[pItem->getId()]?m_ppAnalyzers[pItem->getId()]->getRecommendIndexer():m_pAnalyzer->getRecommendIndexer();
						pItem->setComponent(_T("indexer"),_identifier);
					}
					CFieldFactory* pFactory = pFactoryFinder->mapFactory(pItem->getId(),_identifier);					
					if(!pFactory)
					{
						tstring str = _T("create field factory: ");
						str += _identifier;
						str += _T(" failed.");
						FIRTEX_THROW3(ANALYZE_ERROR,str);
					}
				}
			}		
			if(pFactoryFinder->numFactories() <= 0)
			{
				delete pFactoryFinder;
				FIRTEX_THROW3(ANALYZE_ERROR,_T("no field factory created."));				
			}
			return pFactoryFinder;
		}
		void CIndexWriter::close()
		{			
			flushDocuments();
			///perform last merge
			if(m_pIndexMerger)
			{
				///there is a in-memory index
				if(m_pIndexBarrelWriter && m_pCurDocCount && ((*m_pCurDocCount) > 0) )
				{
					if(m_pIndex->m_accessMode & CIndex::ACCESS_SEARCH)
						mergeAndWriteCachedIndex2();
					else 
						mergeAndWriteCachedIndex();
				}
				else 
				{
					if(m_pCurBarrelInfo)
					{
						m_pBarrelsInfo->deleteLastBarrel();
						m_pCurBarrelInfo = NULL;
						m_pCurDocCount = NULL;
						m_pCurMinDocID = NULL;
					}
					m_pIndex->setDirty();					
					m_pIndexMerger->merge(m_pBarrelsInfo);
					
				}
				delete m_pIndexMerger;
				m_pIndexMerger = NULL;
			}		
			else 
			{
				if(m_pIndexBarrelWriter && m_pCurDocCount && ((*m_pCurDocCount) > 0) )
					writeCachedIndex();
				else 
				{
					if(m_pCurBarrelInfo)
					{
						m_pBarrelsInfo->deleteLastBarrel();
						m_pCurBarrelInfo = NULL;
						m_pCurDocCount = NULL;
						m_pCurMinDocID = NULL;
					}
				}
			}			

			if(m_pMemCache)
			{
				delete m_pMemCache;
				m_pMemCache = NULL;
			}
		}

		void CIndexWriter::createMerger()
		{
			//SCOPED_LOCK(LOCK);
			if(!m_pIndexMerger)
			{				
				if(!_tcsicmp(getConfigurator()->mergestrategy.strategy.c_str(),_T("HLM")))
					m_pIndexMerger = new CDBTIndexMerger(m_pIndex->m_pDirectory);
				else if(!_tcsicmp(getConfigurator()->mergestrategy.strategy.c_str(),_T("OPTIMIZE")))
				{
					m_pIndexMerger = new COptimizeMerger(m_pIndex->m_pDirectory);
				}					
				else  if(!_tcsicmp(getConfigurator()->mergestrategy.strategy.c_str(),_T("GP")))
				{
					m_pIndexMerger = new CGPIndexMerger(m_pIndex->m_pDirectory);
				}
				else if(!_tcsicmp(getConfigurator()->mergestrategy.strategy.c_str(),_T("LOG")))
				{
					m_pIndexMerger = new CLogarithmicMerger(m_pIndex->m_pDirectory);
				}
				else if(!_tcsicmp(getConfigurator()->mergestrategy.strategy.c_str(),_T("IMM")))
				{
					m_pIndexMerger = new CImmediateMerger(m_pIndex->m_pDirectory);
				}

				if(m_pIndexMerger)
				{
					m_pIndexMerger->setFactoryFinder(m_pIndex->getFactoryFinder());
					m_pIndexMerger->setParam(getConfigurator()->mergestrategy.param.c_str());
					m_pIndexMerger->setThreshold(getConfigurator()->mergestrategy.gc_threshold.c_str());
				}	
			}			
		}		
		CIndexMerger* CIndexWriter::acquireMerger()
		{
			if(!m_pIndexMerger)
			{
				createMerger();
			}			
			return m_pIndexMerger;
		}

		void CIndexWriter::createBarrelWriter()
		{			
			if(m_pBarrelsInfo->getBarrelCount() > 0)//append
			{
				CBarrelInfo* pLastBarrel = m_pBarrelsInfo->getLastBarrel();
				m_pBarrelsInfo->addBarrel(m_pBarrelsInfo->newBarrel().c_str(),0,0);
				m_pCurBarrelInfo = m_pBarrelsInfo->getLastBarrel();
				m_pCurBarrelInfo->setMinDocID(pLastBarrel->getMinDocID()+pLastBarrel->getDocCount());
			}
			else
			{
				m_pBarrelsInfo->addBarrel(m_pBarrelsInfo->newBarrel().c_str(),0,0);
				m_pCurBarrelInfo = m_pBarrelsInfo->getLastBarrel();
			}
			m_pCurDocCount = &(m_pCurBarrelInfo->m_nNumDocs);
			m_pCurMinDocID = &(m_pCurBarrelInfo->m_nMinDocID);
			if(!m_pMemCache)
				m_pMemCache = new CMemCache((size_t)getConfigurator()->index.memory);
			m_pIndexBarrelWriter = new CIndexBarrelWriter(m_pIndex->getDirectory(),m_pMemCache,m_pCurBarrelInfo->getName().c_str());
			m_pCurBarrelInfo->setWriter(m_pIndexBarrelWriter);				
			CFactoryFinder* pFactories = m_pIndex->getFactoryFinder();
			m_pIndexBarrelWriter->setSchema(m_pIndex->getSchema());
			m_pIndexBarrelWriter->setFactoryFinder(pFactories);				
			m_pBarrelsInfo->setSchema(m_pIndex->getSchema());
			createMerger();
		}		
		void CIndexWriter::setConfigurator(const firtex::utility::Configurator& configurator)
		{
			delete m_pConfigurator;			
			m_pConfigurator = new firtex::utility::Configurator(configurator);
		}
		void CIndexWriter::buildIndex(const tchar* pszConfigFile)
		{			
			firtex::utility::Configurator configurator;
			try
			{
				configurator.load(pszConfigFile);
			}
			catch(CFirteXException& fe)			
			{
				FIRTEX_CLOG(level::warn) << _T("parse argument file failed:") << pszConfigFile << FIRTEX_ENDL;
				FIRTEX_RETHROW(fe);
			}
			buildIndex(&configurator);		
		}
		void CIndexWriter::buildIndex(firtex::utility::Configurator* pConfigurator)
		{
			firtex::collection::CCollection* pCol = prepareBuildIndex(pConfigurator);			
			pCol->scan();
			delete pCol;
		}
		
		firtex::collection::CCollection* CIndexWriter::prepareBuildIndex(const tchar* pszConfigFile)
		{
			firtex::utility::Configurator configurator;
			try
			{
				configurator.load(pszConfigFile);
			}
			catch(CFirteXException& fe)			
			{
				FIRTEX_CLOG(level::warn) << _T("parse argument file failed:") << pszConfigFile << FIRTEX_ENDL;
				FIRTEX_RETHROW(fe);
			}
			return prepareBuildIndex(&configurator);
		}
		firtex::collection::CCollection* CIndexWriter::prepareBuildIndex(firtex::utility::Configurator* pConfigurator)
		{
			FIRTEX_ASSERT((pConfigurator!=NULL),_T("Invalid argument:CIndexWriter.buildIndex()"));
			if(pConfigurator->index.source == _T(""))
			{
				FIRTEX_CLOG(level::warn) << _T("please specify source collection path") << FIRTEX_ENDL;
				FIRTEX_THROW2(ILLEGALARGUMENT_ERROR,_T("please specify source collection path"));
				return NULL;
			}
			if(pConfigurator->index.indexlocation == _T(""))
			{
				FIRTEX_CLOG(level::warn) << _T("please specify location of indexes") << FIRTEX_ENDL;
				FIRTEX_THROW2(ILLEGALARGUMENT_ERROR,_T("please specify location of indexes"));
				return NULL;
			}			

			std::auto_ptr<firtex::collection::CCollection> col_ptr(firtex::collection::CCollectionFactory::createCollection(pConfigurator->collection.identifier.c_str(),pConfigurator->index.source.c_str(),pConfigurator->collection.parameter.c_str()));
			if(!col_ptr.get())
			{
				FIRTEX_CLOG(level::warn) << _T("no such CCollection object ::") << pConfigurator->collection.identifier << FIRTEX_ENDL;
				FIRTEX_THROW2(ILLEGALARGUMENT_ERROR,_T("no such CCollection object"));
				return NULL;
			}

			FIRTEX_CLOG(level::info) << _T("Colleciont:") << pConfigurator->collection.identifier << FIRTEX_ENDL;
			std::auto_ptr<firtex::parser::CParser> parser_ptr(firtex::parser::CParserFactory::createParserByIdentifier(pConfigurator->parser.identifier.c_str(),pConfigurator->parser.parameter.c_str()));
			/*if(!parser_ptr.get())
			{				
				FIRTEX_CLOG(level::warn) << _T("no such CParser object ::") << pArgument->parser.identifier << FIRTEX_ENDL;
				FIRTEX_THROW2(ILLEGALARGUMENT_ERROR,_T("no such CParser object"));
				return NULL;
			}*/
			FIRTEX_CLOG(level::info) << _T("Parser:") << pConfigurator->parser.identifier << FIRTEX_ENDL;		

			std::auto_ptr<firtex::analyzer::CAnalyzer> analyzer_ptr(firtex::analyzer::CAnalyzerFactory::createAnalyzer(pConfigurator->analyzer.identifier.c_str(),pConfigurator->analyzer.parameter.c_str()));
			if(!analyzer_ptr.get())
			{				
				FIRTEX_CLOG(level::warn) << _T("No such analyzer object ::") << pConfigurator->analyzer.identifier << FIRTEX_ENDL;
				FIRTEX_THROW2(ILLEGALARGUMENT_ERROR,_T("no such CAnaylzer object"));
				return NULL;
			}			
			FIRTEX_CLOG(level::info) << _T("Analyzer:") << pConfigurator->analyzer.identifier << FIRTEX_ENDL;
			
			CIndex::ACCESS_MODE am;
			if(!_tcsicmp(pConfigurator->index.accessmode.c_str(),_T("w") ))
				am = CIndex::ACCESS_CREATE;
			else if(!_tcsicmp(pConfigurator->index.accessmode.c_str(),_T("a")))
				am = CIndex::ACCESS_APPEND;
			else if(!_tcsicmp(pConfigurator->index.accessmode.c_str(),_T("w+s")))
			{
				am = (CIndex::ACCESS_MODE)(CIndex::ACCESS_CREATE | CIndex::ACCESS_SEARCH);
			}
			else if(!_tcsicmp(pConfigurator->index.accessmode.c_str(),_T("a+s")))
			{
				am = (CIndex::ACCESS_MODE)(CIndex::ACCESS_APPEND | CIndex::ACCESS_SEARCH);
			}
			else
			{
				FIRTEX_CLOG(level::warn) << _T("prepareBuildIndex():Wrong index access mode.") << FIRTEX_ENDL;
				return NULL;
			}

			CIndex* pIndex = new CIndex(pConfigurator->index.indexlocation.c_str(),am);
			if(pConfigurator->getSchema())
				pIndex->setDocSchema(pConfigurator->getSchema());
			CIndexWriter* pWriter = pIndex->acquireWriter(parser_ptr.release(),analyzer_ptr.release());
			pWriter->setConfigurator(*pConfigurator);
			FIRTEX_ASSERT(pWriter,_T("acquire index writer failed."));			

			col_ptr->setWriter(pWriter,true);
			return col_ptr.release();
		}
	}
}
