//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: �����(GuoRuijie)
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
			 * ��ȡ��һ���ĵ�
			 * @return ��ȡ�ɹ�����true���Ѿ���ȡ��Ϸ���false
			 */
			count_t	nextDocs();

			/**
			 * ȡ�ĵ��Ĵ�ֽ��
			 * @param docs ����ĵ�id��docs��ַ���ڲ�ά��
			 * @param scores ����ĵ���ֽ����scores��ַ���ڲ�ά��			 
			 * @return ��ֵ��ĵ�����
			 */
			count_t	scores(docid_t*& docs,score_t*& scores);

			/** 
			 * �ƶ�����һ���ĵ���Ŵ��ڻ����target��λ�ã�һ�����{@link score()}ʹ��
			 * @param target Ŀ���ĵ����
			 * @param nearTarget �������ɹ�ʱnearTarget=target,���򣬷��ش���target������target��ӽ���doc id,
			 *             ���target��������ֵ�򷵻�С��target����target��ӽ���doc id
			 * @return ���������������ĵ����ʱ����true
			 */ 
			bool	skipTo(docid_t target,docid_t& nearTarget);

			/**
			 * ����ȡһ���ĵ����д��
			 * @return ����false���������β
			 */
			bool	next();

			/*
			 * ���ص�ǰλ���ĵ��Ĵ�ֽ�����������{@link skipTo(docid_t,DOC_ID)}��{@link next()}ʹ�ã�ֻ�з���true�����Ч
			 */
			score_t score();

			/*
			 * ���ص�ǰλ���ĵ�ID���������{@link skipTo(docid_t,DOC_ID)}��{@link next()}ʹ�ã�ֻ�з���true�����Ч
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
			docid_t		m_docs[PHRASE_PAGESIZE];	//�ĵ�ID���ڴ���CTermDocFreqsά��
			freq_t		m_freqs[PHRASE_PAGESIZE];	//��Ƶ���ڴ���CTermDocFreqsά��
			score_t		m_scores[PHRASE_PAGESIZE];	//��ֽ��
			count_t		m_numDocs;		//�ĵ���

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
					if(m_pFirst->skipToPosition(startPhrase,target,nearTarget))//�ҵ�
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
				if(nMatch == m_nNumPositions)//�ҵ�һ��ƥ����ĵ����
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
