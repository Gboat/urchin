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
// Created	: 2006/6/10
//
#include "index/DateTimeTermReaderImpl.h"
#include "index/BarrelTermDocFreqs.h"
#include "index/BarrelTermPositions.h"
#include "index/DateTimeTermIterator.h"

#include "index/DictHashTermReader.h"
#include "index/BarrelTermDocFreqs.h"
#include "index/BarrelTermPositions.h"
#include "index/InMemoryTermReader.h"
#include "index/DictHashIndexer.h"
#include "index/DictHashTermIterator.h"
#include "index/InputDescriptor.h"

namespace firtex
{
	namespace index
	{
		CDateTimeTermReaderImpl::CDateTimeTermReaderImpl(void) 
			: m_pTermTable(NULL)
			, m_pInputDescriptor(NULL)
			, m_nTermCount(0)
			, m_nVocLength(0)
		{			
		}
		CDateTimeTermReaderImpl::CDateTimeTermReaderImpl(CDirectory* pDirectory,const tchar* barrelname,CFieldInfo* pFieldInfo,CBitVector* pDocFilter)
			: CTermReader(pDocFilter)			
			, m_pTermTable(NULL)
			, m_nTermCount(0)
			, m_pInputDescriptor(NULL)
			, m_nVocLength(0)
		{		
			open(pDirectory,barrelname,pFieldInfo,pDocFilter);
		}

		CDateTimeTermReaderImpl::~CDateTimeTermReaderImpl(void)
		{			
			close();			
		}

		void CDateTimeTermReaderImpl::open(CDirectory* pDirectory,const tchar* barrelname,CFieldInfo* pFieldInfo,CBitVector* pDocFilter)
		{
			CTermReader::open(pDirectory,barrelname,pFieldInfo,pDocFilter);

			close();
			tstring bn = barrelname;

			setFilter(pDocFilter);

			CIndexInput* pVocInput = pDirectory->openInput(bn + _T(".voc"));
			pVocInput->seek(pFieldInfo->getIndexOffset());

			fileoffset_t voffset = pVocInput->getFilePointer();
			///begin read vocabulary descriptor
			m_nVocLength = pVocInput->readVLong();
			m_nTermCount = (int32_t)pVocInput->readVLong();	///get total term count
			///end read vocabulary descriptor
			pVocInput->seek(voffset - m_nVocLength);///seek to begin of vocabulary data

			m_pTermTable = new TERM_TABLE[m_nTermCount];
			freq_t			df = 0;
			fileoffset_t	dfiP = 0;
			int64_t		tid = 0;
			///read term table
			for (int32_t i = 0;i < m_nTermCount;i++)
			{
				tid += pVocInput->readVLong();
				m_pTermTable[i].tid = tid;
				df = pVocInput->readVInt();
				dfiP += pVocInput->readVLong();								
				m_pTermTable[i].ti.set(df,dfiP);
			}			
			delete pVocInput;

			m_pInputDescriptor = new CInputDescriptor(true);
			m_pInputDescriptor->setDPostingInput(pDirectory->openInput(bn + _T(".dfp")));			
		}

		CTermIterator* CDateTimeTermReaderImpl::termIterator(CTerm* pLowerTerm,CTerm* pUpperTerm)
		{
			if(!pLowerTerm && !pUpperTerm)
				FIRTEX_THROW3(ILLEGALARGUMENT_ERROR,_T("At least one term must be non-null."));
			if(pLowerTerm && pUpperTerm && _tcscmp(pLowerTerm->getField(),pUpperTerm->getField()))
				FIRTEX_THROW3(ILLEGALARGUMENT_ERROR,_T("Both terms must be for the same field"));
			CDateTimeTermIterator* pTermIterator = new CDateTimeTermIterator(this,pLowerTerm,pUpperTerm);
			if(pLowerTerm && pTermIterator->skipTo(pLowerTerm) == false)
			{
				delete pTermIterator;
				pTermIterator = NULL;
			}
			return pTermIterator;				
		}

		CTermIterator* CDateTimeTermReaderImpl::termIterator(const tchar* field)
		{
			if((field != NULL) && (!_tcsicmp(getFieldInfo()->getName(),field)))
				return NULL;			
			return new CDateTimeTermIterator(this);			
		}

		bool CDateTimeTermReaderImpl::seek(CTerm* term)
		{			
			return false;
		}

		CTermDocFreqs* CDateTimeTermReaderImpl::termDocFreqs()
		{
			return NULL;
		}

		CTermPositions*	CDateTimeTermReaderImpl::termPositions()
		{
			return NULL;
		}
		freq_t CDateTimeTermReaderImpl::docFreq(CTerm* term)
		{
			CTermInfo* ti = termInfo(term);
			if(ti)
			{
				return ti->docFreq();
			}
			return 0;
		}

		CTermInfo* CDateTimeTermReaderImpl::termInfo(CTerm* term)
		{			
			if(_tcscmp(term->getField(),getFieldInfo()->getName()))
				return NULL;
			CTermX<int64_t>* pTermX = (CTermX<int64_t>*)term->asTermX();			

			int64_t tid = pTermX->getValue();
			int32_t start = 0,end = m_nTermCount-1;
			int32_t mid = (start + end)/2;
			while (start <= end)
			{
				mid = (start + end)/2;
				if(m_pTermTable[mid].tid == tid)
				{					
					return &(m_pTermTable[mid].ti);
				}
				if(m_pTermTable[mid].tid > tid)
				{
					end = mid - 1;					
				}
				else
				{
					start = mid + 1;									
				}				
			}
			return NULL;
		}
		void CDateTimeTermReaderImpl::close()
		{
			if(m_pInputDescriptor)
			{
				delete m_pInputDescriptor;
				m_pInputDescriptor = NULL;
			}

			if(m_pTermTable)
			{
				delete[] m_pTermTable;
				m_pTermTable = NULL;
				m_nTermCount = 0;
			}
		}	
	}
}


