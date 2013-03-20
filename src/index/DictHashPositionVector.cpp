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
// Email	: ruijieguo@software.ict.ac.cn ruijieguo@gmail.com
// Created	: 2006/7/15
//
#include "index/DictHashPositionVector.h"

namespace firtex
{
	namespace index
	{
		CDictHashPositionVector::CDictHashPositionVector() 
			: m_pTermDictionary(NULL)
			, m_termIDs(NULL)
			, m_termFreqs( NULL)
			, m_termPositions(NULL)
			, m_pFIVInput(NULL)			
			, m_nNumTerms(0)
			, m_nNumDistinctTerms(0)
		{
		}
		CDictHashPositionVector::CDictHashPositionVector(const tchar* field,CIndexInput* pTVVInput)
			: CTermPositionVector(field)			
			, m_pTermDictionary(NULL)
			, m_termIDs(NULL)
			, m_termFreqs( NULL)
			, m_termPositions(NULL)
			, m_pFIVInput(NULL)			
			, m_nNumTerms(0)
			, m_nNumDistinctTerms(0)
		{
		}

		CDictHashPositionVector::~CDictHashPositionVector(void)
		{
			if(m_pTermDictionary)
			{
				delete m_pTermDictionary;
				m_pTermDictionary = NULL;
			}

			if(m_termIDs)
			{
				delete[] m_termIDs;
				m_termIDs = NULL;
			}
			
			if(m_termFreqs)
			{
				delete[] m_termFreqs;
				m_termFreqs = NULL;
			}
			if(m_termPositions)
			{
				delete[] m_termPositions;
				m_termPositions = NULL;
			}
			if(m_pFIVInput)
			{
				delete m_pFIVInput;
				m_pFIVInput = NULL;
			}			
		}
		void CDictHashPositionVector::open(const tchar* field,CIndexInput* pInput)
		{
			m_termIDs = NULL;
			m_termFreqs = NULL;
			m_termPositions = NULL;
			m_pFIVInput = pInput->clone();
			m_pFIVInput->seek(pInput->getFilePointer());
			m_nNumTerms = m_pFIVInput->readVInt();
			m_nNumDistinctTerms = m_pFIVInput->readVInt();
		}
		count_t CDictHashPositionVector::numTerms()
		{
			return m_nNumTerms;
		}
		
		count_t CDictHashPositionVector::numDistinctTerms()
		{
			return m_nNumDistinctTerms;
		}

		const CTerm* CDictHashPositionVector::getTerms()
		{
			return NULL;
		}

		const termid_t*	CDictHashPositionVector::getTermIDs()
		{
			if(m_termIDs == NULL)
			{
				readForwardIndex();
			}
			return m_termIDs;
		}
		
		const freq_t* CDictHashPositionVector::getTermFrequencies()
		{
			if(m_termFreqs == NULL)
			{
				readForwardIndex();
			}
			return m_termFreqs;
		}
		
		void CDictHashPositionVector::getTermFreqVector(termid_t*& tids,freq_t*& freqs,count_t& size)
		{
			if(m_termIDs == NULL)
			{
				readForwardIndex();
			}
			tids = m_termIDs;
			freqs = m_termFreqs;
			size = m_nNumDistinctTerms;
		}
		
		int CDictHashPositionVector::indexOf(termid_t tid)
		{
			return 0;
		}

		const loc_t* CDictHashPositionVector::getTermPositions(const CTerm* term)
		{
			CTermX<termid_t>* pXTerm = (CTermX<termid_t>*)term->asTermX();
			return getTermPositions(pXTerm->getValue());
		}

		const loc_t* CDictHashPositionVector::getTermPositions()
		{	
			if(!m_termPositions)
				readForwardIndex();		
			return m_termPositions;
		}
		
		const loc_t* CDictHashPositionVector::getTermPositions(termid_t tid)
		{	
			if(!m_termPositions)
				readForwardIndex();
			int32_t nFreqs = 0;
			for (int32_t i = 0;i < m_nNumDistinctTerms;i++)
			{
				nFreqs += m_termFreqs[i];
				if(m_termIDs[i] == tid)
					break;
			}
			return m_termPositions ? &(m_termPositions[nFreqs]):NULL;
		}
		
		const loc_t* CDictHashPositionVector::getTermPositions(size_t index)
		{
			if(!m_termPositions)
				readForwardIndex();
			if((int32_t)index >= m_nNumDistinctTerms)
				return NULL;
			int32_t nFreqs = 0;
			for (size_t i = 0;i < index;i++)
			{
				nFreqs += m_termFreqs[i];				
			}
			return m_termPositions? &(m_termPositions[nFreqs]) : NULL;			
		}
		void CDictHashPositionVector::addField(CIndexOutput*	pOutput,CField* pField)
		{
			CDictHashPositionVector::FreqPos* pPos = NULL;
			try
			{
				if(!m_pTermDictionary)
					m_pTermDictionary = new freqpos_array();
				CTokens* pTokens = pField->tokensValue();
				if(!pTokens)
					return ;
				count_t numTerms = 0;
				size_t numTokens = pTokens->getNumTokens();
				termid_t* tids = (termid_t*)pTokens->getTokenBuffer();
				for (size_t i = 0;i<numTokens;i++)
				{
					if(tids[i] > 0 )
					{						
						pPos = (*m_pTermDictionary)[tids[i]];
						if(!pPos)
						{
							pPos = new CDictHashPositionVector::FreqPos((int32_t)numTokens);
							(*m_pTermDictionary)[tids[i]] = pPos;
						}						
						pPos->addPosition((loc_t)i);
						numTerms++;
					}				
				}

				int32_t len = (int32_t)m_pTermDictionary->length();
				if(len <= 0)
					return;	

				pOutput->writeVInt(numTerms);				///NumTerms(VInt32)
				pOutput->writeVInt(len);					///NumDistinct (VInt32)
								
				freqpos_array::array_iterator aiter = m_pTermDictionary->elements();				
				while(aiter.next())
				{
					pPos = aiter.element();
					pOutput->writeVInt((int32_t)aiter.position());		///TermID(VInt32) 
					pOutput->writeVInt(pPos->freq);						///TermFreqs(VInt32) 					
				}				

				aiter = m_pTermDictionary->elements();
				while(aiter.next())
				{
					loc_t nPre = 0;
					pPos = aiter.element();					
					for (int32_t nP = 0;nP < pPos->freq;nP++)
					{
						pOutput->writeVInt(pPos->positions[nP] - nPre);
						nPre = pPos->positions[nP];
					}
					delete pPos;
				}
				m_pTermDictionary->reset();
			}
			catch (std::bad_alloc& ) 
			{
				FIRTEX_THROW3(OUTOFMEM_ERROR,_T("CDictHashFreqVector::addField()"));
			}
			catch (CFirteXException& fe)
			{				
				FIRTEX_RETHROW(fe);
			}
			/*catch (...)
			{
			FIRTEX_THROW3(UNKNOWN_ERROR,_T("CDictHashFreqVector::addField()"));
			}*/
		}

		void CDictHashPositionVector::readForwardIndex()
		{			
			if(m_nNumDistinctTerms <= 0)
				return;			
			m_termIDs = new termid_t[m_nNumDistinctTerms];
			m_termFreqs = new freq_t[m_nNumDistinctTerms];
			int32_t nTotalFreqs = 0;
			for(int32_t i = 0;i<m_nNumDistinctTerms;i++)
			{
				m_termIDs[i] = m_pFIVInput->readVInt();
				m_termFreqs[i] = m_pFIVInput->readVInt();				
				nTotalFreqs += m_termFreqs[i];
			}

			m_termPositions = new loc_t[nTotalFreqs];
			nTotalFreqs = 0;
			for(int32_t nTerms = 0;nTerms < m_nNumDistinctTerms; nTerms++)
			{		
				loc_t nPre = 0;
				for (int32_t j = 0;j < m_termFreqs[nTerms];j++)
				{	
					nPre += m_pFIVInput->readVInt();
					m_termPositions[nTotalFreqs + j] = nPre;
				}
				nTotalFreqs += m_termFreqs[nTerms];
			}
		}
	}
}

