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
#include "index/DictHashTermIterator.h"
#include "index/TermInfo.h"
#include "index/InputDescriptor.h"
#include "index/Posting.h"
#include "index/DictHashTermReader.h"
#include "index/DictHashTermReaderImpl.h"
#include <sstream>
using namespace std;
namespace firtex
{
	namespace index
	{
		CDictHashTermIterator::CDictHashTermIterator(CDictHashTermReaderImpl* pTermReader)
			:m_pTermReaderImpl(pTermReader)
			,m_pCurTerm(NULL)
			,m_pCurTermInfo(NULL)
			,m_pCurTermPosting(NULL)
			,m_pInputDescriptor(NULL)
			,m_nCurPos(-1)
		{
		}

		CDictHashTermIterator::~CDictHashTermIterator(void)
		{			
			if(m_pCurTerm)
			{
				delete m_pCurTerm;
				m_pCurTerm = NULL;
			}
			if(m_pCurTermPosting)
			{
				delete m_pCurTermPosting;
				m_pCurTermPosting = NULL;
			}
			if(m_pInputDescriptor)
			{
				delete m_pInputDescriptor;
				m_pInputDescriptor = NULL;
			}
			m_pCurTermInfo = NULL;
		}

		bool CDictHashTermIterator::next()
		{
			if(m_pTermReaderImpl->m_nTermCount > m_nCurPos+1)			
			{
				if(m_pCurTerm == NULL)
					m_pCurTerm = new CTermX<termid_t>(m_pTermReaderImpl->getFieldInfo()->getName(),m_pTermReaderImpl->m_pTermTable[++m_nCurPos].tid);
				else ((CTermX<termid_t>*)m_pCurTerm)->setValue(m_pTermReaderImpl->m_pTermTable[++m_nCurPos].tid);
				m_pCurTermInfo = &(m_pTermReaderImpl->m_pTermTable[m_nCurPos].ti);
				return true;
			}
			else return false;
		}

		bool CDictHashTermIterator::skipTo(const CTerm* target)
		{
			/*bool ret = m_termIterator.skipTo(((CTermX<termid_t>*)target)->getValue());
			if(ret)
			{
				if(m_pCurTerm == NULL)
					m_pCurTerm = new CTermX<termid_t>(m_field.c_str(),m_termIterator.term());
				else ((CTermX<termid_t>*)m_pCurTerm)->setValue(m_termIterator.term());
			}*/
			///TODO:
			return false;			
		}		
		const CTerm* CDictHashTermIterator::term()
		{
			return m_pCurTerm;			
		}		
		const CTermInfo* CDictHashTermIterator::termInfo()
		{			
			return m_pCurTermInfo;
		}
		CPosting* CDictHashTermIterator::termPosting()
		{
			if(!m_pCurTermPosting)
			{
				size_t bufSizeForDPosting = 0;
				CIndexInput* pInput = m_pTermReaderImpl->m_pInputDescriptor->getPPostingInput();
				if(pInput)
				{
					int64_t nDLen;
					m_pTermReaderImpl->getFieldInfo()->getLength(NULL,&nDLen,NULL);
					if(nDLen < (size_t)(m_nBuffSize/2))
						bufSizeForDPosting = (size_t)nDLen;
					else 
						bufSizeForDPosting = m_nBuffSize/2;
				}
				else 
				{
					bufSizeForDPosting = m_nBuffSize;
				}
				m_pInputDescriptor = new CInputDescriptor(true);
				if(bufSizeForDPosting > INDEXINPUT_BUFFSIZE)
					pInput = m_pTermReaderImpl->m_pInputDescriptor->getDPostingInput()->clone(m_pBuffer,bufSizeForDPosting);
				else 
					pInput = m_pTermReaderImpl->m_pInputDescriptor->getDPostingInput()->clone();				
				m_pInputDescriptor->setDPostingInput(pInput);
				pInput = m_pTermReaderImpl->m_pInputDescriptor->getPPostingInput();
				if(pInput)
				{
					if((m_nBuffSize - bufSizeForDPosting) < INDEXINPUT_BUFFSIZE)
						pInput = pInput->clone();
					else 
						pInput = pInput->clone(m_pBuffer + bufSizeForDPosting,m_nBuffSize - bufSizeForDPosting);					
					m_pInputDescriptor->setPPostingInput(pInput);
					m_pCurTermPosting = new COnDiskPosting_WL(m_pInputDescriptor,m_pCurTermInfo->docPointer());
					m_pCurTermPosting->setFilter(m_pTermReaderImpl->getFilter());
				}					
			}
			else 
			{				
				((COnDiskPosting_WL*)m_pCurTermPosting)->reset(m_pCurTermInfo->docPointer());///reset to a new posting
			}
			return m_pCurTermPosting;
		}
		freq_t CDictHashTermIterator::docFreq()
		{			
			return m_pCurTermInfo->docFreq();
		}		

		size_t CDictHashTermIterator::setBuffer(char* pBuffer,size_t bufSize)
		{
			int64_t nDLen,nPLen;
			m_pTermReaderImpl->getFieldInfo()->getLength(NULL,&nDLen,&nPLen);
			if( (nDLen + nPLen) < bufSize)
			{
				CTermIterator::setBuffer(pBuffer,(size_t)(nDLen + nPLen));
				return (size_t)(nDLen + nPLen);
			}
			else 
			{
				CTermIterator::setBuffer(pBuffer,bufSize);
				return bufSize;
			}
		}
		//////////////////////////////////////////////////////////////////////////
		//
		CDictHashInMemoryTermIterator::CDictHashInMemoryTermIterator(CDictHashInMemoryTermReader* pTermReader)
			:m_pTermReader(pTermReader)
			,m_pCurTerm(NULL)
			,m_pCurTermInfo(NULL)
			,m_pCurTermPosting(NULL)			
			,m_aIterator(pTermReader->m_pIndexer->m_array.elements())
		{
		}

		CDictHashInMemoryTermIterator::~CDictHashInMemoryTermIterator(void)
		{			
			if(m_pCurTerm)
			{
				delete m_pCurTerm;
				m_pCurTerm = NULL;
			}
			m_pCurTermPosting = NULL;
			if(m_pCurTermInfo)
			{
				delete m_pCurTermInfo;
				m_pCurTermInfo = NULL;
			}			
		}

		bool CDictHashInMemoryTermIterator::next()
		{
			bool bret = m_aIterator.next();			
			if(bret)
			{
				m_pCurTermPosting = m_aIterator.element();
				while (m_pCurTermPosting->hasNoChunk())
				{
					bret = m_aIterator.next();
					if(bret)
						m_pCurTermPosting = m_aIterator.element();
					else return false;
				}
				if(m_pCurTerm == NULL)
					m_pCurTerm = new CTermX<termid_t>(m_pTermReader->m_sField.c_str(),(termid_t)m_aIterator.position());
				else ((CTermX<termid_t>*)m_pCurTerm)->setValue((termid_t)m_aIterator.position());
				m_pCurTermPosting = m_aIterator.element();
				if(m_pCurTermInfo == NULL)
					m_pCurTermInfo = new CTermInfo(m_pCurTermPosting->docFreq(),-1);
				else 
					m_pCurTermInfo->set(m_pCurTermPosting->docFreq(),-1);
				return true;
			}
			else return false;
		}

		bool CDictHashInMemoryTermIterator::skipTo(const CTerm* target)
		{
			/*bool ret = m_termIterator.skipTo(((CTermX<termid_t>*)target)->getValue());
			if(ret)
			{
			if(m_pCurTerm == NULL)
			m_pCurTerm = new CTermX<termid_t>(m_field.c_str(),m_termIterator.term());
			else ((CTermX<termid_t>*)m_pCurTerm)->setValue(m_termIterator.term());
			}*/
			///TODO:
			return false;			
		}		
		const CTerm* CDictHashInMemoryTermIterator::term()
		{
			return m_pCurTerm;			
		}		
		const CTermInfo* CDictHashInMemoryTermIterator::termInfo()
		{			
			return m_pCurTermInfo;
		}
		CPosting* CDictHashInMemoryTermIterator::termPosting()
		{			
			return m_pCurTermPosting;
		}
		freq_t CDictHashInMemoryTermIterator::docFreq()
		{
			if(!m_pCurTermInfo)
				return 0;
			return m_pCurTermInfo->docFreq();
		}		
		size_t CDictHashInMemoryTermIterator::setBuffer(char* pBuffer,size_t bufSize)
		{
			return 0;///don't need buffer
		}
	}
}
