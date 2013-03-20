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
#ifndef _TERMSEQUENCEVECTOR_H
#define _TERMSEQUENCEVECTOR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../utility/StdHeader.h"
#include "../utility/FXString.h"
#include "../store/IndexOutput.h"
#include "../store/IndexInput.h"
#include "../document/Field.h"

using namespace firtex::store;
using namespace firtex::document;

namespace firtex
{
	namespace index
	{
		class CTermSequenceVector
		{
		public:

			CTermSequenceVector()
			{
			}
			CTermSequenceVector(const tchar* field) : m_field(field)
			{
			}			
			virtual ~CTermSequenceVector(void)
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
			 * ��ȡ������
			 */
			virtual count_t			numTerms() = 0;

			/**
			 * ��ȡ������,�����С��<code>numTerms</code>�ķ���ֵһ��
			 * @return ����������
			 * @memory �����ڲ�������ָ��,�ⲿ��Ҫɾ��
			 */
			virtual	const termid_t*	getTermIDs() = 0;
		protected:
			/**
			 * ����ֶΣ������ֶε�ǰ����������{@link CForwardIndexWriter}���ã��ⲿ����ֱ��ʹ��
			 * @param pOutput ǰ������������豸
			 * @param pField �ֶζ���ָ��
			 */
			virtual void	addField(CIndexOutput*	pOutput,CField* pField) = 0;
		protected:
			CFXString		m_field;

			friend class CForwardIndexWriter;
		};
	}
}
#endif
