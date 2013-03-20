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
// Created	: 2007/1/5
//

#include "index/NormBytes.h"

namespace firtex
{
	namespace index
	{
		CNormsBytes::CNormsBytes()
			:m_nNumSubNorms(1)			
		{
			m_ppSubNorms = new CNormBytes*[1];
			m_ppSubNorms[0] = NULL;
		}
		CNormsBytes::CNormsBytes(size_t numSubNorms)
			:m_nNumSubNorms(numSubNorms)						
		{
			FIRTEX_ASSERT((numSubNorms > 1),_T("Invalidate parameter."));
			m_ppSubNorms = new CNormBytes*[numSubNorms];
			memset(m_ppSubNorms,0,m_nNumSubNorms*sizeof(CNormBytes*));
		}
		CNormsBytes::~CNormsBytes()
		{
			if(m_nNumSubNorms == 1)///own the sub norm
			{
				if(m_ppSubNorms[0])
					delete m_ppSubNorms[0];
			}
			delete[] m_ppSubNorms;
			m_ppSubNorms = NULL;
			m_nNumSubNorms = 0;
		}
		void CNormsBytes::setNorms(size_t _off,CNormsBytes* pNorms)
		{
			FIRTEX_ASSERT( ((m_nNumSubNorms >1) && (_off < m_nNumSubNorms)),_T("Invalidate parameter."));
			FIRTEX_ASSERT((pNorms->m_nNumSubNorms == 1),_T("Invalidate parameter."));
			m_ppSubNorms[_off] = pNorms->m_ppSubNorms[0];
		}
		void CNormsBytes::setNormBytes(int32_t _off,CNormBytes* pNormByte)
		{			
			m_ppSubNorms[_off] = pNormByte;
		}
	}
}
