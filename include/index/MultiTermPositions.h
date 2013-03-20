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
#ifndef _MULTITERMPOSITIONS_H
#define _MULTITERMPOSITIONS_H

#include "TermPositions.h"
#include "BarrelsInfo.h"
#include "../utility/PriorityQueue.h"
#include <list>
using namespace std;
using namespace firtex::utility;

namespace firtex
{
	namespace index
	{		
		class CBarrelTermPositionsEntry
		{
		public:
			CBarrelTermPositionsEntry(CBarrelInfo*	barrelInfoA,CTermPositions*	termPositionsA)
			{
				barrelInfo = new CBarrelInfo(*barrelInfoA);
				termPositions = termPositionsA;
			}
			~CBarrelTermPositionsEntry()
			{
				delete barrelInfo;
				delete termPositions;
			}
		protected:
			CBarrelTermPositionsEntry(){}
		public:
			CBarrelInfo*	barrelInfo;
			CTermPositions*	termPositions;

			friend class CMultiTermPositions;			
		};

		class CMultiTermPositions :	public CTermPositions
		{
			class CTermPositionQueue:public CPriorityQueue<CBarrelTermPositionsEntry*>
			{
			public:
				CTermPositionQueue(size_t size) 
				{
					initialize(size,false);
				}
			protected:
				bool lessThan(CBarrelTermPositionsEntry* o1, CBarrelTermPositionsEntry* o2) 
				{
					return (o1->barrelInfo->getMinDocID() + o1->termPositions->doc()) < (o2->barrelInfo->getMinDocID() + o2->termPositions->doc());
				}
			};
		public:
			CMultiTermPositions(void);
			virtual ~CMultiTermPositions(void);
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
			 * 实现类似于下面的代码
			 * @param target 目标文档编号
			 * @param nearTarget 当搜索成功时nearTarget=target,否则，返回大于target并且与target最接近的doc id,
			 *             如果target大于所有值则返回小于target且与target最接近的doc id
			 * @return 仅当存在这样的文档编号时返回true
			 */ 
			bool skipTo(docid_t target,docid_t& nearTarget);

			/** 
			 * 移动到第一个位置大于或等于target的位置			 
			 * @param target 目标位置
			 * @param nearTarget 当搜索成功时nearTarget=target,否则，返回大于target并且与target最接近的位置,
			 *             如果target大于所有值则返回小于target且与target最接近的位置
			 * @return 仅当存在target位置时返回true
			 */ 
			bool skipToPosition(loc_t target,loc_t& nearTarget);

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
			 * 关闭，释放资源
			 */
			void  close();
			
			/**
			 * 下一个位置
			 */
			loc_t nextPosition();
			
			/**
			 * 下一块位置
			 * @param positions 位置数组
			 * @param count 需要的位置个数
			 * @return 实际位置个数，positions数组大小
			 */
			count_t nextPositions(loc_t*& positions);

			/**
			 * 添加term positions
			 * @param pTermPositions 从搜索器返回的term Positions
			 */
			void	add(CBarrelInfo* pBarrelInfo,CTermPositions* pTermPositions);
		protected:
			void	initQueue();
		protected:
			list<CBarrelTermPositionsEntry*>	m_TermPositions;			
			CBarrelTermPositionsEntry*		m_current;
			CTermPositionQueue*				m_pTermPositionQueue;
		};
	}
}


#endif

