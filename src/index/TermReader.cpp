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

#include "index/TermReader.h"

namespace firtex
{
	namespace index
	{		
		CTermReader::CTermReader(void)
			: m_pDocFilter(NULL)
			, m_pFieldInfo(NULL)
		{
		}
		CTermReader::CTermReader(CBitVector* pDocFilter)
			: m_pDocFilter(pDocFilter)
			, m_pFieldInfo(NULL)
		{
		}

		CTermReader::CTermReader(CFieldInfo* pFieldInfo,CBitVector* pDocFilter)
			: m_pDocFilter(pDocFilter)
			, m_pFieldInfo(pFieldInfo)
		{
		}

		CTermReader::~CTermReader(void)
		{
			m_pDocFilter = NULL;
		}
		void CTermReader::open(CDirectory* pDirectory,const tchar* barrelname,CFieldInfo* pFieldInfo,CBitVector* pDocFilter)
		{
			setFilter(pDocFilter);
			setFieldInfo(pFieldInfo);
		}
	}
}
