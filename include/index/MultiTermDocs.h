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
			 * @param target Ŀ���ĵ����
			 * @param nearTarget �������ɹ�ʱnearTarget=target,���򣬷��ش���target������target��ӽ���doc id,
			 *             ���target��������ֵ�򷵻�С��target����target��ӽ���doc id
			 * @return ���������������ĵ����ʱ����true
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
			 * �رգ��ͷ���Դ
			 */
			void  close();

			/**
			 * ���term docs
			 * @param pTermDocs �����������ص�term docs
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
