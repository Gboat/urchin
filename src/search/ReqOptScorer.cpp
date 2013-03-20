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
// Created	: 2006/5/9
//
#include "search/ReqOptScorer.h"

namespace firtex
{
	namespace search
	{
		CReqOptScorer::CReqOptScorer(CScorer* pReqScorer,CScorer* pOptScorer)
			:m_pReqScorer(pReqScorer),m_pOptScorer(pOptScorer)
		{
		}

		CReqOptScorer::~CReqOptScorer(void)
		{
			if (m_pReqScorer)
			{
				delete m_pReqScorer;
				m_pReqScorer = NULL;
			}
			if (m_pOptScorer)
			{
				delete m_pOptScorer;
				m_pOptScorer = NULL;
			}			
		}
		count_t	CReqOptScorer::nextDocs()
		{
			return m_pReqScorer->nextDocs();
		}

		count_t	CReqOptScorer::scores(docid_t*& docs,score_t*& scores)
		{			
			count_t count = m_pReqScorer->scores(docs,scores);			
			docid_t nearCurDoc;
			int numDocs = 0;
			while ( (numDocs < count) && m_pOptScorer)
			{
				if(m_pOptScorer->skipTo(docs[numDocs],nearCurDoc))
				{
					scores[numDocs] += m_pOptScorer->score();
				}
				else
				{
					if(nearCurDoc < docs[numDocs])//µ½´ï½áÎ²
					{
						delete m_pOptScorer;
						m_pOptScorer = NULL;
					}									
				} 		
				numDocs++;
			}			
			return count;
		}	
		bool CReqOptScorer::next()
		{
			return m_pReqScorer->next();
		}
		bool CReqOptScorer::skipTo(docid_t target,docid_t& nearTarget)
		{
			return m_pReqScorer->skipTo(target,nearTarget);
		}

		score_t CReqOptScorer::score()
		{
			docid_t curDoc = m_pReqScorer->doc();
			score_t reqScore = m_pReqScorer->score();
			docid_t nearCurDoc;
			if (!m_pOptScorer->skipTo(curDoc,nearCurDoc))
			{
				if(nearCurDoc < curDoc)
				{
					delete m_pOptScorer;
					m_pOptScorer = NULL;
				}				
				return reqScore;
			} 
			else if (m_pOptScorer == NULL) 
			{
				return reqScore;
			}		
			else 
			{
				return reqScore + m_pOptScorer->score();
			}			
		}
		docid_t CReqOptScorer::doc()
		{
			return m_pReqScorer->doc();
		}
	}
}
