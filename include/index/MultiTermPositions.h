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
			 * ��λ��ָ��term
			 */
			bool  seek(CTerm* term);
						
			/**
			 * ���ص�ǰ�ĵ���š��ڵ�һ��û�е���next֮ǰ������Ч
			 */
			docid_t doc();

			/**
			 * ���ص�ǰtf���ڵ�һ��û�е���next֮ǰ������Ч
			 */
			count_t freq();

			/**
			 * ��ǰ�ƶ�
			 * @return ��������һ��termʱ����ture����false
			 */
			bool next();

			/**
			 * ��ǰ�ƶ�һ��
			 * @param docs �ĵ��������ָ��
			 * @param freqs tf����ָ�룬��docsһ����С
			 * @param count �ƶ����С
			 * @return ʵ���ƶ�λ�ô�С��ʵ����docs��freqs���鳤��
			 */			
			count_t next(docid_t*& docs, count_t*& freqs);

			/** 
			 * �ƶ�����һ���ĵ���Ŵ��ڻ����target��λ��
			 * ʵ������������Ĵ���
			 * @param target Ŀ���ĵ����
			 * @param nearTarget �������ɹ�ʱnearTarget=target,���򣬷��ش���target������target��ӽ���doc id,
			 *             ���target��������ֵ�򷵻�С��target����target��ӽ���doc id
			 * @return ���������������ĵ����ʱ����true
			 */ 
			bool skipTo(docid_t target,docid_t& nearTarget);

			/** 
			 * �ƶ�����һ��λ�ô��ڻ����target��λ��			 
			 * @param target Ŀ��λ��
			 * @param nearTarget �������ɹ�ʱnearTarget=target,���򣬷��ش���target������target��ӽ���λ��,
			 *             ���target��������ֵ�򷵻�С��target����target��ӽ���λ��
			 * @return ��������targetλ��ʱ����true
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
			 * �رգ��ͷ���Դ
			 */
			void  close();
			
			/**
			 * ��һ��λ��
			 */
			loc_t nextPosition();
			
			/**
			 * ��һ��λ��
			 * @param positions λ������
			 * @param count ��Ҫ��λ�ø���
			 * @return ʵ��λ�ø�����positions�����С
			 */
			count_t nextPositions(loc_t*& positions);

			/**
			 * ���term positions
			 * @param pTermPositions �����������ص�term Positions
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

