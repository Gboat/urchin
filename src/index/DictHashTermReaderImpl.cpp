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
// Created	: 2006/2/23
//
#include "index/DictHashTermReaderImpl.h"
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
		CDictHashTermReaderImpl::CDictHashTermReaderImpl(void) 
			: m_pTermTable(NULL)
			, m_pInputDescriptor(NULL)
			, m_nTermCount(0)
			, m_nVocLength(0)
		{			
		}
		CDictHashTermReaderImpl::CDictHashTermReaderImpl(CDirectory* pDirectory,const tchar* barrelname,CFieldInfo* pFieldInfo,CBitVector* pDocFilter)
			: CTermReader(pDocFilter)
			, m_pTermTable(NULL)
			, m_nTermCount(0)
			, m_pInputDescriptor(NULL)
			, m_nVocLength(0)
		{		
			open(pDirectory,barrelname,pFieldInfo,pDocFilter);
		}

		CDictHashTermReaderImpl::~CDictHashTermReaderImpl(void)
		{			
			close();			
		}

		void CDictHashTermReaderImpl::open(CDirectory* pDirectory,const tchar* barrelname,CFieldInfo* pFieldInfo,CBitVector* pDocFilter)
		{
			CTermReader::open(pDirectory,barrelname,pFieldInfo,pDocFilter);

			close();
			setFilter(pDocFilter);

			tstring bn = barrelname;

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
			termid_t		tid = 0;
			///read term table
			for (int32_t i = 0;i < m_nTermCount;i++)
			{
				tid += pVocInput->readVInt();
				m_pTermTable[i].tid = tid;
				df = pVocInput->readVInt();
				dfiP += pVocInput->readVLong();								
				m_pTermTable[i].ti.set(df,dfiP);
			}			
			delete pVocInput;

			m_pInputDescriptor = new CInputDescriptor(true);
			m_pInputDescriptor->setDPostingInput(pDirectory->openInput(bn + _T(".dfp")));
			if(pFieldInfo->getIndexLevel() == WORD_LEVEL)
				m_pInputDescriptor->setPPostingInput(pDirectory->openInput(bn + _T(".pop")));
		}

		CTermIterator* CDictHashTermReaderImpl::termIterator(CTerm* pLowerTerm,CTerm* pUpperTerm)
		{			
			return NULL;
		}

		CTermIterator* CDictHashTermReaderImpl::termIterator(const tchar* field)
		{
			if((field != NULL) && (!_tcsicmp(getFieldInfo()->getName(),field)))
				return NULL;			
			return new CDictHashTermIterator(this);			
		}

		bool CDictHashTermReaderImpl::seek(CTerm* term)
		{			
			return false;
		}

		CTermDocFreqs* CDictHashTermReaderImpl::termDocFreqs()
		{
			return NULL;
		}

		CTermPositions*	CDictHashTermReaderImpl::termPositions()
		{
			return NULL;
		}
		freq_t CDictHashTermReaderImpl::docFreq(CTerm* term)
		{
			CTermInfo* ti = termInfo(term);
			if(ti)
			{
				return ti->docFreq();
			}
			return 0;
		}

		CTermInfo* CDictHashTermReaderImpl::termInfo(CTerm* term)
		{			
			if(_tcscmp(term->getField(),getFieldInfo()->getName()))
				return NULL;
			CTermX<termid_t>* pTermX = (CTermX<termid_t>*)term->asTermX();			

			termid_t tid = pTermX->getValue();
			int32_t start = 0,end = m_nTermCount - 1;
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
		void CDictHashTermReaderImpl::close()
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
