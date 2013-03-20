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
// Created	: 2006/11/27
//

#include "index/OutputDescriptor.h"

namespace firtex
{
	namespace index
	{
		COutputDescriptor::COutputDescriptor(void)
			:m_pVocOutput(NULL)
			,m_pDPostingOutput(NULL)
			,m_pPPostingOutput(NULL)
			,m_bDestroy(false)
		{
		}
		COutputDescriptor::COutputDescriptor(CIndexOutput* pVocOutput,CIndexOutput* pDPostingOutput,CIndexOutput* pPPostingOutput,bool bDestroy)
			:m_pVocOutput(pVocOutput)
			,m_pDPostingOutput(pDPostingOutput)
			,m_pPPostingOutput(pPPostingOutput)
			,m_bDestroy(bDestroy)
		{
		}

		COutputDescriptor::~COutputDescriptor(void)
		{
			if(m_bDestroy)
			{
				if(m_pVocOutput)
					delete m_pVocOutput;
				if(m_pDPostingOutput)
					delete m_pDPostingOutput;
				if(m_pPPostingOutput)
					delete m_pPPostingOutput;
			}
			m_pVocOutput = NULL;
			m_pDPostingOutput = NULL;
			m_pPPostingOutput = NULL;
		}
	}
}
