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
			 * 打开Term Vector，准备读取Term Vector 信息
			 * @param field 字段名
			 * @param pInput 读取位置,不作为内部对象,内部将克隆一份
			 */
			void	open(const tchar* field,CIndexInput* pInput);
			/**
			 * 获取词总数
			 */
			count_t			numTerms();

			/**
			 * 获取词序列,数组大小与<code>numTerms</code>的返回值一致
			 * @return 词序列数组
			 * @memory 返回内部的数组指针,外部不要删除
			 */
			const termid_t*	getTermIDs();
		protected:
			/**
			 * 添加字段，建立字段的前向索引，供{@link CForwardIndexWriter}调用，外部不能直接使用
			 * @param pOutput 前向索引的输出设备
			 * @param pField 字段对象指针
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
