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
// Created	: 2006/3/9
//
#ifndef _SIMILARITY_H
#define _SIMILARITY_H

#include "../utility/StdHeader.h"
#include "../index/Term.h"
#include "../utility/SmallFloat.h"
#include <vector>

using namespace firtex::index;

namespace firtex
{
	namespace search
	{
		class CSearcher;
		class CSimilarity
		{
		public:

			CSimilarity(void);			
			virtual ~CSimilarity(void);
		public:
			/**
			 * 计算tf
			 * @param freq 词频
			 * @return tf值
			 */
			virtual float tf(int freq)
			{
				return tf((float)freq);
			}

			/**
			 * 计算tf
			 * @param freq 词频
			 * @return tf值
			 */
			virtual float tf(float freq) = 0;


			/**
			 * compute idf value
			 * @param term Term
			 * @param searcher Index Searcher
			 * @return idf value
			 */
			virtual float idf(CTerm* term,CSearcher* searcher);

			/**
			 * compute idf value
			 * @param term Term
			 * @param searcher Index Searcher
			 * @return idf value
			 */
			virtual float idf(std::vector<CTerm*>* terms,CSearcher* searcher);

			/**
			 * 计算idf
			 * @param docFreq 文档频率
			 * @param numDocs 文档总数
			 * @return idf值
			 */
			virtual float idf(freq_t docFreq, count_t numDocs) = 0;

			/** Computes the amount of a sloppy phrase match, based on an edit distance.
			 * This value is summed for each sloppy phrase match in a document to form
			 * the frequency that is passed to {@link #tf(float)}.
			 *
			 * <p>A phrase match with a small edit distance to a document passage more
			 * closely matches the document, so implementations of this method usually
			 * return larger values when the edit distance is small and smaller values
			 * when it is large.
			 *
			 * @see CPhraseQuery#setSlop(int)
			 * @param distance the edit distance of this sloppy phrase match
			 * @return the frequency increment for this match
			 */
			virtual float	sloppyFreq(int distance) = 0;
			
			virtual float	lengthNorm(count_t numTerms) = 0;
			virtual float	queryNorm(float sumOfSquaredWeights) = 0;

			/** Computes a score factor based on the fraction of all query terms that a
			 * document contains.  This value is multiplied into scores.
			 *
			 * <p>The presence of a large portion of the query terms indicates a better
			 * match with the query, so implementations of this method usually return
			 * larger values when the ratio between these parameters is large and smaller
			 * values when the ratio between them is small.
			 *
			 * @param overlap the number of query terms matched in the document
			 * @param maxOverlap the total number of terms in the query
			 * @return a score factor based on term overlap with the query
			 */
			virtual float coord(int overlap, int maxOverlap) = 0;

			static CSimilarity* getDefault();
			static void			setDefault(CSimilarity* pDefault);

			/**
			 * Decodes a normalization factor stored in an index.
			 * @see #encodeNorm(float)
			 */
			float decodeNorm(int32_t n) 
			{
				if(m_bNormTableInited == false)
				{
					initNormTable();
					m_bNormTableInited = true;
				}
				byte b = (byte)(n >> 24);
				return (b==0)?lengthNorm(n):(lengthNorm(n&0x00ffffff) * NORM_TABLE[b & 0xFF]);  // & 0xFF maps negative bytes to positive above 127
			}
			
			int32_t encodeNorm(float fBoost,int32_t numTerms)
			{
				byte b = 0;
				int32_t n = (numTerms&0x00ffffff);
				if(fBoost != 1.0)
				{
					b = CSmallFloat::floatToByte315(fBoost);
					n |= ((uint32_t)b)<<24; 
				}				
				return n;				
			};					
		protected:
			static void	initNormTable();
		protected:
			/** Cache of decoded bytes. */
			static float NORM_TABLE[256];
			static bool m_bNormTableInited;
		private:
			static CSimilarity*	m_pDefaultSimilarity;
		};
	}
}

#endif
