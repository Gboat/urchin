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
// Created	: 2006/1/4
//
#include "index/MultiTermDocs.h"
#include "index/BarrelTermDocFreqs.h"

namespace firtex
{
	namespace index
	{

		CMultiTermDocs::CMultiTermDocs(void)
		{
			m_current = NULL;
			m_pTermDocsQueue = NULL;
		}

		CMultiTermDocs::~CMultiTermDocs(void)
		{
			close();
		}
		bool CMultiTermDocs::seek(CTerm* term)
		{
			return false;
		}
		
		docid_t CMultiTermDocs::doc()
		{
			return m_current->barrelInfo->getMinDocID() + m_current->termDocs->doc();
		}
		
		count_t CMultiTermDocs::freq()
		{
			return m_current->termDocs->freq();
		}
		
		bool CMultiTermDocs::next()
		{
			if(m_pTermDocsQueue == NULL)
			{
				initQueue();
				if(m_current)
					return true;
				return false;
			}

			while (m_pTermDocsQueue->size() > 0)
			{				
				if(m_current->termDocs->next())
					return true;					
				else
				{
					m_pTermDocsQueue->pop();
					if(m_pTermDocsQueue->size() > 0)
					{
						m_current = m_pTermDocsQueue->top();
						return true;
					}
				}	
			}
			return false;			
		}

		count_t CMultiTermDocs::next(docid_t*& docs, count_t*& freqs)
		{
			if(m_pTermDocsQueue == NULL)
			{
				initQueue();
			}

			int c = -1;
			docid_t	mindid; 
			int i ;
			while (m_pTermDocsQueue->size() > 0)
			{
				m_current = m_pTermDocsQueue->top();
				c = m_current->termDocs->next(docs,freqs);
				if(c> 0)				
				{
					mindid = m_current->barrelInfo->getMinDocID();
					if(mindid > 0)
					{
						for (i = 0;i<c;i++)
						{
							docs[i] += mindid;
						}
					}					
					return c;			
				}
				else
					m_pTermDocsQueue->pop();
			}
			return c;			
		}

		bool CMultiTermDocs::skipTo(docid_t target,docid_t& nearTarget)
		{
			if(m_pTermDocsQueue == NULL)
			{
				initQueue();
			}

			CTermDocFreqs* pTop = NULL;
			bool bret = false;
			docid_t mindid;
			docid_t t;
			nearTarget = -1;
			while (m_pTermDocsQueue->size() > 0)
			{
				m_current = m_pTermDocsQueue->top();
				mindid = m_current->barrelInfo->getMinDocID();
				t = target - mindid;
				pTop = m_current->termDocs;
				if( pTop->doc() < t)
				{
					if(pTop->skipTo(t,nearTarget))
					{
						nearTarget += mindid;
						return true;
					}
					else 
					{
						if(nearTarget > t)
							m_pTermDocsQueue->adjustTop();
						else //µ½´ï½áÎ²
							m_pTermDocsQueue->pop();
					}
				}
				else
				{
					nearTarget = pTop->doc() + mindid;
					break;
				}
			}
			return (nearTarget==target);			
		}
		freq_t CMultiTermDocs::docFreq()
		{
			CBarrelTermDocsEntry* pEntry;
			freq_t df = 0;
			list<CBarrelTermDocsEntry*>::iterator iter = m_barrelTermDocs.begin();
			while (iter != m_barrelTermDocs.end())
			{
				pEntry = (*iter);
				df += pEntry->termDocs->docFreq();
				iter++;
			}
			return df;
		}
		int64_t CMultiTermDocs::getCTF()
		{
			CBarrelTermDocsEntry* pEntry;
			int64_t ctf = 0;
			list<CBarrelTermDocsEntry*>::iterator iter = m_barrelTermDocs.begin();
			while (iter != m_barrelTermDocs.end())
			{
				pEntry = (*iter);
				ctf += pEntry->termDocs->getCTF();
				iter++;
			}
			return ctf;
		}

		size_t CMultiTermDocs::setBuffer(int32_t* buffer,size_t length)
		{
			size_t tds = m_barrelTermDocs.size();
			if(tds == 0)
				return 0;
			int32_t* pSubBuff = buffer;
			size_t nSubBuffSize = (length/tds);
			size_t nCurBufferSize = nSubBuffSize;
			size_t nSubBuffUsed = 0;
			CBarrelTermDocsEntry* pEntry;			
			list<CBarrelTermDocsEntry*>::iterator iter = m_barrelTermDocs.begin();
			while (iter != m_barrelTermDocs.end())
			{
				pEntry = (*iter);
				nSubBuffUsed = pEntry->termDocs->setBuffer(pSubBuff,nCurBufferSize);
				pSubBuff += nSubBuffUsed;
				nCurBufferSize = nSubBuffSize + (nCurBufferSize - nSubBuffUsed);
				iter++;
			}	
			return (pSubBuff - buffer);
		}
		size_t CMultiTermDocs::getBufferSize()
		{
			size_t bufSize = 0;
			CBarrelTermDocsEntry* pEntry;
			list<CBarrelTermDocsEntry*>::iterator iter = m_barrelTermDocs.begin();
			while (iter != m_barrelTermDocs.end())
			{
				pEntry = (*iter);
				bufSize += pEntry->termDocs->getBufferSize();
				iter++;
			}	
			return bufSize;
		}

		void  CMultiTermDocs::close()
		{
			list<CBarrelTermDocsEntry*>::iterator iter = m_barrelTermDocs.begin();
			while (iter != m_barrelTermDocs.end())
			{
				delete (*iter);
				iter++;
			}
			m_barrelTermDocs.clear();
			if(m_pTermDocsQueue)
			{
				delete m_pTermDocsQueue;
				m_pTermDocsQueue = NULL;
			}			
			m_current = NULL;
			m_cursor = -1;
		}		

		void CMultiTermDocs::add(CBarrelInfo* pBarrelInfo,CTermDocFreqs* pTermDocs)
		{
			m_barrelTermDocs.push_back(new CBarrelTermDocsEntry(pBarrelInfo,pTermDocs));
			if(m_current == NULL)
				m_current = m_barrelTermDocs.front();
		}
		void CMultiTermDocs::initQueue()
		{
			m_pTermDocsQueue = new CTermDocsQueue(m_barrelTermDocs.size());
			list<CBarrelTermDocsEntry*>::iterator iter = m_barrelTermDocs.begin();
			CBarrelTermDocsEntry* pEntry;
			while (iter != m_barrelTermDocs.end())
			{
				pEntry = *iter;
				if(pEntry->termDocs->next())
					m_pTermDocsQueue->insert(pEntry);
				iter++;
			}		
			if(m_pTermDocsQueue->size() > 0)
				m_current = m_pTermDocsQueue->top();
			else m_current = NULL;
		}
	}
}

