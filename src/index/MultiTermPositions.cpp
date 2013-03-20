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
#include "index/MultiTermPositions.h"

namespace firtex
{
	namespace index
	{
		CMultiTermPositions::CMultiTermPositions(void)
		{
			m_current = NULL;
			m_pTermPositionQueue = NULL;
		}

		CMultiTermPositions::~CMultiTermPositions(void)
		{
			close();
		}
		
		bool CMultiTermPositions::seek(CTerm* term)
		{
			return false;
		}
		
		docid_t CMultiTermPositions::doc()
		{
			return m_current->barrelInfo->getMinDocID() + m_current->termPositions->doc();
		}
		
		count_t CMultiTermPositions::freq()
		{
			return m_current->termPositions->freq();
		}
		
		bool CMultiTermPositions::next()
		{			
			if(m_pTermPositionQueue == NULL)
			{
				initQueue();
				if(m_current)
					return true;
				return false;
			}

			while (m_pTermPositionQueue->size() > 0)
			{				
				if(m_current->termPositions->next())
				{
					return true;
				}
				else
				{
					m_pTermPositionQueue->pop();
					if(m_pTermPositionQueue->size() > 0)
					{
						m_current = m_pTermPositionQueue->top();
						return true;
					}	
				}
			}
			return false;			
		}
		
		count_t CMultiTermPositions::next(docid_t*& docs, count_t*& freqs)
		{			
			if(m_pTermPositionQueue == NULL)
			{
				initQueue();
			}

			int c = -1;
			docid_t	mindid; 
			int i ;
			while (m_pTermPositionQueue->size() > 0)
			{
				m_current = m_pTermPositionQueue->top();
				c = m_current->termPositions->next(docs,freqs);
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
					m_pTermPositionQueue->pop();
			}
			return c;			
		}
		
		bool CMultiTermPositions::skipTo(docid_t target,docid_t& nearTarget)
		{			
			if(m_pTermPositionQueue == NULL)
			{
				initQueue();
			}

			CTermPositions* pTop = NULL;
			bool bret = false;
			docid_t mindid;
			docid_t t;
			nearTarget = -1;
			while (m_pTermPositionQueue->size() > 0)
			{
				m_current = m_pTermPositionQueue->top();
				mindid = m_current->barrelInfo->getMinDocID();
				t = target - mindid;
				pTop = m_current->termPositions;
				if(pTop->doc() < t)
				{
					if(pTop->skipTo(t,nearTarget))
					{
						nearTarget += mindid;
						return true;
					}
					else 
					{
						if(nearTarget > t)
							m_pTermPositionQueue->adjustTop();
						else //µ½´ï½áÎ²
							m_pTermPositionQueue->pop();
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
		bool CMultiTermPositions::skipToPosition(loc_t target,loc_t& nearTarget)
		{
			return m_current->termPositions->skipToPosition(target,nearTarget);
		}
		freq_t CMultiTermPositions::docFreq()
		{
			CBarrelTermPositionsEntry* pEntry;
			freq_t df = 0;
			list<CBarrelTermPositionsEntry*>::iterator iter = m_TermPositions.begin();
			while (iter != m_TermPositions.end())
			{
				pEntry = (*iter);
				df += pEntry->termPositions->docFreq();
				iter++;
			}
			return df;
		}
		int64_t CMultiTermPositions::getCTF()
		{
			CBarrelTermPositionsEntry* pEntry;
			int64_t ctf = 0;
			list<CBarrelTermPositionsEntry*>::iterator iter = m_TermPositions.begin();
			while (iter != m_TermPositions.end())
			{
				pEntry = (*iter);
				ctf += pEntry->termPositions->getCTF();
				iter++;
			}
			return ctf;
		}

		size_t CMultiTermPositions::setBuffer(int32_t* buffer,size_t length)
		{
			size_t tps = m_TermPositions.size();
			if(tps == 0)
				return 0;
			int32_t* pSubBuff = buffer;
			size_t nSubBuffSize = (length/tps);
			size_t nCurBufferSize = nSubBuffSize;
			size_t	nSubBufUsed = 0;

			CBarrelTermPositionsEntry* pEntry;			
			list<CBarrelTermPositionsEntry*>::iterator iter = m_TermPositions.begin();
			while (iter != m_TermPositions.end())
			{
				pEntry = (*iter);
				nSubBufUsed = pEntry->termPositions->setBuffer(pSubBuff,nCurBufferSize);
				pSubBuff += nSubBufUsed;
				nCurBufferSize = nSubBuffSize + (nCurBufferSize - nSubBufUsed);
				iter++;
			}
			return (pSubBuff - buffer);
		}
		size_t CMultiTermPositions::getBufferSize()
		{
			size_t bufSize = 0;
			CBarrelTermPositionsEntry* pEntry;			
			list<CBarrelTermPositionsEntry*>::iterator iter = m_TermPositions.begin();
			while (iter != m_TermPositions.end())
			{
				pEntry = (*iter);
				bufSize += pEntry->termPositions->getBufferSize();				
				iter++;
			}
			return bufSize;
		}

		void CMultiTermPositions::close()
		{
			list<CBarrelTermPositionsEntry*>::iterator iter = m_TermPositions.begin();
			while (iter != m_TermPositions.end())
			{
				delete (*iter);
				iter++;
			}
			m_TermPositions.clear();
			if(m_pTermPositionQueue)
			{
				delete m_pTermPositionQueue;
				m_pTermPositionQueue = NULL;
			}
			m_current = NULL;
		}
		
		loc_t CMultiTermPositions::nextPosition()
		{
			return m_current->termPositions->nextPosition();
		}
		
		count_t CMultiTermPositions::nextPositions(loc_t*& positions)
		{
			return m_current->termPositions->nextPositions(positions);
		}
		void CMultiTermPositions::add(CBarrelInfo* pBarrelInfo,CTermPositions* pTermPositions)
		{			
			m_TermPositions.push_back(new CBarrelTermPositionsEntry(pBarrelInfo,pTermPositions));
			if(m_current == NULL)
				m_current = m_TermPositions.front();
		}
		void CMultiTermPositions::initQueue()
		{
			m_pTermPositionQueue = new CTermPositionQueue(m_TermPositions.size());
			list<CBarrelTermPositionsEntry*>::iterator iter = m_TermPositions.begin();
			CBarrelTermPositionsEntry* pEntry;
			while (iter != m_TermPositions.end())
			{
				pEntry = *iter;
				if(pEntry->termPositions->next())
					m_pTermPositionQueue->insert(pEntry);
				iter++;
			}		
			if(m_pTermPositionQueue->size() > 0)
				m_current = m_pTermPositionQueue->top();
			else m_current = NULL;
		}
	}
}

