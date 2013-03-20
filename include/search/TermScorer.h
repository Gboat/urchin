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
// Created	: 2005/12/28
//
#ifndef _TERMSCORER_H
#define _TERMSCORER_H


#include "Scorer.h"
#include "../index/TermDocFreqs.h"
#include "../index/NormBytes.h"

using namespace firtex::index;

namespace firtex
{
	namespace search
	{
#define	TERMSCORE_CACHESIZE	1024
		class CWeight;
		class CSearcher;		
		class CSimilarity;		
		class CTermScorer :	public CScorer
		{
		public:
			CTermScorer(CWeight* pWeight,CTermDocFreqs* pTermDocs,CSimilarity* pSimilarity,CNormsBytes* norms);
			virtual ~CTermScorer(void);
		public:			

			/**
			 * 取下一批打完分的文档			 
			 * @return 文档个数
			 */
			count_t	nextDocs();

			/**
			 * 返回一批文档id及其分值，必须当 nextDocs() 返回值大于0时调用才有效
			 * @param docs 文档编号 内存由内部维护
			 * @param scores 文档的分值，内存由内部维护
			 * @return 文档个数
			 */
			count_t	scores(docid_t*& docs,score_t*& scores);

			/** 
			 * 移动到第一个文档编号大于或等于target的位置，一般配合{@link score()}使用
			 * @param target 目标文档编号
			 * @param nearTarget 当搜索成功时nearTarget=target,否则，返回大于target并且与target最接近的doc id,
			 *             如果target大于所有值则返回小于target且与target最接近的doc id
			 * @return 仅当存在这样的文档编号时返回true
			 */ 
			bool	skipTo(docid_t target,docid_t& nearTarget);

			/**
			 * 依次取一个文档进行打分
			 * @return 返回false表明到达结尾
			 */
			bool	next();

			/*
			 * 返回当前位置文档的打分结果，必须配合{@link skipTo(docid_t,DOC_ID)}或{@link next()}使用，只有返回true后才有效
			 */
			score_t score();

			/*
			 * 返回当前位置文档ID，必须配合{@link skipTo(docid_t,DOC_ID)}或{@link next()}使用，只有返回true后才有效
			 */
			docid_t doc();
		protected:
			CWeight*		m_pWeight;
			CTermDocFreqs*	m_pTermDocs;

			score_t			m_fWeights;
			docid_t*		m_docs;			//多个文档ID，内存由CTermDocFreqs维护，调用nextDocs()时使用
			freq_t*			m_freqs;		//多个词频，内存由CTermDocFreqs维护，调用nextDocs()时使用
			count_t			m_numDocs;		//文档数

			docid_t			m_doc;			//单个文档ID，调用skipTo()时使用
			freq_t			m_freq;			//单个词频，调用skipTo()时使用

			score_t*		m_scores;		//打分结果
			count_t			m_pointer;		//m_scores数组访问指针
			count_t			m_pointerMax;	//m_scores数组大小

			score_t			m_scoreCache[TERMSCORE_CACHESIZE];	//Score Cache
			CNormsBytes*	m_pNorms;
		};
	}
}

#endif
