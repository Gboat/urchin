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
// Created	: 2005/12/28
//
#ifndef _TERMSCORER_H
#define _TERMSCORER_H


#include "Scorer.h"
#include "../index/TermDocFreqs.h"
#include "../index/NormBytes.h"

using namespace firtex::index;

namespace firtex
{
	namespace search
	{
#define	TERMSCORE_CACHESIZE	1024
		class CWeight;
		class CSearcher;		
		class CSimilarity;		
		class CTermScorer :	public CScorer
		{
		public:
			CTermScorer(CWeight* pWeight,CTermDocFreqs* pTermDocs,CSimilarity* pSimilarity,CNormsBytes* norms);
			virtual ~CTermScorer(void);
		public:			

			/**
			 * ȡ��һ������ֵ��ĵ�			 
			 * @return �ĵ�����
			 */
			count_t	nextDocs();

			/**
			 * ����һ���ĵ�id�����ֵ�����뵱 nextDocs() ����ֵ����0ʱ���ò���Ч
			 * @param docs �ĵ���� �ڴ����ڲ�ά��
			 * @param scores �ĵ��ķ�ֵ���ڴ����ڲ�ά��
			 * @return �ĵ�����
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
			CWeight*		m_pWeight;
			CTermDocFreqs*	m_pTermDocs;

			score_t			m_fWeights;
			docid_t*		m_docs;			//����ĵ�ID���ڴ���CTermDocFreqsά��������nextDocs()ʱʹ��
			freq_t*			m_freqs;		//�����Ƶ���ڴ���CTermDocFreqsά��������nextDocs()ʱʹ��
			count_t			m_numDocs;		//�ĵ���

			docid_t			m_doc;			//�����ĵ�ID������skipTo()ʱʹ��
			freq_t			m_freq;			//������Ƶ������skipTo()ʱʹ��

			score_t*		m_scores;		//��ֽ��
			count_t			m_pointer;		//m_scores�������ָ��
			count_t			m_pointerMax;	//m_scores�����С

			score_t			m_scoreCache[TERMSCORE_CACHESIZE];	//Score Cache
			CNormsBytes*	m_pNorms;
		};
	}
}

#endif
