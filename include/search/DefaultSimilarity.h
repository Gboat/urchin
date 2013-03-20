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
			 * ����tf
			 * @param freq ��Ƶ
			 * @return tfֵ
			 */
			float tf(float freq);

			/**
			 * ����idf
			 * @param docFreq �ĵ�Ƶ��
			 * @param numDocs �ĵ�����
			 * @return idfֵ
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
