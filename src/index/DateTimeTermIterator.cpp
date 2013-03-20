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
#include "index/DateTimeTermIterator.h"
#include "index/TermInfo.h"
#include "analyzer/DateTimeAnalyzer.h"
#include "index/DateTimeTermReaderImpl.h"
#include "index/InputDescriptor.h"

namespace firtex
{
	namespace index
	{
		CDateTimeTermIterator::CDateTimeTermIterator(CDateTimeTermReaderImpl* pTermReaderImpl)
			: m_pTermReaderImpl(pTermReaderImpl)
			, m_pCurTerm(NULL)
			, m_pCurTermInfo(NULL)
			, m_pCurTermPosting(NULL)
			, m_pInputDescriptor(NULL)
			, m_nCurPos(-1)
			, m_pLowerTerm(NULL)
			, m_pUpperTerm(NULL)
		{
		}
		CDateTimeTermIterator::CDateTimeTermIterator(CDateTimeTermReaderImpl* pTermReaderImpl,CTerm* pLowerTerm,CTerm* pUpperTerm)
			: m_pTermReaderImpl(pTermReaderImpl)
			, m_pCurTerm(NULL)
			, m_pCurTermInfo(NULL)
			, m_pCurTermPosting(NULL)
			, m_pInputDescriptor(NULL)
			, m_nCurPos(-1)
			, m_pLowerTerm(pLowerTerm?(CTermX<int64_t>*)pLowerTerm->clone():NULL)
			, m_pUpperTerm(pUpperTerm?(CTermX<int64_t>*)pUpperTerm->clone():NULL)
		{
		}		

		CDateTimeTermIterator::~CDateTimeTermIterator(void)
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
			if(m_pLowerTerm)
			{
				delete m_pLowerTerm;
				m_pLowerTerm = NULL;
			}
			if(m_pUpperTerm)
			{
				delete m_pUpperTerm;
				m_pUpperTerm = NULL;
			}
		}

		bool CDateTimeTermIterator::next()
		{
			if(m_pTermReaderImpl->m_nTermCount > m_nCurPos+1)			
			{
				if(m_pUpperTerm)
				{
					if( m_pUpperTerm->getValue() < m_pTermReaderImpl->m_pTermTable[m_nCurPos + 1].tid)
						return false;
				}

				if(m_pCurTerm == NULL)
					m_pCurTerm = new CTermX<int64_t>(m_pTermReaderImpl->getFieldInfo()->getName(),m_pTermReaderImpl->m_pTermTable[++m_nCurPos].tid);
				else ((CTermX<int64_t>*)m_pCurTerm)->setValue(m_pTermReaderImpl->m_pTermTable[++m_nCurPos].tid);
				m_pCurTermInfo = &(m_pTermReaderImpl->m_pTermTable[m_nCurPos].ti);
				return true;
			}
			else return false;
		}

		bool CDateTimeTermIterator::skipTo(const CTerm* target)
		{
			CTermX<int64_t>* pTermX = (CTermX<int64_t>*)target->asTermX();
			int64_t t = pTermX->getValue();
			if(m_pLowerTerm && (t < m_pLowerTerm->getValue()))
			{
				return false;
			}
			if(m_pUpperTerm && (t > m_pUpperTerm->getValue()))
			{
				return false;
			}

			
			int32_t start = m_nCurPos,end = m_pTermReaderImpl->m_nTermCount - 1;
			int32_t mid = (start + end)/2;
			while (start <= end)
			{
				mid = (start + end)/2;
				if(m_pTermReaderImpl->m_pTermTable[mid].tid == t)
				{					
					if(m_pCurTerm == NULL)
						m_pCurTerm = new CTermX<int64_t>(m_pTermReaderImpl->getFieldInfo()->getName(),t);
					else ((CTermX<int64_t>*)m_pCurTerm)->setValue(t);
					m_nCurPos = mid;
					return true;
				}
				if(m_pTermReaderImpl->m_pTermTable[mid].tid > t)
				{
					end = mid - 1;					
				}
				else
				{
					start = mid + 1;									
				}				
			}
			return false;			
		}		
		const CTerm* CDateTimeTermIterator::term()
		{
			return m_pCurTerm;			
		}		
		const CTermInfo* CDateTimeTermIterator::termInfo()
		{			
			return m_pCurTermInfo;
		}
		CPosting* CDateTimeTermIterator::termPosting()
		{
			if(!m_pCurTermPosting)
			{
				size_t bufSizeForDPosting = 0;
				bufSizeForDPosting = m_nBuffSize;
				
				CIndexInput* pInput = NULL;
				m_pInputDescriptor = new CInputDescriptor(true);
				if(bufSizeForDPosting > INDEXINPUT_BUFFSIZE)
					pInput = m_pTermReaderImpl->m_pInputDescriptor->getDPostingInput()->clone(m_pBuffer,bufSizeForDPosting);
				else 
					pInput = m_pTermReaderImpl->m_pInputDescriptor->getDPostingInput()->clone();				
				m_pInputDescriptor->setDPostingInput(pInput);
				m_pCurTermPosting = new COnDiskPosting_DL0(m_pInputDescriptor,m_pCurTermInfo->docPointer());
				m_pCurTermPosting->setFilter(m_pTermReaderImpl->getFilter());
									
			}
			else 
			{				
				((COnDiskPosting_DL0*)m_pCurTermPosting)->reset(m_pCurTermInfo->docPointer());///reset to a new posting
			}
			return m_pCurTermPosting;
		}
		freq_t CDateTimeTermIterator::docFreq()
		{			
			return m_pCurTermInfo->docFreq();
		}		

		size_t CDateTimeTermIterator::setBuffer(char* pBuffer,size_t bufSize)
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
		CDateTimeInMemoryTermIterator::CDateTimeInMemoryTermIterator(CDateTimeInMemoryTermReader* pTermReader)
			: m_pTermReader(pTermReader)
			, m_pCurTerm(NULL)
			, m_pCurTermInfo(NULL)
			, m_pCurTermPosting(NULL)			
			, m_aIterator(pTermReader->m_pMapDateTime->begin())
		{
		}
		CDateTimeInMemoryTermIterator::CDateTimeInMemoryTermIterator(CDateTimeInMemoryTermReader* pTermReader,CTerm* pLowerTerm,CTerm* pUpperTerm)
			: m_pTermReader(pTermReader)
			, m_pCurTerm(NULL)
			, m_pCurTermInfo(NULL)
			, m_pCurTermPosting(NULL)			
			, m_aIterator(pTermReader->m_pMapDateTime->begin())
		{
		}

		CDateTimeInMemoryTermIterator::~CDateTimeInMemoryTermIterator(void)
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

		bool CDateTimeInMemoryTermIterator::next()
		{			
			m_aIterator++;
			if(m_aIterator != m_pTermReader->m_pMapDateTime->end())
			{
				m_pCurTermPosting = m_aIterator->second;
				while (m_pCurTermPosting->hasNoChunk())
				{
					m_aIterator++;					
					if(m_aIterator != m_pTermReader->m_pMapDateTime->end())
						m_pCurTermPosting = m_aIterator->second;
					else return false;
				}
				if(m_pCurTerm == NULL)
					m_pCurTerm = new CTermX<int64_t>(m_pTermReader->m_sField.c_str(),(int64_t)m_aIterator->first);
				else ((CTermX<int64_t>*)m_pCurTerm)->setValue((int64_t)m_aIterator->first);
				m_pCurTermPosting = m_aIterator->second;
				if(m_pCurTermInfo == NULL)
					m_pCurTermInfo = new CTermInfo(m_pCurTermPosting->docFreq(),-1);
				else 
					m_pCurTermInfo->set(m_pCurTermPosting->docFreq(),-1);
				return true;
			}
			else return false;
		}

		bool CDateTimeInMemoryTermIterator::skipTo(const CTerm* target)
		{
			CTermX<int64_t>* pTermX = (CTermX<int64_t>*)target->asTermX();
			int64_t t = pTermX->getValue();
			m_aIterator = m_pTermReader->m_pMapDateTime->find(t);
			if(m_aIterator != m_pTermReader->m_pMapDateTime->end())
			{
				if(m_pCurTerm == NULL)
					m_pCurTerm = new CTermX<int64_t>(m_pTermReader->m_sField.c_str(),(int64_t)m_aIterator->first);
				else ((CTermX<int64_t>*)m_pCurTerm)->setValue((int64_t)m_aIterator->first);
				return true;
			}			
			return false;
		}		
		const CTerm* CDateTimeInMemoryTermIterator::term()
		{
			return m_pCurTerm;			
		}		
		const CTermInfo* CDateTimeInMemoryTermIterator::termInfo()
		{			
			return m_pCurTermInfo;
		}
		CPosting* CDateTimeInMemoryTermIterator::termPosting()
		{			
			return m_pCurTermPosting;
		}
		freq_t CDateTimeInMemoryTermIterator::docFreq()
		{
			if(!m_pCurTermInfo)
				return 0;
			return m_pCurTermInfo->docFreq();
		}		
		size_t CDateTimeInMemoryTermIterator::setBuffer(char* pBuffer,size_t bufSize)
		{
			return 0;///don't need buffer
		}
//		/*CDateTimeTermIterator::CDateTimeTermIterator(const tchar* field,CDateVocabulary* pVocabulary)
//			:m_pVocabulary(pVocabulary)
//			,m_pCurTerm(NULL)
//			,m_curPos(-1)
//			,m_pTermInfo(NULL)
//			,m_field(field)
//			,m_termIterator(pVocabulary->terms())
//			,m_pLowerTerm(NULL)
//			,m_pUpperTerm(NULL)
//		{
//		}
//		CDateTimeTermIterator::CDateTimeTermIterator(CTerm* pLowerTerm,CTerm* pUpperTerm,CDateVocabulary* pVocabulary)
//			:m_pVocabulary(pVocabulary)
//			,m_pCurTerm(NULL)
//			,m_curPos(-1)
//			,m_pTermInfo(NULL)			
//			,m_termIterator(pVocabulary->terms())			
//		{
//			if(!pLowerTerm && !pUpperTerm)
//				FIRTEX_THROW3(ILLEGALARGUMENT_ERROR,_T("At least one term must be non-null."));
//			if(pLowerTerm && pUpperTerm && _tcscmp(pLowerTerm->getField(),pUpperTerm->getField()))
//				FIRTEX_THROW3(ILLEGALARGUMENT_ERROR,_T("Both terms must be for the same field"));
//            if(pLowerTerm)
//			{
//				int64_t t = (pLowerTerm->stringValue()!=NULL)?firtex::analyzer::CDateTimeAnalyzer::parse(pLowerTerm->stringValue()):((CTermX<int64_t>*)pLowerTerm)->getValue();
//				m_pLowerTerm = new CTermX<int64_t>(pLowerTerm->getField(),t);
//			} 
//			else m_pLowerTerm = NULL;
//			if(pUpperTerm)
//			{
//				int64_t t = (pUpperTerm->stringValue()!=NULL)?firtex::analyzer::CDateTimeAnalyzer::parse(pUpperTerm->stringValue()):((CTermX<int64_t>*)pUpperTerm)->getValue();
//				m_pUpperTerm = new CTermX<int64_t>(pUpperTerm->getField(),t);
//			}
//			else m_pUpperTerm = NULL;
//			m_field = pUpperTerm?pUpperTerm->getField():pLowerTerm->getField();
//			if(m_pLowerTerm)
//			{
//				if(!skipTo(m_pLowerTerm))
//				{
//					delete m_pLowerTerm;
//					m_pLowerTerm = NULL;
//				}
//			}
//		}*/
//
//		CDateTimeTermIterator::~CDateTimeTermIterator(void)
//		{
////			m_pVocabulary = NULL;
//			if(m_pCurTerm)
//			{
//				delete m_pCurTerm;
//				m_pCurTerm = NULL;
//			}
//			if(m_pUpperTerm)
//			{
//				delete m_pUpperTerm;
//				m_pUpperTerm = NULL;
//			}
//			if(m_pLowerTerm)
//			{
//				delete m_pLowerTerm;
//				m_pLowerTerm = NULL;
//			}
//		}
//
//		bool CDateTimeTermIterator::next()
//		{
//			/*if(m_pLowerTerm)
//			{
//				delete m_pLowerTerm;
//				m_pLowerTerm = NULL;
//				return true;
//			}
//			bool ret = m_termIterator.next();
//			if(ret)
//			{
//				if(m_pUpperTerm)
//				{
//					if(m_termIterator.term() > ((CTermX<int64_t>*)m_pUpperTerm)->getValue())
//						return false;
//				}
//				if(m_pCurTerm == NULL)
//					m_pCurTerm = new CTermX<int64_t>(m_field.c_str(),m_termIterator.term());
//				else ((CTermX<int64_t>*)m_pCurTerm)->setValue(m_termIterator.term());
//			}
//			return ret;*/
//			return false;
//		}
//
//		bool CDateTimeTermIterator::skipTo(const CTerm* target)
//		{
//			/*int64_t t = (target->stringValue()!=NULL)?firtex::analyzer::CDateTimeAnalyzer::parse(target->stringValue()):((CTermX<int64_t>*)target)->getValue();
//			bool ret = m_termIterator.skipTo(t);
//			if(ret)
//			{
//				if(m_pUpperTerm)
//				{
//					if(m_termIterator.term() > ((CTermX<int64_t>*)m_pUpperTerm)->getValue())
//						return false;
//				}
//				if(m_pCurTerm == NULL)
//					m_pCurTerm = new CTermX<int64_t>(m_field.c_str(),m_termIterator.term());
//				else ((CTermX<int64_t>*)m_pCurTerm)->setValue(m_termIterator.term());
//			}
//			return ret;			*/
//			return false;
//		}
	}
}
