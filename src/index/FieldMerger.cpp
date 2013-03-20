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
// Created	: 2006/12/20
//
#include "index/FieldMerger.h"
#include "utility/Exception.h"
#include "index/Posting.h"
#include "index/FieldFactory.h"
#include "index/IndexBarrelWriter.h"

using namespace firtex::utility;

namespace firtex
{
	namespace index
	{
		CFieldMerger::CFieldMerger()
			:m_buffer(NULL)
			,m_bufsize(0)		
			,m_pMergeQueue(NULL)
			,m_pFactory(NULL)
			,m_pPostingMerger(NULL)
			,m_nNumTermCached(0)
			,m_pDirectory(NULL)
			,m_ppFieldInfos(NULL)
			,m_nNumInfos(0)
		{
		}
		CFieldMerger::CFieldMerger(CFieldFactory* pFactory)
			:m_buffer(NULL)
			,m_bufsize(0)		
			,m_pMergeQueue(NULL)
			,m_pFactory(pFactory)
			,m_pPostingMerger(NULL)
			,m_nNumTermCached(0)
			,m_pDirectory(NULL)
			,m_ppFieldInfos(NULL)
			,m_nNumInfos(0)
		{
			memset(m_cachedTermInfos,0,NUM_CACHEDTERMINFO*sizeof(CMergeTermInfo*));
		}
		CFieldMerger::CFieldMerger(CDirectory* pDirectory,CFieldFactory* pFactory)
			:m_buffer(NULL)
			,m_bufsize(0)		
			,m_pMergeQueue(NULL)
			,m_pFactory(pFactory)
			,m_pPostingMerger(NULL)
			,m_nNumTermCached(0)
			,m_pDirectory(pDirectory)
			,m_ppFieldInfos(NULL)
			,m_nNumInfos(0)
		{
			memset(m_cachedTermInfos,0,NUM_CACHEDTERMINFO*sizeof(CMergeTermInfo*));
		}
		CFieldMerger::~CFieldMerger(void)
		{
			m_buffer = NULL;
			m_bufsize = 0;
			vector<CMergeFieldEntry*>::iterator iter = m_fieldEntries.begin();
			while (iter != m_fieldEntries.end())
			{
				delete (*iter);
				iter++;
			}
			m_fieldEntries.clear();

			if(m_pMergeQueue)
			{
				delete m_pMergeQueue;
				m_pMergeQueue = NULL;
			}
			if(m_pPostingMerger)
			{
				delete m_pPostingMerger;
				m_pPostingMerger = NULL;
			}

			for (int32_t i = 0;i<NUM_CACHEDTERMINFO;i++)
			{
				if(m_cachedTermInfos[i])
				{
					delete m_cachedTermInfos[i];
					m_cachedTermInfos[i] = NULL;
				}
			}

			if(m_ppFieldInfos)
			{
				delete[] m_ppFieldInfos;
				m_ppFieldInfos = NULL;
				m_nNumInfos = 0;
			}

		}	
		void CFieldMerger::addField(CBarrelInfo* pBarrelInfo,CFieldInfo* pFieldInfo,CBitVector* pDocFilter)
		{
			m_fieldEntries.push_back(new CMergeFieldEntry(pBarrelInfo,pFieldInfo,pDocFilter));
			if(m_pPostingMerger == NULL)
			{
				if(pFieldInfo->getIndexLevel() == WORD_LEVEL)
					m_pPostingMerger = new CPostingMerger_WL();
				else if(pFieldInfo->getIndexLevel() == DOCUMENT_LEVEL)
					m_pPostingMerger = NULL;//new CPostingMerger_DL();
				else//  if(pFieldInfo->getIndexLevel() == DOCUMENT_LEVEL_0)
					m_pPostingMerger = NULL;//new CPostingMerger_DL0();
			}
		}

		fileoffset_t CFieldMerger::merge(COutputDescriptor* pOutputDescriptor)
		{
			if(initQueue() == false)///initialize merge queue
				return 0;

			m_pPostingMerger->setOutputDescriptor(pOutputDescriptor);

			m_nMergedTerms = 0;
			int64_t nMergedTerms = 0;
			int32_t nMatch = 0;
			CFieldMergeInfo**	match = new CFieldMergeInfo*[m_pMergeQueue->size()];
			CTerm* pTerm = NULL;
			CFieldMergeInfo* pTop = NULL;
			fileoffset_t postingoffset = 0;

			while (m_pMergeQueue->size() > 0) 
			{
				nMatch = 0;				
				//Pop the first 
				match[nMatch++] = m_pMergeQueue->pop();				
				pTerm = match[0]->m_pCurTerm;				
				//Get the current top of the queue
				pTop = m_pMergeQueue->top();				
				while (pTop != NULL && (pTerm->compare(pTop->m_pCurTerm)==0) )
				{
					//A match has been found so add the matching CFieldMergeInfo to the match array
					match[nMatch++] = m_pMergeQueue->pop();
					//Get the next CFieldMergeInfo
					pTop = m_pMergeQueue->top();
				}				
				
				postingoffset = mergeTerms(match,nMatch);

				if(postingoffset > 0)
				{
					///store merged terms to term info cache
					if(m_cachedTermInfos[m_nNumTermCached] == NULL)
					{
						m_cachedTermInfos[m_nNumTermCached] = new CMergeTermInfo(pTerm->clone(),new CTermInfo(m_pPostingMerger->getPostingDescriptor().df,postingoffset));
					}
					else 
					{                
						m_cachedTermInfos[m_nNumTermCached]->m_pTerm->copyValue(pTerm);
						m_cachedTermInfos[m_nNumTermCached]->m_pTermInfo->set(m_pPostingMerger->getPostingDescriptor().df,postingoffset);
					}
					m_nNumTermCached++;
					if(m_nNumTermCached >= NUM_CACHEDTERMINFO)///cache is exhausted
					{
						flushTermInfo(pOutputDescriptor,m_cachedTermInfos,m_nNumTermCached);
						m_nNumTermCached = 0;
					}	
					nMergedTerms++;
				}
				
				while (nMatch > 0)
				{
					pTop = match[--nMatch];
					
					//Move to the next term i
					if (pTop->next())
					{
						//There still are some terms so restore it in the queue
						m_pMergeQueue->put(pTop);
					}
					else
					{						
						//No terms anymore						
						delete pTop;
					}
				}
			}		
			if(m_nNumTermCached > 0)///flush cache
			{
				flushTermInfo(pOutputDescriptor,m_cachedTermInfos,m_nNumTermCached);
				m_nNumTermCached = 0;
			}	
			delete[] match;
			m_nMergedTerms = nMergedTerms;
			return endMerge(pOutputDescriptor);///merge end here			
		}
		
		void CFieldMerger::setBuffer(char* buf,size_t bufSize)
		{
			m_buffer = buf;
			m_bufsize = bufSize;
		}

		bool CFieldMerger::initQueue()
		{			
			FIRTEX_ASSERT((m_pMergeQueue==NULL),_T("CFieldMerger::initQueue():m_pMergeQueue is not null."));
			if(m_fieldEntries.size() <= 0)
				return false;
			m_pMergeQueue = new CFieldMergeQueue(m_fieldEntries.size());
			CTermReader* pTermReader = NULL;		
			CFieldMergeInfo* pMI = NULL;
			CMergeFieldEntry* pEntry;
			bool bDeleteTermReader;
			size_t nSubBufSize = 0;
			size_t nCurBufferSize = 0;
			size_t nTotalUsed = 0;
			size_t nSubBufUsed = 0;

			///dispatch buffer
			if( (m_bufsize > 0) && (m_fieldEntries.size() > 0))
			{
				nSubBufSize = m_bufsize/m_fieldEntries.size();
				nCurBufferSize = nSubBufSize;
			}

			int32_t nOrder = 0;
			m_nNumInfos = m_fieldEntries.size();
			if(m_nNumInfos > 0)
			{
				m_ppFieldInfos = new CMergeFieldEntry*[m_fieldEntries.size()];
				memset(m_ppFieldInfos,0,m_nNumInfos * sizeof(CMergeFieldEntry*));
			}
			vector<CMergeFieldEntry*>::iterator iter = m_fieldEntries.begin();
			while (iter != m_fieldEntries.end())
			{
				pEntry = (*iter);
				m_ppFieldInfos[nOrder] = pEntry;
				if(pEntry->m_pBarrelInfo->getWriter()) ///in-memory index barrel
				{
					pTermReader = pEntry->m_pBarrelInfo->getWriter()->getFieldIndexer(pEntry->m_pFieldInfo->getID())->termReader();
					bDeleteTermReader = true;
				}	
				else	///on-disk index barrel
				{
					pTermReader = m_pFactory->createTermReader();
					///open on-disk index barrel
					pTermReader->open(m_pDirectory,pEntry->m_pBarrelInfo->getName().c_str(),pEntry->m_pFieldInfo,pEntry->m_pDocFilter);
					bDeleteTermReader = true;
				}
				pMI = new CFieldMergeInfo(nOrder,pEntry->m_pBarrelInfo,pTermReader,bDeleteTermReader);			
				if(nSubBufSize > 0)
				{				
					nSubBufUsed = pMI->m_pIterator->setBuffer(m_buffer + nTotalUsed,nCurBufferSize);
					nTotalUsed += nSubBufUsed;					
				}
				if(pMI->next())	///get first term
				{
					m_pMergeQueue->put(pMI);
					nCurBufferSize = nSubBufSize + (nCurBufferSize - nSubBufUsed);
					nOrder++;
				}
				else
				{
					nTotalUsed -= nSubBufUsed;
					delete pMI;
				}
				iter++;
			}
			m_nNumInfos = nOrder;

			///buffer for posting merging
			if( (m_bufsize - nTotalUsed) > POSTINGMERGE_BUFFERSIZE)
				m_pPostingMerger->setBuffer(m_buffer + nTotalUsed,m_bufsize - nTotalUsed);

			return (m_pMergeQueue->size() > 0);
		}
	}
}
