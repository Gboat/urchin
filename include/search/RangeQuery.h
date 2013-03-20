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
// Created	: 2006/7/16
//
#ifndef _RANGEQUERY_H
#define _RANGEQUERY_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Query.h"
#include "../index/Term.h"
using namespace firtex::index;

namespace firtex
{
	namespace search
	{
		class CRangeQuery :	public CQuery
		{
		public:
			/**
			 * 构造CRangeQuery对象,lowerTerm和upperTerm可以有一个为空,但是不能两个都为空,
			 * 如果两个都不空,则两个Term必须具有相同的字段
			 * @param lowerTerm 下限
			 * @param upperTerm 上限
			 * @param bInclusive 是否包含边界,即[lowerTerm,upperTerm]或者(lowerTerm,upperTerm)
			 */
			CRangeQuery(CTerm* lowerTerm,CTerm* upperTerm,bool bInclusive);
			CRangeQuery(const CRangeQuery& clone);
			virtual ~CRangeQuery(void);
		public:
			/** 获取查询名称标识*/
			const tchar*	getQueryName() const{return _T("rangquery");};
			
			/** get query length */
			int32_t			getQueryLength() const;

			/**
			 * 对查询进行优化,生成新的查询对象,子类可重载,默认情况下返回自身
			 * @param pReader 用于访问索引
			 * @return 优化后的新的查询对象,默认情况下返回自身
			 * @memory 非内部对象,外部需要删除
			 */
			CQuery*			optimize(firtex::index::CIndexReader* pReader);

			/**
			 * 克隆新的对象,需子类实现
			 * @memory 非内部对象,外部删除
			 */
			CQuery*			clone();		

			/**
			 * extract terms from query
			 * @return term array, MUST be deleted by the caller
			 */
			CTermArray*		extractTerms();
		private:
			CTerm*	m_pLowerTerm;	//下限
			CTerm*	m_pUpperTerm;	//上限
			bool	m_bInclusive;	//是否包含边界值
		};
	}
}

#endif
