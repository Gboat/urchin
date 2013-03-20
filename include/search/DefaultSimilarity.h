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
// Created	: 2006/5/9
//
#ifndef _DEFAULT_SIMILARITY_H
#define _DEFAULT_SIMILARITY_H

#include "Similarity.h"

namespace firtex
{
	namespace search
	{
		class CDefaultSimilarity :	public CSimilarity
		{
		public:
			CDefaultSimilarity(void);
			virtual ~CDefaultSimilarity(void);
		public:				

			/**
			 * 计算tf
			 * @param freq 词频
			 * @return tf值
			 */
			float tf(float freq);

			/**
			 * 计算idf
			 * @param docFreq 文档频率
			 * @param numDocs 文档总数
			 * @return idf值
			 */
			float idf(freq_t docFreq, count_t numDocs);

			float sloppyFreq(int distance);

			float lengthNorm(count_t numTerms);

			float queryNorm(float sumOfSquaredWeights);

			float coord(int overlap, int maxOverlap);
		};
	}
}


#endif
