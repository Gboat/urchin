//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: ¹ùÈð½Ü(GuoRuijie)
// Email	: ruijieguo@software.ict.ac.cn,ruijieguo@gmail.com
// Created	: 2006/5/9
//
#include <math.h>
#include "search/DefaultSimilarity.h"


namespace firtex
{
	namespace search
	{
		CDefaultSimilarity::CDefaultSimilarity(void)
		{
		}

		CDefaultSimilarity::~CDefaultSimilarity(void)
		{
		}
	
		float CDefaultSimilarity::tf(float freq)
		{
			return (float) sqrt(freq);
		}
		
		float CDefaultSimilarity::idf(freq_t docFreq, count_t numDocs)
		{
			return (float) (log((double)numDocs / (double) (docFreq + 1)) + 1.0);
		}

		float CDefaultSimilarity::sloppyFreq(int distance)
		{
			return 1.0f / (distance + 1);
		}

		float CDefaultSimilarity::lengthNorm(count_t numTerms)
		{
			if(numTerms == 0)
				return 0.0f;
			return (float)(1.0f / sqrt((float)numTerms));
		}

		float CDefaultSimilarity::queryNorm(float sumOfSquaredWeights)
		{
			if(sumOfSquaredWeights == 0)
				return 0.0f;
			return (float)(1.0f / sqrt((float)sumOfSquaredWeights));
		}

		float CDefaultSimilarity::coord(int overlap, int maxOverlap)
		{
			return (float)((float)overlap / (float)maxOverlap);
		}
	}
}
