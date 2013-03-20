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
#ifndef _REQPROHSCORER_H
#define _REQPROHSCORER_H

#include "Scorer.h"

namespace firtex
{
	namespace search
	{
		class CReqProhScorer :	public CScorer
		{
#define REQPROHSCORER_PAGESIZE		1024
		public:
			CReqProhScorer(CScorer* pReqScorer,CScorer* pProhScorer);
			virtual ~CReqProhScorer(void);
		public:
			/**
			 * 提取下一批文档
			 * @return 提取成功返回true，已经提取完毕返回false
			 */
			count_t	nextDocs();

			/**
			 * 取文档的打分结果
			 * @param docs 存放文档id，docs地址由内部维护
			 * @param scores 存放文档打分结果，scores地址由内部维护			 
			 * @return 打分的文档总数
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
			CScorer*	m_pReqScorer;
			CScorer*	m_pProhScorer;

			docid_t		m_docs[REQPROHSCORER_PAGESIZE];	//文档ID，内存由CTermDocFreqs维护			
			score_t		m_scores[REQPROHSCORER_PAGESIZE];	//打分结果
			count_t		m_numDocs;						//文档数	
			
		};
	}	
}


#endif
