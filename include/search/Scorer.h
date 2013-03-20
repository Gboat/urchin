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
// Created	: 2006/3/10
//
#ifndef _SCORER_H
#define _SCORER_H

#include "HitCollector.h"
#include "../utility/typedefs.h"
using namespace firtex;

namespace firtex
{
	namespace search
	{		
		class CSimilarity;
		class CScorer
		{
		public:
			CScorer():m_pSimilarity(NULL)
			{
			}
			CScorer(CSimilarity* pSimilarity):m_pSimilarity(pSimilarity)
			{
			}

			virtual ~CScorer()
			{
				m_pSimilarity = NULL;
			}
		public:
			/**
			 * 进行打分，打分后的结果存放在pCollector容器里
			 * @param pCollector 存放打分结果的容器
			 */
			void	score(CHitCollector* pCollector)
			{
				count_t num;
				docid_t* docs;
				score_t* ss;
				while (nextDocs() > 0) 
				{
					num = scores(docs,ss);
					pCollector->collect(docs,ss,num);
				}
			}

			CSimilarity* getSimilarity(){return m_pSimilarity;}

			/**
			 * 提取下一批文档
			 * @return 提取成功返回true，已经提取完毕返回false
			 */
			virtual	count_t	nextDocs()=0;

			/**
			 * 取文档的打分结果,必须配合{@link nextDocs()}使用，当返回值>0时才有效。
			 * @param docs 存放文档id，docs地址由内部维护
			 * @param scores 存放文档打分结果，scores地址由内部维护			 
			 * @return 打分的文档总数
			 */
			virtual count_t	scores(docid_t*& docs,score_t*& scores)=0;

			/** 
			 * 移动到第一个文档编号大于或等于target的位置，一般配合{@link score()}使用
			 * @param target 目标文档编号
			 * @param nearTarget 当搜索成功时nearTarget=target,否则，返回大于target并且与target最接近的doc id,
			 *             如果target大于所有值则返回小于target且与target最接近的doc id
			 * @return 仅当存在这样的文档编号时返回true
			 */ 
			virtual bool	skipTo(docid_t target,docid_t& nearTarget)=0;

			/**
			 * 依次取一个文档进行打分
			 * @return 返回false表明到达结尾
			 */
			virtual bool	next() = 0;

			/*
			 * 返回当前位置文档的打分结果，必须配合{@link skipTo(docid_t,DOC_ID)}或{@link next()}使用，只有返回true后才有效
			 */
			virtual score_t score()=0;
			/*
			 * 返回当前位置文档ID，必须配合{@link skipTo(docid_t,DOC_ID)}或{@link next()}使用，只有调用skipTo并返回true后才有效
			 */
			virtual docid_t doc()=0;
		protected:
			CSimilarity* m_pSimilarity;

		};
	}
}

#endif
