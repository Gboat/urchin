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
#ifndef _WORDFREQVECTOR_H
#define _WORDFREQVECTOR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TermFreqVector.h"
#include "../store/IndexInput.h"
#include "../utility/DynamicArray.h"
using namespace firtex::store;
using namespace firtex::utility;


namespace firtex
{
	namespace index
	{
		class CDictHashFreqVector : public CTermFreqVector
		{
			typedef struct _map_item
			{
				termid_t	tid;
				freq_t		freq;
			}map_item;
		public:
			CDictHashFreqVector();
			CDictHashFreqVector(const tchar* field,CIndexInput* pTVVInput);
			virtual ~CDictHashFreqVector(void);
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
			 */
			void			getTermFreqVector(termid_t*& tids,freq_t*& freqs,count_t& size);

			/**
			 * ��ȡ��tid��<code>getTermIDs</code>���ص������е��±�
			 * @param tid ��ID
			 * @return �±�,���tidû�г����򷵻�-1
			 */
			int				indexOf(termid_t tid);
		protected:
			/**
			 * ����ֶΣ������ֶε�ǰ����������{@link CForwardIndexWriter}���ã��ⲿ����ֱ��ʹ��
			 * @param pOutput ǰ������������豸
			 * @param pField �ֶζ���ָ��
			 */
			void	addField(CIndexOutput*	pOutput,CField* pField);

			/**
			 * ��������
			 */
			inline void quickSort(map_item items[], int lo, int hi);

			void	readTermVector();
		private:
			CIndexInput*	m_pTVVInput;
			count_t			m_numTerms;
			count_t			m_numDistinctTerms;
			termid_t*		m_termIDs;
			freq_t*			m_termFreqs;
			CDynamicArray<freq_t>*	m_pTermDictionary;
		};

		//////////////////////////////////////////////////////////////////////////
		//
		inline void CDictHashFreqVector::quickSort(map_item items[], int lo, int hi)
		{
			if (lo >= hi)
				return;

			int mid = (lo + hi) / 2;
			map_item tmp;

			if (items[lo].tid > items[mid].tid) 
			{
				tmp = items[lo];
				items[lo] = items[mid];
				items[mid] = tmp;
			}

			if (items[mid].tid > items[hi].tid)
			{
				tmp = items[mid];
				items[mid] = items[hi];
				items[hi] = tmp;

				if (items[lo].tid > items[mid].tid)
				{
					tmp = items[lo];
					items[lo] = items[mid];
					items[mid] = tmp;
				}
			}

			int left = lo + 1;
			int right = hi - 1;

			if (left >= right)
				return;

			termid_t partition = items[mid].tid;

			for (; ;) 
			{
				while (items[right].tid > partition)
					--right;

				while ( (left < right) && (items[left].tid <= partition))
					++left;

				if (left < right) 
				{
					tmp = items[left];
					items[left] = items[right];
					items[right] = tmp;
					--right;
				}
				else 
				{
					break;
				}
			}

			quickSort(items, lo, left);
			quickSort(items, left + 1, hi);
		}		
	}
}

#endif
