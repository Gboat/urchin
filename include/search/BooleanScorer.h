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
#ifndef _BOOLEANSCORER_H
#define _BOOLEANSCORER_H

#include <list>
#include "Scorer.h"
using namespace std;

namespace firtex
{
	namespace search
	{
		typedef list<CScorer*> ScorerList;
		class CSimilarity;
		class CBooleanScorer :	public CScorer
		{
			class CSingleMatchScorer:public CScorer
			{
			public:
				CSingleMatchScorer(CScorer* pScorer):m_pScorer(pScorer)
				{					
				}
				~CSingleMatchScorer(){delete m_pScorer;}
			public:			
				count_t	nextDocs(){return m_pScorer->nextDocs();}				
				count_t	scores(docid_t*& docs,score_t*& scores){return m_pScorer->scores(docs,scores);};
				bool	skipTo(docid_t target,docid_t& nearTarget){return m_pScorer->skipTo(target,nearTarget);}
				bool	next(){return m_pScorer->next();}
				docid_t	doc(){return m_pScorer->doc();}
				score_t	score(){return m_pScorer->score();}
			protected:
				CScorer* m_pScorer;
			};

		public:
			CBooleanScorer(CSimilarity* pSimilarity,int minNrShouldMatch=1);
			virtual ~CBooleanScorer(void);
		public:		
			count_t	nextDocs();			
			count_t	scores(docid_t*& docs,score_t*& scores);
			bool	skipTo(docid_t target,docid_t& nearTarget);
			bool	next();
			docid_t	doc();
			score_t	score();
			
			void	add(CScorer* pScorer,bool bRequired,bool bProhibited);
		protected:
			CScorer*	makeScorer();
			CScorer*	makeScorerNoReq();
			CScorer*	makeScorerSomeReq();
			CScorer*	makeConjunctionScorer(ScorerList* scorerList);
			CScorer*	makeDisjunctionScorer(ScorerList* scorerList,int minNrShouldMatch);
			CScorer*	addProhibitedScorers(CScorer* requiredScorer);
			CScorer*	dualConjunctionScorer(CScorer* req1, CScorer* req2);
		protected:
			int			m_minNrShouldMatch;

			ScorerList	m_requiredScorers;
			ScorerList	m_optionalScorers;
			ScorerList	m_prohibitedScorers;			

			CScorer*	m_pScorer;
		};
	}
}

#endif
