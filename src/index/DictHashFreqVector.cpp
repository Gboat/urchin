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
// Created	: 2006/7/15
//
#include "index/DictHashFreqVector.h"

namespace firtex
{
	namespace index
	{
		CDictHashFreqVector::CDictHashFreqVector()
			: m_pTVVInput(NULL)
			,m_numTerms(-1)
			,m_numDistinctTerms(-1)
			,m_termIDs(NULL)
			,m_termFreqs(NULL)
			,m_pTermDictionary(NULL)
		{
		}
		CDictHashFreqVector::CDictHashFreqVector(const tchar* field,CIndexInput* pTVVInput)
			:CTermFreqVector(field)			
			,m_numTerms(-1)
			,m_numDistinctTerms(-1)
			,m_termIDs(NULL)
			,m_termFreqs(NULL)
			,m_pTermDictionary(NULL)
		{
			open(field,pTVVInput);
		}

		CDictHashFreqVector::~CDictHashFreqVector(void)
		{
			if(m_pTVVInput)
			{
				delete m_pTVVInput;
				m_pTVVInput = NULL;
			}
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
		}
		
		void CDictHashFreqVector::open(const tchar* field,CIndexInput* pInput)
		{			
			m_termIDs = NULL;
			m_termFreqs = NULL;
			m_pTVVInput = pInput->clone();
			m_pTVVInput->seek(pInput->getFilePointer());
			m_numTerms = m_pTVVInput->readVInt();
			m_numDistinctTerms = m_pTVVInput->readVInt();
		}

		count_t CDictHashFreqVector::numTerms()
		{			
			return m_numTerms;
		}
		
		count_t	CDictHashFreqVector::numDistinctTerms()
		{			
			return m_numDistinctTerms;
		}
		
		const CTerm* CDictHashFreqVector::getTerms()
		{
			return NULL;
		}

		const termid_t*	CDictHashFreqVector::getTermIDs()
		{
			if(m_termIDs == NULL)
			{
				readTermVector();
			}
			return m_termIDs;
		}
		
		const freq_t* CDictHashFreqVector::getTermFrequencies()
		{
			if(m_termFreqs == NULL)
			{
				readTermVector();
			}
			return m_termFreqs;
		}
		
		void CDictHashFreqVector::getTermFreqVector(termid_t*& tids,freq_t*& freqs,count_t& size)
		{
			if(m_termIDs == NULL)
			{
				readTermVector();
			}
			tids = m_termIDs;
			freqs = m_termFreqs;
			size = m_numDistinctTerms;
		}
	
		int CDictHashFreqVector::indexOf(termid_t tid)
		{
			return 0;
		}
		void CDictHashFreqVector::addField(CIndexOutput* pOutput,CField* pField)
		{			
			map_item* items = NULL;
			try
			{
				if(!m_pTermDictionary)
					m_pTermDictionary = new CDynamicArray<freq_t>();
				CTokens* pTokens = pField->tokensValue();
				if(!pTokens)
					return ;
				count_t numTerms = 0;
				size_t getNumTokens = pTokens->getNumTokens();
				termid_t* tids = (termid_t*)pTokens->getTokenBuffer();
				for (size_t i = 0;i<getNumTokens;i++)
				{
					if(tids[i] > 0 )
					{						
						(*m_pTermDictionary)[tids[i]]++;
						numTerms++;
					}				
				}

				int32_t len = (int32_t)m_pTermDictionary->length();
				if(len <= 0)
					return;	
				
				pOutput->writeVInt(numTerms);				//NumTerms(VInt32)
				pOutput->writeVInt(len);					//NumDistinct (VInt32)

				freq_t freq;
				CDynamicArray<freq_t>::array_iterator aiter = m_pTermDictionary->elements();				
				while(aiter.next())
				{
					freq = aiter.element();
					pOutput->writeVInt((int32_t)aiter.position());		//TermID(VInt32) 
					pOutput->writeVInt(freq);							//TermFreqs(VInt32) 				
				}				
				m_pTermDictionary->reset();
			}
			catch (std::bad_alloc& ) 
			{
				FIRTEX_THROW3(OUTOFMEM_ERROR,_T("CDictHashFreqVector::addField()"));
			}
			catch (CFirteXException& fe)
			{
				if(items)
				{
					delete[] items;
				}
				FIRTEX_RETHROW(fe);
			}
			/*catch (...)
			{
				FIRTEX_THROW3(UNKNOWN_ERROR,_T("CDictHashFreqVector::addField()"));
			}*/
		}
		void CDictHashFreqVector::readTermVector()
		{			
			if(m_numDistinctTerms <=0)
				return;
			m_termIDs = new termid_t[m_numDistinctTerms];
			m_termFreqs = new freq_t[m_numDistinctTerms];
			for(int32_t i = 0;i<m_numDistinctTerms;i++)
			{
				m_termIDs[i] = m_pTVVInput->readVInt();
				m_termFreqs[i] = m_pTVVInput->readVInt();
			}
		}
	}
}
