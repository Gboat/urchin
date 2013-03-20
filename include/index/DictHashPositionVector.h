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
// Created	: 2006/7/15
//
#ifndef _WORDPOSITIONVECTOR_H
#define _WORDPOSITIONVECTOR_H

#include "TermFreqVector.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TermPositionVector.h"
#include "../store/IndexInput.h"
#include "../utility/DynamicArray.h"
using namespace firtex::store;
using namespace firtex::utility;

namespace firtex
{
	namespace index
	{
		class CDictHashPositionVector : public CTermPositionVector
		{			
			class loc_t_nullvalue
			{
			public:
				inline operator loc_t()const {return -1;}
			};

			class FreqPos
			{
			public:
				FreqPos(int32_t nMaxPos)
					: freq(0)
					, maxPos(nMaxPos)
					, positions(new loc_t[nMaxPos])
				{
				}
				~FreqPos()
				{
					if(positions)
					{
						delete[] positions;
						positions = NULL;
					}
				}
			public:
				inline void addPosition(loc_t nPos)
				{
					positions[freq++] = nPos;
				}
			protected:
				freq_t	freq;
				loc_t*	positions;				
				int32_t	maxPos;

				friend class CDictHashPositionVector;
			};			
		public:
			CDictHashPositionVector();
			CDictHashPositionVector(const tchar* field,CIndexInput* pTVVInput);
			virtual ~CDictHashPositionVector(void);
		public:
			/**
			 * ��Term Vector��׼����ȡTerm Vector ��Ϣ
			 * @param field �ֶ���
			 * @param pInput ��ȡλ��,����Ϊ�ڲ�����,�ڲ�����¡һ��
			 */
			void	open(const tchar* field,CIndexInput* pInput);
			/**
			 * ��ȡ�ܴ���
			 */
            count_t			numTerms();

			/**
			 * ��ȡ���ظ�������
			 */
            count_t			numDistinctTerms();

			/**
			 * ��ȡ�����飬�����С��<code>numDistinctTerms</code>���ص���ֵһ��
			 * @return Term����ָ��
			 * @memory �����ڲ�������ָ��,�ⲿ��Ҫɾ��
			 */
			const CTerm*	getTerms();

			/**
			 * ��ȡ��ID���飬�����С��<code>numDistinctTerms</code>���ص���ֵһ��
			 * @return ����ָ��
			 * @memory �����ڲ�������ָ�룬�ⲿ��Ҫɾ��
			 */
			const termid_t*	getTermIDs();

			/**
			 * ��ȡ��Ƶ����,�����С��<code>numDistinctTerms</code>���ص���ֵһ��
			 * @return ��Ƶ����
			 * @memory �����ڲ�������ָ��,�ⲿ��Ҫɾ��
			 */
			const freq_t*	getTermFrequencies();

			/**
			 * ��ȡǰ������
			 * @param tids ��ID����
			 * @param freqs ��Ƶ����
			 * @param size �����С
			 */
			void			getTermFreqVector(termid_t*& tids,freq_t*& freqs,count_t& size);

			/**
			 * ��ȡ��tid��<code>getTermIDs</code>���ص������е��±�
			 * @param tid ��ID
			 * @return �±�,���tidû�г����򷵻�-1
			 */
			int				indexOf(termid_t tid);

			///implementation of CTermPositionVector
			/**
			 * ��ȡ�ʵ�λ������
			 * @param term �ʶ���
			 * @return �ʵ�λ������
			 * @memory �ڲ������ⲿ����ɾ��
			 */
			const loc_t*	getTermPositions(const CTerm* term);

			/**
			 * ��ȡ�ʵ�λ������
			 * @param index ����<code>getTerms</code>��<code>getTermIDs</code>���صĴ������е��±�λ��
			 * @return �ʵ�λ������
			 * @memory �ڲ������ⲿ����ɾ��
			 */
			const loc_t*	getTermPositions(size_t index);

			/**
			 * ��ȡ�ʵ�λ������			 
			 * @return �ʵ�λ������
			 * @memory �ڲ������ⲿ����ɾ��
			 */
			const loc_t*	getTermPositions();

			/**
			 * ��ȡ�ʵ�λ������
			 * @param tid ��ID
			 * @return �ʵ�λ������
			 * @memory �ڲ������ⲿ����ɾ��
			 */
			const loc_t*	getTermPositions(termid_t tid);			
		protected:
			/**
			 * ����ֶΣ������ֶε�ǰ����������{@link CForwardIndexWriter}���ã��ⲿ����ֱ��ʹ��
			 * @param pOutput ǰ������������豸
			 * @param pField �ֶζ���ָ��
			 */
			void	addField(CIndexOutput*	pOutput,CField* pField);

			/** read forward index */
			void	readForwardIndex();
		private:
			typedef CDynamicArray<CDictHashPositionVector::FreqPos*,Const_NullValue<CDictHashPositionVector::FreqPos*> > freqpos_array;

			freqpos_array*	m_pTermDictionary;

			termid_t*		m_termIDs;
			freq_t*			m_termFreqs;
			loc_t*			m_termPositions;
			count_t			m_nNumTerms;
			count_t			m_nNumDistinctTerms;

			CIndexInput*	m_pFIVInput;			
		};
	}
}

#endif
