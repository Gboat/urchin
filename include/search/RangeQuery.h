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
// Created	: 2006/7/16
//
#ifndef _RANGEQUERY_H
#define _RANGEQUERY_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Query.h"
#include "../index/Term.h"
using namespace firtex::index;

namespace firtex
{
	namespace search
	{
		class CRangeQuery :	public CQuery
		{
		public:
			/**
			 * ����CRangeQuery����,lowerTerm��upperTerm������һ��Ϊ��,���ǲ���������Ϊ��,
			 * �������������,������Term���������ͬ���ֶ�
			 * @param lowerTerm ����
			 * @param upperTerm ����
			 * @param bInclusive �Ƿ�����߽�,��[lowerTerm,upperTerm]����(lowerTerm,upperTerm)
			 */
			CRangeQuery(CTerm* lowerTerm,CTerm* upperTerm,bool bInclusive);
			CRangeQuery(const CRangeQuery& clone);
			virtual ~CRangeQuery(void);
		public:
			/** ��ȡ��ѯ���Ʊ�ʶ*/
			const tchar*	getQueryName() const{return _T("rangquery");};
			
			/** get query length */
			int32_t			getQueryLength() const;

			/**
			 * �Բ�ѯ�����Ż�,�����µĲ�ѯ����,���������,Ĭ������·�������
			 * @param pReader ���ڷ�������
			 * @return �Ż�����µĲ�ѯ����,Ĭ������·�������
			 * @memory ���ڲ�����,�ⲿ��Ҫɾ��
			 */
			CQuery*			optimize(firtex::index::CIndexReader* pReader);

			/**
			 * ��¡�µĶ���,������ʵ��
			 * @memory ���ڲ�����,�ⲿɾ��
			 */
			CQuery*			clone();		

			/**
			 * extract terms from query
			 * @return term array, MUST be deleted by the caller
			 */
			CTermArray*		extractTerms();
		private:
			CTerm*	m_pLowerTerm;	//����
			CTerm*	m_pUpperTerm;	//����
			bool	m_bInclusive;	//�Ƿ�����߽�ֵ
		};
	}
}

#endif
