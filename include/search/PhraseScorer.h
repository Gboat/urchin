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
// Created	: 2006/3/17
//
#ifndef __PHRASESCORER_H
#define __PHRASESCORER_H

#include "Scorer.h"
#include "../index/TermPositions.h"
#include "../index/NormBytes.h"
using namespace firtex::index;

namespace firtex
{
	namespace search
	{
#define	PHRASESCORE_CACHESIZE	1024
#define PHRASE_PAGESIZE			32768///1024
		class CWeight;
		class CSearcher;		
		class CPhraseScorer :	public CScorer
		{
			class CPhrasePositions
			{
			public:
				CPhrasePositions(CTermPositions* pos,int32_t nPos)
					:m_pTermPositions(pos)
					,m_pNext(NULL)
					,m_nPhrasePosition(nPos)
				{					
				}
				~CPhrasePositions()
				{
					if(m_pNext)
					{
						
						delete m_pNext;
						m_pNext = NULL;
					}
					if(m_pTermPositions)
					{
						delete m_pTermPositions;
						m_pTermPositions = NULL;
					}
				}
			public:				
				bool	skipTo(docid_t target,docid_t& nearTarget)
				{
					return m_pTermPositions->skipTo(target,nearTarget);
				}
				bool	next(){return m_pTermPositions->next();}
				docid_t doc(){return m_pTermPositions->doc();}

				bool	skipToPosition(int32_t startPhrase,loc_t target,loc_t& nearTarget)
				{
					bool bRet = m_pTermPositions->skipToPosition(target + (m_nPhrasePosition - startPhrase),nearTarget);
					nearTarget -= (m_nPhrasePosition - startPhrase);
					return bRet;
				}
				loc_t	nextPosition()
				{
					return m_pTermPositions->nextPosition();
				}				

				bool operator < (const CPhrasePositions& p)const
				{
					return (m_pTermPositions->docFreq() < p.m_pTermPositions->docFreq());
				}

				int32_t	getPhrasePosition(){return m_nPhrasePosition;}
			protected:
				CTermPositions*		m_pTermPositions;
				CPhrasePositions*	m_pNext;
				int32_t					m_nPhrasePosition;

				friend class CPhraseScorer;				
			};

			struct PhrasePositionsGreater
			{
				bool operator () (CPhrasePositions*& left,CPhrasePositions*& right) const
				{
					return ( (*left) < (*right) );
				}
			};

			typedef priority_queue<CPhrasePositions*,deque<CPhrasePositions*>,PhrasePositionsGreater> CPhraseQueue;

		public:
			CPhraseScorer(CWeight* pWeight,CSimilarity* pSimilarity,int32_t slop,CTermPositions** ppPos,int32_t size,CNormsBytes* norms);
			virtual ~CPhraseScorer(void);
		public:
			/**
			 * 提取下一批文档
			 * @return 提取成功返回true，已经提取完毕返回false
			 */
			count_t	nextDocs();

			/**
			 * 取文档的打分结果
			 * @param docs 存放文档id，docs地址由内部维护
			 * @param scores 存放文档打分结果，scores地址由内部维护			 
			 * @return 打分的文档总数
			 */
			count_t	scores(docid_t*& docs,score_t*& scores);

			/** 
			 * 移动到第一个文档编号大于或等于target的位置，一般配合{@link score()}使用
			 * @param target 目标文档编号
			 * @param nearTarget 当搜索成功时nearTarget=target,否则，返回大于target并且与target最接近的doc id,
			 *             如果target大于所有值则返回小于target且与target最接近的doc id
			 * @return 仅当存在这样的文档编号时返回true
			 */ 
			bool	skipTo(docid_t target,docid_t& nearTarget);

			/**
			 * 依次取一个文档进行打分
			 * @return 返回false表明到达结尾
			 */
			bool	next();

			/*
			 * 返回当前位置文档的打分结果，必须配合{@link skipTo(docid_t,DOC_ID)}或{@link next()}使用，只有返回true后才有效
			 */
			score_t score();

			/*
			 * 返回当前位置文档ID，必须配合{@link skipTo(docid_t,DOC_ID)}或{@link next()}使用，只有返回true后才有效
			 */
			docid_t doc();
		protected:
			inline void	firstToLast();

			inline freq_t phraseFreq();
			inline freq_t exactPhraseFreq();
			inline freq_t sloppyPhraseFreq();
		protected:
			CWeight*		m_pWeight;
			CTermDocFreqs*		m_pTermDocs;

			score_t		m_fWeights;
			docid_t		m_docs[PHRASE_PAGESIZE];	//文档ID，内存由CTermDocFreqs维护
			freq_t		m_freqs[PHRASE_PAGESIZE];	//词频，内存由CTermDocFreqs维护
			score_t		m_scores[PHRASE_PAGESIZE];	//打分结果
			count_t		m_numDocs;		//文档数

			score_t		m_scoreCache[PHRASESCORE_CACHESIZE];	//Score Cache

			int32_t				m_nSlop;
			CPhrasePositions*	m_pFirst;
			CPhrasePositions*	m_pLast;
			int32_t				m_nNumPositions;
			CNormsBytes*		m_norms;
		};
		//////////////////////////////////////////////////////////////////////////
		///
		inline void CPhraseScorer::firstToLast()
		{				
			m_pLast->m_pNext = m_pFirst;
			m_pLast = m_pFirst;
			m_pFirst = m_pFirst->m_pNext;				
			m_pLast->m_pNext = NULL;
		}
		inline freq_t CPhraseScorer::phraseFreq()
		{			
			if(m_nSlop > 0)
				return sloppyPhraseFreq();
			else return exactPhraseFreq();		
		}
		inline freq_t CPhraseScorer::exactPhraseFreq()
		{			
			int32_t startPhrase;
			loc_t target=0,nearTarget=1;
			freq_t freq = 0;
			while (true) 
			{				
				target = m_pFirst->nextPosition();
				if(target < 0)
					break;		
				startPhrase = m_pFirst->getPhrasePosition();
				firstToLast();
				nearTarget = target + 1;
				int32_t nMatch = 1;									
				while ( nMatch < m_nNumPositions)
				{					
					if(m_pFirst->skipToPosition(startPhrase,target,nearTarget))//找到
					{
						nMatch++;
						m_pFirst->nextPosition();
					}	
					else
					{
						nMatch = 1;
						if(nearTarget > target)
						{
							target = nearTarget + (m_pFirst->m_nPhrasePosition - startPhrase);
							startPhrase = m_pFirst->getPhrasePosition();							
							nearTarget++;
						}
						else break;

					}
					firstToLast();
				}
				if(nMatch == m_nNumPositions)//找到一个匹配的文档编号
				{
					freq++;
				}
				else 
				{
					break;
				}
			}									
			return freq;
		}

		inline freq_t CPhraseScorer::sloppyPhraseFreq()
		{
			return 0;//TODO:
		}
	}
}

#endif
