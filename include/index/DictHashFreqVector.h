//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: 郭瑞杰(GuoRuijie)
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
			 * 打开Term Vector，准备读取Term Vector 信息
			 * @param field 字段名
			 * @param pInput 读取位置,不作为内部对象,内部将克隆一份
			 */
			void	open(const tchar* field,CIndexInput* pInput);
			/**
			 * 获取总词数
			 */
            count_t			numTerms();

			/**
			 * 获取非重复词总数
			 */
            count_t			numDistinctTerms();

			/**
			 * 获取词数组，数组大小与<code>numDistinctTerms</code>返回的数值一致
			 * @return Term数组指针
			 * @memory 返回内部的数组指针,外部不要删除
			 */
			const CTerm*	getTerms();

			/**
			 * 获取词ID数组，数组大小与<code>numDistinctTerms</code>返回的数值一致
			 * @return 数组指针
			 * @memory 返回内部的数组指针，外部不要删除
			 */
			const termid_t*	getTermIDs();

			/**
			 * 获取词频数组,数组大小与<code>numDistinctTerms</code>返回的数值一致
			 * @return 词频数组
			 * @memory 返回内部的数组指针,外部不要删除
			 */
			const freq_t*	getTermFrequencies();

			/**
			 * 获取前向索引
			 */
			void			getTermFreqVector(termid_t*& tids,freq_t*& freqs,count_t& size);

			/**
			 * 获取词tid在<code>getTermIDs</code>返回的数组中的下标
			 * @param tid 词ID
			 * @return 下标,如果tid没有出现则返回-1
			 */
			int				indexOf(termid_t tid);
		protected:
			/**
			 * 添加字段，建立字段的前向索引，供{@link CForwardIndexWriter}调用，外部不能直接使用
			 * @param pOutput 前向索引的输出设备
			 * @param pField 字段对象指针
			 */
			void	addField(CIndexOutput*	pOutput,CField* pField);

			/**
			 * 快速排序
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
