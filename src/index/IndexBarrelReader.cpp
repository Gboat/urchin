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

#include "index/IndexBarrelReader.h"
#include "index/Index.h"

namespace firtex
{
	namespace index
	{
		CIndexBarrelReader::CIndexBarrelReader(CIndex* pIndex)
			: m_pIndex(pIndex)
		{				
		}
		CIndexBarrelReader::CIndexBarrelReader()
			: m_pIndex(NULL)				
		{
		}
		CIndexBarrelReader::~CIndexBarrelReader(void)
		{
			m_pIndex = NULL;				
		}
		
		CFactoryFinder* CIndexBarrelReader::getFactoryFinder()
		{
			return m_pIndex->getFactoryFinder();
		}
	}
}