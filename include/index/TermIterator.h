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
#ifndef _TERMITERATOR_H
#define _TERMITERATOR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Term.h"

namespace firtex
{
	namespace index
	{
		class CPosting;
		class CTermInfo;
		class CTermIterator
		{
		public:
			CTermIterator(void);
			virtual ~CTermIterator(void);
		public:
			/**
			 * move to next term
			 * @return false if to the end,otherwise true
			 */
			virtual bool				next() = 0;
			
			/** 
			 * move to the first term which equal to or bigger than target
			 * @param target the target term			 
			 * @return true if exist,otherwise false
			 */ 			 
			virtual bool				skipTo(const CTerm* target) = 0;

			/**
			 * get current term ,only valid after calling {@link #next()} or {@link #skipTo()} and returning true.
			 * @return term,internal object
			 */
			virtual const CTerm*		term() = 0;
			
			/**
			 * get current term info,only valid after calling {@link #next()} or {@link #skipTo()} and returning true.
			 * @return term's info,internal object
			 */
			virtual const CTermInfo*	termInfo() = 0;

			/**
			 * get current term's posting(in-memory or on-disk posting),only valid after calling {@link #next()} or {@link #skipTo()} and returning true.
			 * @return term's posting,internal object
			 */
			virtual CPosting*			termPosting() = 0;

			/**
			 * get doc freq of current term,only valid after calling {@link #next()} or {@link #skipTo()} and returning true
			 */
			virtual freq_t				docFreq() = 0;
		public:
			/**
			 * set buffer for iterator
			 * @param pBuffer buffer, only keep the pointer, caller response for destroying.
			 * @param bufSize size of buffer
			 * @return actual size used
			 */
			virtual size_t	setBuffer(char* pBuffer,size_t bufSize);
		protected:
			char*		m_pBuffer;		///buffer for iterator
			size_t		m_nBuffSize;	///size of buffer
		};		
	}
}

#endif
