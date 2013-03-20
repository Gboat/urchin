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
// Created	: 2005/12/20
//
#include "index/Term.h"

namespace firtex
{
	namespace index
	{
		CTerm::CTerm(const tchar* field) : m_field(field),m_strValue(NULL)
		{					
		}
		CTerm::CTerm(const tstring& field) : m_field(field),m_strValue(NULL)
		{
		}
		CTerm::CTerm(const tchar* field,const tchar* value) : m_field(field),m_strValue(new CFXString(value))
		{					
		}
		CTerm::CTerm(const tstring& field,const tstring& value) : m_field(field),m_strValue(new CFXString(value))
		{
		}
		CTerm::CTerm(const CTerm& term)
		{
			m_field = term.m_field;
			if(term.m_strValue)
				m_strValue = new CFXString(*(term.m_strValue));
			else m_strValue = NULL;					
		}
		CTerm::CTerm():m_strValue(NULL)
		{
		}

		CTerm::~CTerm(void)
		{
			if(m_strValue)
			{
				delete m_strValue;
				m_strValue = NULL;
			}
		}

		CTerm* CTerm::asTermX()const
		{
			FIRTEX_THROW3(UNSUPPORTED_ERROR,_T("CTerm::asTermX()"));
			return NULL;
		}
		int32_t CTerm::compare(const CTerm* pOther)
		{
			if(m_field == pOther->m_field)
			{
				if((m_strValue == NULL) && (pOther->m_strValue == NULL))
					return 0;
				if((m_strValue == NULL) && (pOther->m_strValue != NULL))
					return -1;
				else if((m_strValue != NULL) && (pOther->m_strValue == NULL))
					return 1;
				else return m_strValue->compare(*(pOther->m_strValue));
				
			}
			return m_field.compare(pOther->m_field);
		}
		CTerm* CTerm::clone()
		{
			return new CTerm(*this);
		}		
		void CTerm::copy(const CTerm* pSrc)
		{
			m_field = pSrc->m_field;
			if(pSrc->m_strValue)
				m_strValue = new CFXString(*(pSrc->m_strValue));
			else m_strValue = NULL;					
		}
		void CTerm::copyValue(const CTerm* pSrc)
		{
			if(pSrc->m_strValue)
				m_strValue = new CFXString(*(pSrc->m_strValue));
			else m_strValue = NULL;
		}
	}
}
