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
#ifndef _TERMFREQVECTOR_H
#define _TERMFREQVECTOR_H

#include "../utility/StdHeader.h"
#include "../utility/FXString.h"
#include "../store/IndexOutput.h"
#include "../store/IndexInput.h"
#include "../document/Field.h"
#include "Term.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

using namespace firtex::utility;
using namespace firtex::document;
using namespace firtex::store;

namespace firtex
{
	namespace index
	{
		class CTermFreqVector
		{
		public:

			CTermFreqVector(void)
			{
			}
			CTermFreqVector(const tchar* field) : m_field(field)
			{
			}

			virtual ~CTermFreqVector(void)
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
			 * ��ȡ�������ֶ���			 
			 */
			virtual const tchar*	getField(){return m_field.c_str();}

			/**
			 * ��ȡ�ܴ���
			 */
            virtual	count_t			numTerms() = 0;

			/**
			 * ��ȡ���ظ�������
			 */
            virtual	count_t			numDistinctTerms() = 0;

			/**
			 * ��ȡ�����飬�����С��<code>numDistinctTerms</code>���ص���ֵһ��
			 * @return Term����ָ��
			 * @memory �����ڲ�������ָ��,�ⲿ��Ҫɾ��
			 */
			virtual const CTerm*	getTerms() = 0;

			/**
			 * ��ȡ��ID���飬�����С��<code>numDistinctTerms</code>���ص���ֵһ��
			 * @return ����ָ��
			 * @memory �����ڲ�������ָ�룬�ⲿ��Ҫɾ��
			 */
			virtual const termid_t*	getTermIDs() = 0;

			/**
			 * ��ȡ��Ƶ����,�����С��<code>numDistinctTerms</code>���ص���ֵһ��
			 * @return ��Ƶ����
			 * @memory �����ڲ�������ָ��,�ⲿ��Ҫɾ��
			 */
			virtual const freq_t*	getTermFrequencies() = 0;

			/**
			 * ��ȡǰ������
			 */
			virtual void			getTermFreqVector(termid_t*& tids,freq_t*& freqs,count_t& size) = 0;

			/**
			 * ��ȡ��tid��<code>getTermIDs</code>���ص������е��±�
			 * @param tid ��ID
			 * @return �±�,���tidû�г����򷵻�-1
			 */
			virtual int				indexOf(termid_t tid) = 0;
		protected:
			/**
			 * ����ֶΣ������ֶε�ǰ����������{@link CForwardIndexWriter}���ã��ⲿ����ֱ��ʹ��
			 * @param pOutput ǰ������������豸
			 * @param pField �ֶζ���ָ��
			 */
			virtual void	addField(CIndexOutput*	pOutput,CField* pField) = 0;
		protected:
			CFXString	m_field;

			friend class CForwardIndexWriter;
		};
	}
}

#endif
