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
// Created	: 2006/7/12
//
#ifndef _FILTER_H
#define _FILTER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../utility/BitVector.h"
#include "../index/IndexReader.h"

using namespace firtex::utility;
using namespace firtex::index;

namespace firtex
{
	namespace search
	{
		class CFilter
		{
		public:

			CFilter(void){}
			virtual ~CFilter(void){}
		public:
			
			/**
			 * 获取Filter的位向量
			 * @param pReader 索引读接口
			 * @return Filter的位向量
			 * @memory 返回的Filter的位向量属于内部对象,外部不能删除
			 */
			virtual const CBitVector* bits(CIndexReader* pReader) = 0;
		};
	}
}

#endif
