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
// Created	: 2005/12/27
//
#ifndef __TERMSEARCHER_H
#define __TERMSEARCHER_H
#include "../utility/StdHeader.h"
#include "Term.h"
#include "TermDocFreqs.h"
#include "TermPositions.h"
#include "TermInfo.h"
#include "TermIterator.h"
#include "../store/Directory.h"
#include "../utility/BitVector.h"
#include "FieldsInfo.h"
#include <string>

using namespace std;

namespace firtex
{
	namespace index
	{
		class CTermReader
		{
		public:			
			CTermReader(void);
			CTermReader(CBitVector* pDocFilter);
			CTermReader(CFieldInfo* pFieldInfo,CBitVector* pDocFilter);
			virtual ~CTermReader(void);
		public:
			/**
			 * open a index barrel
			 * @param pDirectory index storage
			 * @param barrelname index barrel name 
			 * @param pFieldInfo field information
			 * @param pDocFilter filter for deleted documents
			 */
			virtual void			open(CDirectory* pDirectory,const tchar* barrelname,CFieldInfo* pFieldInfo,CBitVector* pDocFilter);

			/**
			 * get the term iterator
			 * @param pLowerTerm lower bound 
			 * @param pUpperTerm upper bound
			 * @return term iterator, MUST be deleted by caller
			 */
			virtual CTermIterator*	termIterator(CTerm* pLowerTerm,CTerm* pUpperTerm) = 0;			

			/**
			 * get the term iterator
			 * @param field field name
			 * @return term iterator, MUST be deleted by caller			 
			 */
			virtual CTermIterator*	termIterator(const tchar* field) = 0;

			/**
			 * seek a term
			 * @param pTerm term 
			 * @return true if success, otherwise false, if the return value is true then {@link termDocFreqs()} and 
			 * {@link termPositions()}can be called. 
			 */
			virtual bool			seek(CTerm* pTerm) = 0;

			/**
			 * get term's document postings,must be called after calling {@link seek()} success			 
			 * @return return document postings,need to be deleted outside
			 */
			virtual CTermDocFreqs*	termDocFreqs() = 0;

			/**
			 * get term's position postings,must be called after calling {@link seek()} success			 
			 * @return return position postings,need to be deleted outside
			 */
			virtual CTermPositions*	termPositions() = 0;

			/**
			 * get document frequency of a term
			 * @return document frequency
			 */
			virtual	freq_t			docFreq(CTerm* term) = 0;

			/**
			 * clone the term reader
			 * @return term reader, MUST be deleted by caller.
			 */
			virtual	CTermReader*	clone() = 0;

			/**
			 * close term reader			 
			 */
			virtual void			close() = 0;

		protected:
			/**
			 * get term information of a term
			 * @return term information
			 */
			virtual CTermInfo*		termInfo(CTerm* term){return NULL;};
		public:
			/** get documents filter */
			CBitVector*				getFilter(){return m_pDocFilter;}

			/**
			 * set documents filter for filtering deleted documents
			 * @param pDocFilter outside filter,we only keep the reference
			 */
			void					setFilter(CBitVector* pDocFilter){m_pDocFilter = pDocFilter;}

			/**
			 * get field info 
			 * @return field info
			 */
			CFieldInfo*				getFieldInfo(){return m_pFieldInfo;}

			/**
			 * set field info
			 * @param field info
			 */
			void					setFieldInfo(CFieldInfo* pFieldInfo){m_pFieldInfo = pFieldInfo;}
		protected:
			CBitVector*				m_pDocFilter;	///filter deleted docs
			CFieldInfo*				m_pFieldInfo;	///reference to field info

			friend class CTermDocFreqs;	
			friend class CBarrelTermDocFreqs;
			friend class CMultiFieldTermReader;
			friend class CIndexReader;
			friend class CInMemoryTermReader;
		};
	}
}


#endif
