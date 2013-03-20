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
// Email	: ruijieguo@software.ict.ac.cn,ruijieguo@gmail.com
// Created	: 2006/7/17
//
#ifndef _WORDFIELDTERMITERATOR_H
#define _WORDFIELDTERMITERATOR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../utility/StdHeader.h"
#include "TermIterator.h"
#include "DictHashTermReader.h"

namespace firtex
{
	namespace index
	{
		class CInputDescriptor;
		class CDictHashTermReaderImpl;
		///CDictHashTermIterator 
		class CDictHashTermIterator :	public CTermIterator
		{
		public:
			CDictHashTermIterator(CDictHashTermReaderImpl* pTermReader);
			virtual ~CDictHashTermIterator(void);
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
			CDictHashTermReaderImpl*	m_pTermReaderImpl;		///parent term reader
			CTerm*						m_pCurTerm;			///current term in this iterator			
			CTermInfo*					m_pCurTermInfo;		///current term info in this iterator
			CPosting*					m_pCurTermPosting;	///current term's posting in this iterator
			CInputDescriptor*			m_pInputDescriptor;
			int32_t						m_nCurPos;			///current position in this iterator
		};

		///CDictHashInMemoryTermIterator 
		class CDictHashInMemoryTermIterator : public CTermIterator
		{
		public:
			CDictHashInMemoryTermIterator(CDictHashInMemoryTermReader* pTermReader);
			virtual ~CDictHashInMemoryTermIterator();
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
			CDictHashInMemoryTermReader*	m_pTermReader;
			CTerm*							m_pCurTerm;			///current term in this iterator			
			CTermInfo*						m_pCurTermInfo;		///current term info in this iterator
			CInMemoryPosting*				m_pCurTermPosting;	///current term's posting in this iterator
			DynPostingArray::array_iterator	m_aIterator;		///iterator for terms
		};
	}
}

#endif
