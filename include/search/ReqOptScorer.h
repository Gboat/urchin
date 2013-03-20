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
#ifndef _REQOPTSCORER_H
#define _REQOPTSCORER_H

#include "Scorer.h"

namespace firtex
{
	namespace search
	{
		class CReqOptScorer : public CScorer
		{
		public:
			CReqOptScorer(CScorer* pReqScorer,CScorer* pOptScorer);
			virtual ~CReqOptScorer(void);
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
			CScorer*	m_pReqScorer;
			CScorer*	m_pOptScorer;		
		};
	}
}


#endif
