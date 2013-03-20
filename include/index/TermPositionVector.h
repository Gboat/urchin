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
			 * 打开Term Vector，准备读取Term Vector 信息
			 * @param field 字段名
			 * @param pInput 读取位置,不作为内部对象,内部将克隆一份
			 */
			virtual void	open(const tchar* field,CIndexInput* pInput) = 0;

			/**
			 * 获取词的位置数组
			 * @param term 词对象
			 * @return 词的位置数组
			 * @memory 内部对象，外部不能删除
			 */
			virtual const loc_t* getTermPositions(const CTerm* term) = 0;

			/**
			 * 获取词的位置数组		
			 * @return 词的位置数组
			 * @memory 内部对象，外部不能删除
			 */
			virtual const loc_t* getTermPositions() = 0;

			/**
			 * 获取词的位置数组
			 * @param index 词在<code>getTerms</code>或<code>getTermIDs</code>返回的词数组中的下标位置
			 * @return 词的位置数组
			 * @memory 内部对象，外部不能删除
			 */
			virtual const loc_t* getTermPositions(size_t index) = 0;
		};
	}
}

#endif
