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
// Created	: 2006/7/20
//
#include "index/MultiTermIterator.h"
#include "utility/Config.h"
using namespace firtex::utility;

namespace firtex
{
	namespace index
	{
		CMultiTermIterator::CMultiTermIterator(void)
			: m_itersQueue(NULL)
			,m_pTerm(NULL)
			,m_docFreq(0)
		{
		}

		CMultiTermIterator::~CMultiTermIterator(void)
		{
			vector<CTermIteratorEntry*>::iterator iter = m_iters.begin();
			while(iter != m_iters.end())
			{
				delete (*iter);			
				iter++;
			}
			m_iters.clear();
			if(m_pTerm)
			{
				delete m_pTerm; 
				m_pTerm = NULL;
			}			
			if(m_itersQueue)
			{
				delete m_itersQueue;
				m_itersQueue = NULL;
			}
		}

		bool CMultiTermIterator::next()
		{
			if(m_itersQueue == NULL)
			{
				initQueue();
				if(m_itersQueue == NULL)
				{
					FIRTEX_CLOG(level::warn) << _T("No term iterators.") << FIRTEX_ENDL;
					return false;
				}
			}					

			CTermIteratorEntry* top = m_itersQueue->top();
			if (top == NULL) 
			{
				if(m_pTerm)
				{
					delete m_pTerm; 
					m_pTerm = NULL;
				}
				
				return false;
			}

			if(m_pTerm)
			{
				delete m_pTerm; 
				m_pTerm = NULL;
			}
			
			m_pTerm = top->term->clone();
			m_docFreq = 0;
			
			while (top != NULL && m_pTerm->compare(top->term) == 0) 
			{				
				m_docFreq += top->termIterator->docFreq();	  
				if (top->next())
				{					
					m_itersQueue->adjustTop();
				}				
				else m_itersQueue->pop();
				top = m_itersQueue->top();
			}
			return true;
		}
		
		bool CMultiTermIterator::skipTo(const CTerm* target)
		{
			return false;
		}		
		const CTerm* CMultiTermIterator::term()
		{
			return m_pTerm;
		}
		
		freq_t CMultiTermIterator::docFreq()
		{
			return m_docFreq;
		}
		const CTermInfo* CMultiTermIterator::termInfo()
		{
			return NULL;
		}
		CPosting* CMultiTermIterator::termPosting()
		{
			return NULL;
		}

		void CMultiTermIterator::addIterator(CTermIterator* iter)
		{
			m_iters.push_back(new CMultiTermIterator::CTermIteratorEntry(iter));
		}
		void CMultiTermIterator::initQueue()
		{
			if(m_itersQueue || m_iters.size()==0)
				return ;
			m_itersQueue = new CTermIteratorQueue(m_iters.size());
			vector<CTermIteratorEntry*>::iterator iter = m_iters.begin();
			while(iter != m_iters.end())
			{
				if((*iter)->next())
					m_itersQueue->insert(*iter);
				iter++;
			}
		}
	}
}
