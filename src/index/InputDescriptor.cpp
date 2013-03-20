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
// Created	: 2006/12/15
//

#include "index/InputDescriptor.h"

namespace firtex
{
	namespace index
	{
		CInputDescriptor::CInputDescriptor(void)
			:m_pVocInput(NULL)
			,m_pDPostingInput(NULL)
			,m_pPPostingInput(NULL)
			,m_bDestroy(false)
		{
		}
		CInputDescriptor::CInputDescriptor(bool bDestroy)
			:m_pVocInput(NULL)
			,m_pDPostingInput(NULL)
			,m_pPPostingInput(NULL)
			,m_bDestroy(bDestroy)
		{
		}
		CInputDescriptor::CInputDescriptor(CIndexInput* pVocInput,CIndexInput* pDPostingInput,CIndexInput* pPPostingInput,bool bDestroy)
			:m_pVocInput(pVocInput)
			,m_pDPostingInput(pDPostingInput)
			,m_pPPostingInput(pPPostingInput)
			,m_bDestroy(bDestroy)
		{

		}

		CInputDescriptor::~CInputDescriptor(void)
		{
			if(m_bDestroy)
			{
				if(m_pVocInput)
					delete m_pVocInput;
				if(m_pDPostingInput)
					delete m_pDPostingInput;
				if(m_pPPostingInput)
					delete m_pPPostingInput;
			}
			m_pVocInput = NULL;
			m_pDPostingInput = NULL;
			m_pPPostingInput = NULL;
		}
	}
}
