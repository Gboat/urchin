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
// Created	: 2006/7/17
//

#ifndef _DATEFIELDTERMITERATOR_H
#define _DATEFIELDTERMITERATOR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../utility/StdHeader.h"
#include "TermIterator.h"
#include "DateTimeIndexer.h"
#include "DateTimeTermReader.h"

namespace firtex
{
	namespace index
	{
		class CInputDescriptor;
		class CDateTimeTermReaderImpl;
		///CDateTimeTermIterator 
		class CDateTimeTermIterator :	public CTermIterator
		{
		public:
			CDateTimeTermIterator(CDateTimeTermReaderImpl* pTermReader);
			CDateTimeTermIterator(CDateTimeTermReaderImpl* pTermReader,CTerm* pLowerTerm,CTerm* pUpperTerm);
			virtual ~CDateTimeTermIterator(void);
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
			 * @return term's doc freq
			 */
			freq_t				docFreq();

			/**
			 * set buffer for iterator
			 * @param pBuffer buffer, only keep the pointer, caller response for destroying.
			 * @param bufSize size of buffer
			 * @return actual size used
			 */
			size_t	setBuffer(char* pBuffer,size_t bufSize);
		private:
			CDateTimeTermReaderImpl*	m_pTermReaderImpl;		///parent term reader
			CTerm*						m_pCurTerm;			///current term in this iterator			
			CTermInfo*					m_pCurTermInfo;		///current term info in this iterator
			CPosting*					m_pCurTermPosting;	///current term's posting in this iterator
			CInputDescriptor*			m_pInputDescriptor;
			int32_t						m_nCurPos;			///current position in this iterator

			CTermX<int64_t>*			m_pLowerTerm;		///lower term of this iterator
			CTermX<int64_t>*			m_pUpperTerm;		///upper term of this iterator
		};

		///CDateTimeInMemoryTermIterator 
		class CDateTimeInMemoryTermIterator : public CTermIterator
		{
		public:
			CDateTimeInMemoryTermIterator(CDateTimeInMemoryTermReader* pTermReader);
			CDateTimeInMemoryTermIterator(CDateTimeInMemoryTermReader* pTermReader,CTerm* pLowerTerm,CTerm* pUpperTerm);			
			virtual ~CDateTimeInMemoryTermIterator();
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
			 * @return term's doc freq
			 */
			freq_t				docFreq();

			/**
			 * set buffer for iterator
			 * @param pBuffer buffer, only keep the pointer, caller response for destroying.
			 * @param bufSize size of buffer
			 * @return actual size used
			 */
			size_t	setBuffer(char* pBuffer,size_t bufSize);
		protected:
			CDateTimeInMemoryTermReader*	m_pTermReader;
			CTerm*							m_pCurTerm;			///current term in this iterator			
			CTermInfo*						m_pCurTermInfo;		///current term info in this iterator
			CInMemoryPosting*				m_pCurTermPosting;	///current term's posting in this iterator
			DateTimeMap::iterator			m_aIterator;		///iterator for terms

			CTermX<int64_t>*				m_pLowerTerm;				///lower term of this iterator
			CTermX<int64_t>*				m_pUpperTerm;				///upper term of this iterator
		};
	}
}

#endif
