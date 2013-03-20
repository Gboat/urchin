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
// Created	: 2006/1/4
//
#ifndef _MULTITERMDOCS_H
#define _MULTITERMDOCS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "TermDocFreqs.h"
#include "BarrelsInfo.h"
#include "../utility/PriorityQueue.h"
#include <list>
using namespace firtex::utility;
using namespace std;

namespace firtex
{
	namespace index
	{
		class CBarrelTermDocsEntry
		{
		public:
			CBarrelTermDocsEntry(CBarrelInfo* barrelInfoA,CTermDocFreqs* termDocsA)
			{
				barrelInfo = new CBarrelInfo(*barrelInfoA);
				termDocs = termDocsA;
			}
			~CBarrelTermDocsEntry()
			{
				delete barrelInfo;
				delete termDocs;
			}
		protected:
			CBarrelTermDocsEntry(){}
		public:
			CBarrelInfo*	barrelInfo;
			CTermDocFreqs*		termDocs;

			friend class CMultiTermDocs;
		};

		class CMultiTermDocs :	public CTermDocFreqs
		{
			class CTermDocsQueue:public CPriorityQueue<CBarrelTermDocsEntry*>
			{
			public:
				CTermDocsQueue(size_t size) 
				{
					initialize(size,false);
				}
			protected:
				bool lessThan(CBarrelTermDocsEntry* o1, CBarrelTermDocsEntry* o2) 
				{
					return (o1->barrelInfo->getMinDocID() + o1->termDocs->doc()) < (o2->barrelInfo->getMinDocID() + o2->termDocs->doc());
				}
			};
		public:
			CMultiTermDocs(void);
			virtual ~CMultiTermDocs(void);
		public:
			/**
			 * 定位到指定term
			 */
			bool  seek(CTerm* term);			
						
			/**
			 * 返回当前文档编号。在第一次没有调用next之前调用无效
			 */
			docid_t doc();

			/**
			 * 返回当前tf。在第一次没有调用next之前调用无效
			 */
			count_t freq();

			/**
			 * 往前移动
			 * @return 当存在下一个term时返回ture否则false
			 */
			bool next();

			/**
			 * 往前移动一块
			 * @param docs 文档编号数组指针
			 * @param freqs tf数组指针，和docs一样大小
			 * @param count 移动块大小
			 * @return 实际移动位置大小，实际是docs和freqs数组长度
			 */			
			count_t next(docid_t*& docs, count_t*& freqs);

			/** 
			 * 移动到第一个文档编号大于或等于target的位置
			 * @param target 目标文档编号
			 * @param nearTarget 当搜索成功时nearTarget=target,否则，返回大于target并且与target最接近的doc id,
			 *             如果target大于所有值则返回小于target且与target最接近的doc id
			 * @return 仅当存在这样的文档编号时返回true
			 */ 
			bool	skipTo(docid_t target,docid_t& nearTarget);

			/** 
			 * set document postings buffer
			 * @param buffer buffer for document postings
			 * @param length length of buffer
			 * @return size of buffer used.
			 */
			size_t	setBuffer(int32_t* buffer,size_t length);

			/** 
			 * get size of buffer
			 * @return get size of buffer
			 */
			size_t	getBufferSize();

			/**
			 * get document frequency
			 * @return frequency
			 */
			freq_t	docFreq();

			/**
			 * get collection's total term frequency
			 * @return CTF value
			 */
			int64_t	getCTF(); 

			/**
			 * 关闭，释放资源
			 */
			void  close();

			/**
			 * 添加term docs
			 * @param pTermDocs 从搜索器返回的term docs
			 */
			void	add(CBarrelInfo* pBarrelInfo,CTermDocFreqs* pTermDocs);
		protected:
			void initQueue();
		protected:
			list<CBarrelTermDocsEntry*>		m_barrelTermDocs;			
			CBarrelTermDocsEntry*			m_current;
			int								m_cursor;
			CTermDocsQueue*					m_pTermDocsQueue;
		};

	}
}

#endif
