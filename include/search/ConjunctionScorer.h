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
// Created	: 2006/5/9
//
#ifndef _CONJUNCTIONSCORER_H
#define _CONJUNCTIONSCORER_H

#include <list>
#include "Scorer.h"
using namespace std;

namespace firtex
{
	namespace search
	{		
		class CConjunctionScorer :public CScorer
		{
#define CONJUNCTION_PAGESIZE		1024
			typedef list<CScorer*> ScorerList;
		public:
			CConjunctionScorer(CSimilarity* pSimilarity);
			CConjunctionScorer(list<CScorer*>* scorers,CSimilarity* pSimilarity);
			virtual ~CConjunctionScorer(void);

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
	
			/*
			 * add one scorer
			 * @param pScorer The scorer witch will be added to the m_scoresList
			 */
			void	add(CScorer* pScorer){m_scoresList.push_back(pScorer);}
		protected:
			ScorerList	m_scoresList;

			docid_t		m_docs[CONJUNCTION_PAGESIZE];	//�ĵ�ID���ڴ���CTermDocFreqsά��			
			score_t		m_scores[CONJUNCTION_PAGESIZE];	//��ֽ��
			count_t		m_numDocs;		//�ĵ���			
		};
	}
}

#endif
