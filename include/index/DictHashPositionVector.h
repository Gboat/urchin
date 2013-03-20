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
			 * @param tids 词ID数组
			 * @param freqs 词频数组
			 * @param size 数组大小
			 */
			void			getTermFreqVector(termid_t*& tids,freq_t*& freqs,count_t& size);

			/**
			 * 获取词tid在<code>getTermIDs</code>返回的数组中的下标
			 * @param tid 词ID
			 * @return 下标,如果tid没有出现则返回-1
			 */
			int				indexOf(termid_t tid);

			///implementation of CTermPositionVector
			/**
			 * 获取词的位置数组
			 * @param term 词对象
			 * @return 词的位置数组
			 * @memory 内部对象，外部不能删除
			 */
			const loc_t*	getTermPositions(const CTerm* term);

			/**
			 * 获取词的位置数组
			 * @param index 词在<code>getTerms</code>或<code>getTermIDs</code>返回的词数组中的下标位置
			 * @return 词的位置数组
			 * @memory 内部对象，外部不能删除
			 */
			const loc_t*	getTermPositions(size_t index);

			/**
			 * 获取词的位置数组			 
			 * @return 词的位置数组
			 * @memory 内部对象，外部不能删除
			 */
			const loc_t*	getTermPositions();

			/**
			 * 获取词的位置数组
			 * @param tid 词ID
			 * @return 词的位置数组
			 * @memory 内部对象，外部不能删除
			 */
			const loc_t*	getTermPositions(termid_t tid);			
		protected:
			/**
			 * 添加字段，建立字段的前向索引，供{@link CForwardIndexWriter}调用，外部不能直接使用
			 * @param pOutput 前向索引的输出设备
			 * @param pField 字段对象指针
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
