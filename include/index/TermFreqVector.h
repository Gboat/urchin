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
			 * 打开Term Vector，准备读取Term Vector 信息
			 * @param field 字段名
			 * @param pInput 读取位置,不作为内部对象,内部将克隆一份
			 */
			virtual void	open(const tchar* field,CIndexInput* pInput) = 0;
			/**
			 * 获取关联的字段名			 
			 */
			virtual const tchar*	getField(){return m_field.c_str();}

			/**
			 * 获取总词数
			 */
            virtual	count_t			numTerms() = 0;

			/**
			 * 获取非重复词总数
			 */
            virtual	count_t			numDistinctTerms() = 0;

			/**
			 * 获取词数组，数组大小与<code>numDistinctTerms</code>返回的数值一致
			 * @return Term数组指针
			 * @memory 返回内部的数组指针,外部不要删除
			 */
			virtual const CTerm*	getTerms() = 0;

			/**
			 * 获取词ID数组，数组大小与<code>numDistinctTerms</code>返回的数值一致
			 * @return 数组指针
			 * @memory 返回内部的数组指针，外部不要删除
			 */
			virtual const termid_t*	getTermIDs() = 0;

			/**
			 * 获取词频数组,数组大小与<code>numDistinctTerms</code>返回的数值一致
			 * @return 词频数组
			 * @memory 返回内部的数组指针,外部不要删除
			 */
			virtual const freq_t*	getTermFrequencies() = 0;

			/**
			 * 获取前向索引
			 */
			virtual void			getTermFreqVector(termid_t*& tids,freq_t*& freqs,count_t& size) = 0;

			/**
			 * 获取词tid在<code>getTermIDs</code>返回的数组中的下标
			 * @param tid 词ID
			 * @return 下标,如果tid没有出现则返回-1
			 */
			virtual int				indexOf(termid_t tid) = 0;
		protected:
			/**
			 * 添加字段，建立字段的前向索引，供{@link CForwardIndexWriter}调用，外部不能直接使用
			 * @param pOutput 前向索引的输出设备
			 * @param pField 字段对象指针
			 */
			virtual void	addField(CIndexOutput*	pOutput,CField* pField) = 0;
		protected:
			CFXString	m_field;

			friend class CForwardIndexWriter;
		};
	}
}

#endif
