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
// Created	: 2006/7/12
//
#include "search/Sort.h"
#include "search/SortFieldCache.h"

namespace firtex
{
	namespace search
	{
		//////////////////////////////////////////////////////////////////////////
		//	
		CSortField::CSortField()
		{			
		}
		CSortField::CSortField(const CSortField& src)
			: m_field(src.m_field)
			, m_type(src.m_type)
			, m_bReverse(src.m_bReverse)
		{		
		}
		CSortField::CSortField(const tchar* field)
			:m_field(field==NULL?_T(""):field)
			,m_type(AUTO)
			,m_comparator(NULL)
			,m_bReverse(false)
		{			
		}
		CSortField::CSortField(const tchar* field,bool bReverse)
			:m_field(field==NULL?_T(""):field)
			,m_type(AUTO)
			,m_comparator(NULL)
			,m_bReverse(bReverse)
		{			
		}
		CSortField::CSortField(const tchar* field,SortFieldType type,bool bReverse)
			:m_field(field==NULL?_T(""):field)
			,m_type(type)
			,m_comparator(NULL)
			,m_bReverse(bReverse)
		{		
		}
		CSortField::CSortField(const tchar* field,CSortFieldComparator* pComparator,bool bReverse)
			:m_field(field==NULL?"":field)
			,m_type(CUSTOM)
			,m_comparator(pComparator)
			,m_bReverse(bReverse)
		{
		}
		CSortField::~CSortField()
		{			
			if(m_comparator)
			{
				delete m_comparator;
				m_comparator = NULL;
			}
		}

		CSortField* CSortField::getScoreField()
		{
			static CSortField FIELD_SCORE(NULL,DOCSCORE);
			return &FIELD_SCORE;
		}
		CSortField* CSortField::getDocField()
		{
			static CSortField FIELD_DOC(NULL,DOC);
			return &FIELD_DOC;
		}
		
		//////////////////////////////////////////////////////////////////////////
		//
		CSortFields::CSortFields(int32_t nNumFields)
			: m_numFields(nNumFields)
			, m_sortFields(new CSortField*[nNumFields])
		{
			memset(m_sortFields,0,nNumFields*sizeof(CSortField*));
		}
		CSortFields::CSortFields(const CSortFields& src)
		{
			m_numFields = src.m_numFields;
			if(m_numFields > 0)
			{
				m_sortFields = new CSortField*[m_numFields];
				memset(m_sortFields,0,m_numFields*sizeof(CSortField*));
				for(int32_t i = 0;i<m_numFields;i++)
				{					
					if(src.m_sortFields[i])
					{
						if((m_sortFields[i] != CSortField::getScoreField()) && (m_sortFields[i] != CSortField::getDocField()) )
							m_sortFields[i] = new CSortField((*src.m_sortFields[i]));
						else 
							m_sortFields[i] = src.m_sortFields[i];
					}
				}
			}

		}
		CSortFields::~CSortFields()
		{
			for(int32_t i = 0;i<m_numFields;i++)
			{
				if(m_sortFields[i] && (m_sortFields[i] != CSortField::getScoreField()) && (m_sortFields[i] != CSortField::getDocField()) )
					delete m_sortFields[i];
			}
			delete[] m_sortFields;
		}
		//////////////////////////////////////////////////////////////////////////
		//		

		CSort::CSort(void)
		{
			m_pSortFields = new CSortFields(2);
			(*m_pSortFields)[0] = CSortField::getScoreField();
			(*m_pSortFields)[1] = CSortField::getDocField();
		}		
		CSort::CSort(const tchar* field,bool reverse)
			: m_pSortFields(NULL)
		{			
			setSortField(field,reverse);
		}
		CSort::CSort(const tchar** field,int32_t nNumFields)
			: m_pSortFields(NULL)
		{			
			setSortField(field,nNumFields);
		}

		CSort::CSort(const tchar** field,bool* bReverses,int32_t nNumFields)
			: m_pSortFields(NULL)
		{
			setSortField(field,bReverses,nNumFields);
		}

		CSort::CSort(CSortField* pSortField) 
			: m_pSortFields(NULL)
		{
			setSortField(pSortField);
		}
		CSort::CSort(CSortFields* pSortFields) 
			: m_pSortFields(NULL)
		{		
			setSortField(pSortFields);
		}

		CSort::~CSort(void)
		{			
			clear();
		}

		void CSort::setSortField(const tchar* field,bool bReverse)
		{
			if(m_pSortFields)
				delete m_pSortFields;
			m_pSortFields = new CSortFields(2);
			(*m_pSortFields)[0] = new CSortField(field,bReverse);
			(*m_pSortFields)[1] = CSortField::getDocField();
		}

		void CSort::setSortField(const tchar** field,int32_t nNumFields)
		{
			if(m_pSortFields)
				delete m_pSortFields;
			m_pSortFields = new CSortFields(nNumFields);
			for (int32_t i = 0; i < nNumFields;i++)
			{
				(*m_pSortFields)[i] = new CSortField(field[i]);
			}
		}

		void CSort::setSortField(const tchar** field,bool* bReverses,int32_t nNumFields)
		{
			if(m_pSortFields)
				delete m_pSortFields;
			m_pSortFields = new CSortFields(nNumFields);
			for (int32_t i = 0; i < nNumFields;i++)
			{
				(*m_pSortFields)[i] = new CSortField(field[i],bReverses[i]);
			}
		}
		
		void CSort::setSortField(CSortField* pSortField)
		{
			if(m_pSortFields)
				delete m_pSortFields;
			m_pSortFields = new CSortFields(2);
			(*m_pSortFields)[0] = pSortField;
			(*m_pSortFields)[1] = CSortField::getDocField();
		}		
		void CSort::setSortField(CSortFields* pSortFields)
		{			
			if(m_pSortFields)
				delete m_pSortFields;
			m_pSortFields = pSortFields;
		}
		void CSort::clear()
		{
			if(m_pSortFields)
			{
				delete m_pSortFields;
				m_pSortFields = NULL;
			}
		}
		CSort* CSort::getRelevanceSort()
		{
			static CSort RELEVANCE;
			return &RELEVANCE;			
		}
		CSort* CSort::getDocOrderSort()
		{						
			static CSort DOCORDER;
			return &DOCORDER;
		}
		void CSort::getComparators(CIndexReader* pReader,CSortFieldCache* pFieldCache)
		{
			int32_t nNumFields = m_pSortFields->m_numFields;
			CSortField* pSortField;
			for(int32_t i = 0;i<nNumFields;i++)
			{
				pSortField = m_pSortFields->m_sortFields[i];
				if(pSortField)
				{
					switch(pSortField->getType())
					{
					case CSortField::DOCSCORE:
						pSortField->m_comparator = new CSortFieldComparators::Relevance();
						break;
					case CSortField::DOC:
						pSortField->m_comparator = new CSortFieldComparators::IndexOrder();
						break;
					case CSortField::AUTO:
						pSortField->m_comparator = pFieldCache->getComparator(pReader,pSortField->getField());
						break;
					}
				}
			}			
		}
	}
}
