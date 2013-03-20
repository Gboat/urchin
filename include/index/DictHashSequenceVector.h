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
// Created	: 2006/7/4
//
#ifndef __WORDSEQUENCEVECTOR_H
#define __WORDSEQUENCEVECTOR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TermSequenceVector.h"
#include "../store/IndexInput.h"

using namespace firtex::store;

namespace firtex
{
	namespace index
	{
		class CDictHashSequenceVector : public CTermSequenceVector
		{
		public:
			CDictHashSequenceVector();
			CDictHashSequenceVector(const tchar* field,CIndexInput* pTVVInput);
			virtual ~CDictHashSequenceVector(void);
		public:

			/**
			 * ��Term Vector��׼����ȡTerm Vector ��Ϣ
			 * @param field �ֶ���
			 * @param pInput ��ȡλ��,����Ϊ�ڲ�����,�ڲ�����¡һ��
			 */
			void	open(const tchar* field,CIndexInput* pInput);
			/**
			 * ��ȡ������
			 */
			count_t			numTerms();

			/**
			 * ��ȡ������,�����С��<code>numTerms</code>�ķ���ֵһ��
			 * @return ����������
			 * @memory �����ڲ�������ָ��,�ⲿ��Ҫɾ��
			 */
			const termid_t*	getTermIDs();
		protected:
			/**
			 * ����ֶΣ������ֶε�ǰ����������{@link CForwardIndexWriter}���ã��ⲿ����ֱ��ʹ��
			 * @param pOutput ǰ������������豸
			 * @param pField �ֶζ���ָ��
			 */
			void	addField(CIndexOutput*	pOutput,CField* pField);
		private:
			count_t			m_numTerms;
			termid_t*		m_termIds;
			CIndexInput*	m_pTVVInput;
		};
	}
}

#endif
