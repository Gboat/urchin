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
// Email	: ruijieguo@software.ict.ac.cn,ruijieguo@gmail.com
// Created	: 2005/12/27
//
#include "search/IndexSearcher.h"
#include "search/Scorer.h"
#include "search/Query.h"
#include "index/Index.h"
#include "search/QueryParser.h"
#include "analyzer/AnalyzerFactory.h"
#include <memory>

namespace firtex
{
	namespace search
	{
		CIndexSearcher::CIndexSearcher(void)
			: m_pIndexReader(NULL)
			, m_bDestroyIndex(false)
			, m_pQueryParser(NULL)
		{			
		}		
		CIndexSearcher::CIndexSearcher(const tchar* indexpath)
			: m_pQueryParser(NULL)
		{
			CIndex* pIndex = new CIndex(indexpath,CIndex::ACCESS_SEARCH);
			m_pIndexReader = pIndex->acquireReader();
			m_bDestroyIndex = true;
		}
		CIndexSearcher::CIndexSearcher(CIndexReader* pIndexReader)
			: m_pIndexReader(pIndexReader)
			, m_bDestroyIndex(false)
			, m_pQueryParser(NULL)
		{				
		}
		CIndexSearcher::~CIndexSearcher(void)
		{
			if(m_bDestroyIndex)
			{
				delete m_pIndexReader->getIndex();
			}
			m_pIndexReader = NULL;

			if(m_pQueryParser)
			{
				delete m_pQueryParser;
				m_pQueryParser = NULL;
			}
		}

		CQueryHits* CIndexSearcher::search(CQuery* pQuery,int32_t nDocs)
		{			
			

			if(pQuery == NULL)
				return NULL;
			CQuery* pOptQuery = this->optimizeQuery(pQuery);			
			if(!pOptQuery)
				return NULL;
			CScorer* scorer = pOptQuery->scorer(this,getBuffer(),getBufferSize(),NULL);
			if (scorer == NULL)
			{
				if(pOptQuery != pQuery)
					delete pOptQuery;
				return NULL;
			}
			
			CHitQueue* hq = new CHitQueue(nDocs);
			if(!hq)
			{
				delete scorer;
				if(pOptQuery != pQuery)
					delete pOptQuery;
				return NULL;
			}
            
			count_t totalHits = 0;			

			CSimpleTopDocsCollector hitCollector(hq,&totalHits,nDocs);
			scorer->score( &hitCollector);	//打分
			delete scorer;
			if(totalHits == 0)
			{
				delete hq;
				if(pOptQuery != pQuery)
					delete pOptQuery;
				return NULL;
			}

			CQueryHits* pHits = new CQueryHits(hq,pQuery,this);
			pHits->m_totalHits = totalHits;

			delete hq;

			if(pOptQuery != pQuery)
				delete pOptQuery;

			return pHits;
		}

		CQueryHits* CIndexSearcher::search(CQuery* pQuery,CFilter* pFilter,int32_t nDocs)
		{			
		//	IndexReadGuard readGuard(m_pIndexReader);

			if(pQuery == NULL)
				return NULL;
			CScorer* scorer = pQuery->scorer(this);
			if (scorer == NULL)
			{
				return NULL;
			}

			CHitQueue* hq = new CHitQueue(nDocs);
			if(!hq)
			{
				delete scorer;
				return NULL;
			}

			count_t totalHits = 0;			

			CSimpleTopDocsCollector hitCollector(hq,&totalHits,nDocs,(pFilter==NULL)?NULL:pFilter->bits(m_pIndexReader));
			scorer->score( &hitCollector);	//打分
			delete scorer;

			CQueryHits* pHits = new CQueryHits(hq,pQuery,this);
			pHits->m_totalHits = totalHits;

			delete hq;

			return pHits;
		}

		CQueryHits* CIndexSearcher::search(CQuery* pQuery,CFilter* pFilter,CSort* pSort,int32_t nDocs)
		{			
		//	IndexReadGuard readGuard(m_pIndexReader);

			if(pQuery == NULL)
				return NULL;
			CScorer* scorer = pQuery->scorer(this);
			if (scorer == NULL)
			{
				return NULL;
			}

			CFieldDocSortedHitQueue* hq = new CFieldDocSortedHitQueue(m_pIndexReader,pSort,nDocs);
			if(!hq)
			{
				delete scorer;
				return NULL;
			}

			count_t totalHits = 0;			

			CSortedTopDocsCollector hitCollector(hq,&totalHits,nDocs,(pFilter==NULL)?NULL:pFilter->bits(m_pIndexReader));
			scorer->score( &hitCollector);	//打分
			delete scorer;

			CQueryHits* pHits = new CQueryHits(hq,pQuery,this);
			pHits->m_totalHits = totalHits;

			delete hq;

			return pHits;
		}

		CQueryHits*	CIndexSearcher::search(const tchar* query,const tchar* field,int32_t nDocs)
		{
			if(!m_pQueryParser)
			{
				m_pQueryParser = CQueryParser::createQueryParser(this->getReader());
			}		

			CQuery* pQuery = m_pQueryParser->parse(query,field);
			std::auto_ptr<CQuery> query_ptr(pQuery);
			return search(pQuery,nDocs);
		}
		
		CQueryHits* CIndexSearcher::search(const tchar* query,const tchar* field,CFilter* pFilter,CSort* pSort,int32_t nDocs)
		{
			if(!m_pQueryParser)
			{
				m_pQueryParser = CQueryParser::createQueryParser(this->getReader());
			}

			CQuery* pQuery = m_pQueryParser->parse(query,field);
			std::auto_ptr<CQuery> query_ptr(pQuery);
			return search(pQuery,pFilter,pSort,nDocs);
		}

		freq_t CIndexSearcher::docFreq(CTerm* t)
		{			
			return m_pIndexReader->docFreq(t);			
		}

		document::CDocument* CIndexSearcher::doc(docid_t docID)
		{
		//	IndexReadGuard readGuard(m_pIndexReader);
			return m_pIndexReader->document(docID);
		}

		CTermReader* CIndexSearcher::termReader()
		{			
			return m_pIndexReader->termReader();
		}

		CNormsBytes* CIndexSearcher::norms(const tchar* field)
		{
			return m_pIndexReader->norms(field);
		}

		void CIndexSearcher::attachReader(CIndexReader* pIndexReader)
		{
			if(m_bDestroyIndex && m_pIndexReader)
			{
				delete m_pIndexReader->getIndex();				
			}			
			m_pIndexReader = pIndexReader;
		}

		CIndexReader* CIndexSearcher::detachReader()
		{
			CIndexReader* pIndexReader = m_pIndexReader;
			m_pIndexReader = NULL;			
			return pIndexReader;
		}

		CIndexReader* CIndexSearcher::getReader()
		{
			return m_pIndexReader;
		}

		CQuery* CIndexSearcher::optimizeQuery(CQuery* pOrgQuery)
		{
			CQuery* query = pOrgQuery;
			CQuery* last = pOrgQuery;
			for (CQuery* optQuery = query->optimize(m_pIndexReader);
				optQuery != query;
				optQuery = query->optimize(m_pIndexReader)) 
			{
				if(optQuery)
				{
					query = optQuery;
					if ( query != last && last != pOrgQuery)
					{
						delete last;
					}
					last = query;
				}
				else return NULL;//?				
			}
			return query;
		}
			
		CQueryParser* CIndexSearcher::getQueryParser()
		{
			if(!m_pQueryParser)
			{
				m_pQueryParser = CQueryParser::createQueryParser(this->getReader());
			}
			return m_pQueryParser;
		}
	}
}

