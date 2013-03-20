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
// Created	: 2006/5/9
//
#include "search/ReqProhScorer.h"

namespace firtex
{
	namespace search
	{
		CReqProhScorer::CReqProhScorer(CScorer* pReqScorer,CScorer* pProhScorer):
		m_pReqScorer(pReqScorer),m_pProhScorer(pProhScorer),m_numDocs(0)
		{			
		}

		CReqProhScorer::~CReqProhScorer(void)
		{			
			if(m_pProhScorer)
			{
				delete m_pProhScorer;
				m_pProhScorer = NULL;
			}
			if(m_pReqScorer)
			{
				delete m_pReqScorer;
				m_pReqScorer = NULL;
			}
		}		

		count_t CReqProhScorer::nextDocs()
		{
			if(m_pReqScorer == NULL)
				return -1;
			m_numDocs = 0;
			count_t numDocs = 0;
			docid_t target = 0,nearTarget = -1;
			while (numDocs < REQPROHSCORER_PAGESIZE) 
			{					
				if(!m_pReqScorer->next())
				{
					delete m_pReqScorer;
					m_pReqScorer = NULL;
					break;
				}
				target = m_pReqScorer->doc();
				if(target == nearTarget)
					continue;

				if( (!m_pProhScorer) || target < nearTarget || (m_pProhScorer->skipTo(target,nearTarget) == false) )
				{
					m_docs[numDocs] = target;
					m_scores[numDocs] = m_pReqScorer->score();
					numDocs++;
					if(nearTarget < target)//到达尾部
					{
						delete m_pProhScorer;
						m_pProhScorer = NULL;
					}
				}
			}			
			m_numDocs = numDocs;
			return numDocs;
		}
		
		count_t	CReqProhScorer::scores(docid_t*& docs,score_t*& scores)
		{
			docs = m_docs;
			scores = m_scores;
			return m_numDocs;
		}

		bool CReqProhScorer::next()
		{
			if(m_pReqScorer == NULL)
				return false;
			m_numDocs = 0;
			count_t numDocs = 0;
			docid_t target = 0,nearTarget = -1;
			do
			{					
				if(!m_pReqScorer->next())
				{
					delete m_pReqScorer;
					m_pReqScorer = NULL;
					break;
				}	
				target = m_pReqScorer->doc();
				if(target == nearTarget)
					continue;

				if( (!m_pProhScorer) || target < nearTarget || (m_pProhScorer->skipTo(target,nearTarget) == false) )
				{
					m_docs[numDocs] = target;
					m_scores[numDocs] = m_pReqScorer->score();
					numDocs++;
					if(nearTarget < target)//到达尾部
						m_pProhScorer = NULL;
					return true;
				}
			}while (numDocs == 0);
			m_numDocs = numDocs;
			return (numDocs==1);
		}
		bool CReqProhScorer::skipTo(docid_t target,docid_t& nearTarget)
		{
			if(m_pReqScorer == NULL)
			{
				nearTarget = target - 1;
				return false;
			}									
			if( (m_pReqScorer->skipTo(target,nearTarget)) && ( (!m_pProhScorer) || (!m_pProhScorer->skipTo(target,nearTarget))) )
			{
				m_docs[0] = target;
				m_scores[0] = m_pReqScorer->score();
				m_numDocs = 1;
				if(nearTarget < target)//到达尾部
					m_pProhScorer = NULL;
				nearTarget = target;
				return true;			
			}
			if(next())
			{
				nearTarget = doc();						
			}
			else 
			{
				nearTarget = target - 1;
			}
			return false;
		}

		score_t CReqProhScorer::score()
		{
			return m_scores[0];
		}
		
		docid_t CReqProhScorer::doc()
		{
			return m_docs[0];
		}
	}
}
