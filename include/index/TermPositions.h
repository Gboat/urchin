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
// Created	: 2005/7/17
//
#ifndef _TERMPOSITIONS_H
#define _TERMPOSITIONS_H

#include "TermDocFreqs.h"

namespace firtex
{
	namespace index
	{
		class CTermPositions : public CTermDocFreqs
		{
		public:

			CTermPositions(void)
			{
			}

			virtual ~CTermPositions(void)
			{
			}
		public:
			
			/**
			 * 下一个位置
			 */
			virtual loc_t nextPosition()=0;
			
			/**
			 * 下一块位置
			 * @param positions 位置数组
			 * @return 实际位置个数，positions数组大小
			 */
			virtual count_t nextPositions(loc_t*& positions)=0;

			/** 
			 * 移动到第一个位置大于或等于target的位置			 
			 * @param target 目标位置
			 * @param nearTarget 当搜索成功时nearTarget=target,否则，返回大于target并且与target最接近的位置,
			 *             如果target大于所有值则返回小于target且与target最接近的位置
			 * @return 仅当存在target位置时返回true
			 */ 
			virtual bool skipToPosition(loc_t target,loc_t& nearTarget)=0;
		};
	}
}


#endif
