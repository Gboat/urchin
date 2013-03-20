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
// Created	: 2007/01/5
//

#include "index/ImmediateMerger.h"
#include "utility/StringUtils.h"

namespace firtex
{
	namespace index
	{
		CImmediateMerger::CImmediateMerger(void)
			:m_nMergeTimes(0)
		{
			m_pMergeBarrel = new CMergeBarrel(_T("imm_0"),2);
		}
		CImmediateMerger::CImmediateMerger(CDirectory* pSrcDirectory)
			:CIndexMerger(pSrcDirectory)			
			,m_nMergeTimes(0)
		{
			m_pMergeBarrel = new CMergeBarrel(_T("imm_0"),2);
		}
		CImmediateMerger::CImmediateMerger(CDirectory* pSrcDirectory,char* buffer,size_t bufsize)
			:CIndexMerger(pSrcDirectory,buffer,bufsize)			
			,m_nMergeTimes(0)
		{
			m_pMergeBarrel = new CMergeBarrel(_T("imm_0"),2);
		}

		CImmediateMerger::~CImmediateMerger(void)
		{
			if(m_pMergeBarrel)
			{
				delete m_pMergeBarrel;
				m_pMergeBarrel = NULL;
			}
		}
		void CImmediateMerger::addBarrel(CMergeBarrelEntry* pEntry)
		{
			m_pMergeBarrel->put(pEntry);
			if(m_pMergeBarrel->size() == 2)
			{
				mergeBarrel(m_pMergeBarrel);
				m_nMergeTimes++;
				tstring str = _T("imm_");
				str = firtex::string_utils::append(str,m_nMergeTimes);
				m_pMergeBarrel->setIdentifier(str);
			}
		}
		void CImmediateMerger::endMerge()
		{			
		}
	}
}
