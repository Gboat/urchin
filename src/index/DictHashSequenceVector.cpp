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
// Created	: 2006/7/4
//
#include "index/DictHashSequenceVector.h"
#include "utility/Exception.h"

namespace firtex
{
	namespace index
	{
		CDictHashSequenceVector::CDictHashSequenceVector() : m_numTerms(-1),m_termIds(NULL),m_pTVVInput(NULL)
		{
		}
		CDictHashSequenceVector::CDictHashSequenceVector(const tchar* field,CIndexInput* pTVVInput) : CTermSequenceVector(field),m_numTerms(-1),m_termIds(NULL)
		{
			m_pTVVInput = pTVVInput->clone();
			m_pTVVInput->seek(pTVVInput->getFilePointer());
		}

		CDictHashSequenceVector::~CDictHashSequenceVector(void)
		{
			if(m_pTVVInput)
			{
				delete m_pTVVInput;
				m_pTVVInput = NULL;				
			}
			if(m_termIds)
			{
				delete[] m_termIds;
				m_termIds = NULL;
			}
		}	
		void CDictHashSequenceVector::open(const tchar* field,CIndexInput* pInput)
		{
			m_numTerms = -1;
			m_termIds = NULL;
			m_field = field;
			m_pTVVInput = pInput->clone();
			m_pTVVInput->seek(pInput->getFilePointer());
		}
		
		count_t CDictHashSequenceVector::numTerms()
		{
			if(m_numTerms == -1)
				m_numTerms = m_pTVVInput->readVInt();
			return m_numTerms;
		}
		
		const termid_t*	CDictHashSequenceVector::getTermIDs()
		{
			if(!m_termIds)
			{
				try
				{
					if(m_numTerms == -1)
						numTerms();
					if(m_numTerms <= 0)
						return NULL;
					m_termIds = new termid_t[m_numTerms];
					count_t numTerms = m_numTerms;
					for (count_t i = 0;i<numTerms;i++)
					{
						m_termIds[i] = m_pTVVInput->readVInt();
					}
				}
				catch (std::bad_alloc ba)
				{
					FIRTEX_THROW2(OUTOFMEM_ERROR,"CDictHashSequenceVector::getTermIDs()");
				}		
			}
			return m_termIds;
		}
		void CDictHashSequenceVector::addField(CIndexOutput*	pOutput,CField* pField)
		{	
			CTokens* pTokens = pField->tokensValue();
			if(!pTokens)
				return ;
			size_t getNumTokens = pTokens->getNumTokens();
			pOutput->writeVInt((int32_t)getNumTokens);				//<NumTerms(VInt32)>
			termid_t* tids = (termid_t*)pTokens->getTokenBuffer();
			for (size_t i = 0;i<getNumTokens;i++)
			{
				pOutput->writeVInt(tids[i]);			//<TermID(VInt32)>
			}
		}
	}
}
