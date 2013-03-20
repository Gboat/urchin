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
// Email	: ruijieguo@software.ict.ac.cn
// Created	: 2006/7/3
//
#ifndef _TERMPOSITIONVECTOR_H
#define _TERMPOSITIONVECTOR_H

#include "TermFreqVector.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace firtex
{
	namespace index
	{
		class CTermPositionVector :	public CTermFreqVector
		{
		public:
			CTermPositionVector()
			{
			}
			CTermPositionVector(const tchar* field) : CTermFreqVector(field)
			{
			}

			virtual ~CTermPositionVector(void)
			{
			}
		public:
			/**
			 * ��Term Vector��׼����ȡTerm Vector ��Ϣ
			 * @param field �ֶ���
			 * @param pInput ��ȡλ��,����Ϊ�ڲ�����,�ڲ�����¡һ��
			 */
			virtual void	open(const tchar* field,CIndexInput* pInput) = 0;

			/**
			 * ��ȡ�ʵ�λ������
			 * @param term �ʶ���
			 * @return �ʵ�λ������
			 * @memory �ڲ������ⲿ����ɾ��
			 */
			virtual const loc_t* getTermPositions(const CTerm* term) = 0;

			/**
			 * ��ȡ�ʵ�λ������		
			 * @return �ʵ�λ������
			 * @memory �ڲ������ⲿ����ɾ��
			 */
			virtual const loc_t* getTermPositions() = 0;

			/**
			 * ��ȡ�ʵ�λ������
			 * @param index ����<code>getTerms</code>��<code>getTermIDs</code>���صĴ������е��±�λ��
			 * @return �ʵ�λ������
			 * @memory �ڲ������ⲿ����ɾ��
			 */
			virtual const loc_t* getTermPositions(size_t index) = 0;
		};
	}
}

#endif
