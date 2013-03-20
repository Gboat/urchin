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
// Created	: 2006/3/10
//
#ifndef _SCORER_H
#define _SCORER_H

#include "HitCollector.h"
#include "../utility/typedefs.h"
using namespace firtex;

namespace firtex
{
	namespace search
	{		
		class CSimilarity;
		class CScorer
		{
		public:
			CScorer():m_pSimilarity(NULL)
			{
			}
			CScorer(CSimilarity* pSimilarity):m_pSimilarity(pSimilarity)
			{
			}

			virtual ~CScorer()
			{
				m_pSimilarity = NULL;
			}
		public:
			/**
			 * ���д�֣���ֺ�Ľ�������pCollector������
			 * @param pCollector ��Ŵ�ֽ��������
			 */
			void	score(CHitCollector* pCollector)
			{
				count_t num;
				docid_t* docs;
				score_t* ss;
				while (nextDocs() > 0) 
				{
					num = scores(docs,ss);
					pCollector->collect(docs,ss,num);
				}
			}

			CSimilarity* getSimilarity(){return m_pSimilarity;}

			/**
			 * ��ȡ��һ���ĵ�
			 * @return ��ȡ�ɹ�����true���Ѿ���ȡ��Ϸ���false
			 */
			virtual	count_t	nextDocs()=0;

			/**
			 * ȡ�ĵ��Ĵ�ֽ��,�������{@link nextDocs()}ʹ�ã�������ֵ>0ʱ����Ч��
			 * @param docs ����ĵ�id��docs��ַ���ڲ�ά��
			 * @param scores ����ĵ���ֽ����scores��ַ���ڲ�ά��			 
			 * @return ��ֵ��ĵ�����
			 */
			virtual count_t	scores(docid_t*& docs,score_t*& scores)=0;

			/** 
			 * �ƶ�����һ���ĵ���Ŵ��ڻ����target��λ�ã�һ�����{@link score()}ʹ��
			 * @param target Ŀ���ĵ����
			 * @param nearTarget �������ɹ�ʱnearTarget=target,���򣬷��ش���target������target��ӽ���doc id,
			 *             ���target��������ֵ�򷵻�С��target����target��ӽ���doc id
			 * @return ���������������ĵ����ʱ����true
			 */ 
			virtual bool	skipTo(docid_t target,docid_t& nearTarget)=0;

			/**
			 * ����ȡһ���ĵ����д��
			 * @return ����false���������β
			 */
			virtual bool	next() = 0;

			/*
			 * ���ص�ǰλ���ĵ��Ĵ�ֽ�����������{@link skipTo(docid_t,DOC_ID)}��{@link next()}ʹ�ã�ֻ�з���true�����Ч
			 */
			virtual score_t score()=0;
			/*
			 * ���ص�ǰλ���ĵ�ID���������{@link skipTo(docid_t,DOC_ID)}��{@link next()}ʹ�ã�ֻ�е���skipTo������true�����Ч
			 */
			virtual docid_t doc()=0;
		protected:
			CSimilarity* m_pSimilarity;

		};
	}
}

#endif
