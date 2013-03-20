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
// Email	: ruijieguo@software.ict.ac.cn
// Created	: 2006/7/20
//
#ifndef _MULTITERMITERATOR_H
#define _MULTITERMITERATOR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TermIterator.h"
#include "../utility/PriorityQueue.h"
#include <vector>
using namespace std;

namespace firtex
{
	namespace index
	{
		class CMultiTermIterator :	public CTermIterator
		{
			class CTermIteratorEntry
			{
			public:
				CTermIteratorEntry(CTermIterator* iter):termIterator(iter),term(NULL){}
				~CTermIteratorEntry()
				{
					if(termIterator)
					{
						delete termIterator;
						termIterator = NULL;
					}
					term = NULL;
				}

				bool next()
				{
					if(termIterator == NULL)
						return false;
					bool ret = termIterator->next();
					if(ret == false)
						return false;
					term = (CTerm*)termIterator->term();
					return true;
				}
			public:
				CTermIterator*	termIterator;
				CTerm*			term;
			};

			class CTermIteratorQueue : public CPriorityQueue<CMultiTermIterator::CTermIteratorEntry*>
			{
			public:
				CTermIteratorQueue(size_t size) 
				{
					initialize(size,false);
				}
			protected:
				bool lessThan(CMultiTermIterator::CTermIteratorEntry* o1, CMultiTermIterator::CTermIteratorEntry* o2) 
				{
					return (o1->term->compare(o2->term) < 0);
				}
			};
		public:
			CMultiTermIterator(void);
			virtual ~CMultiTermIterator(void);
		public:
			/**
			 * move to next term
			 * @return false if to the end,otherwise true
			 */
			bool				next();
			
			/** 
			 * move to the first term which equal to or bigger than target
			 * @param target the target term			 
			 * @return true if exist,otherwise false
			 */ 			 
			bool				skipTo(const CTerm* target);

			/**
			 * get current term ,only valid after calling {@link #next()} or {@link #skipTo()} and returning true.
			 * @return term,internal object
			 */
			const CTerm*		term();
			
			/**
			 * get current term info,only valid after calling {@link #next()} or {@link #skipTo()} and returning true.
			 * @return term's info,internal object
			 */
			const CTermInfo*	termInfo();

			/**
			 * get current term's posting(in-memory or on-disk posting),only valid after calling {@link #next()} or {@link #skipTo()} and returning true.
			 * @return term's posting,internal object
			 */
			CPosting*			termPosting();

			/**
			 * get doc freq of current term,only valid after calling {@link #next()} or {@link #skipTo()} and returning true
			 */
			freq_t				docFreq();

			/**
			 * add term iterator
			 * @param iter term iterator,internal object
			 */
			void		addIterator(CTermIterator* iter);
		protected:
			void		initQueue();
		private:
			vector<CMultiTermIterator::CTermIteratorEntry*>	m_iters;
			CMultiTermIterator::CTermIteratorQueue*	m_itersQueue;
			CTerm*				m_pTerm;			
			freq_t				m_docFreq;
		};
	}
}

#endif
