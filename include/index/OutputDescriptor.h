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

#ifndef _OUTPUTDESCRIPTOR_H
#define _OUTPUTDESCRIPTOR_H

#include "../store/IndexOutput.h"

using namespace firtex::store;

namespace firtex
{
	namespace index
	{
		class COutputDescriptor
		{
		public:
			COutputDescriptor(void);
			COutputDescriptor(CIndexOutput* pVocOutput,CIndexOutput* pDPostingOutput,CIndexOutput* pPPostingOutput,bool bDestroy);
			~COutputDescriptor(void);

		public:
			CIndexOutput*	getVocOutput(){return m_pVocOutput;}
			CIndexOutput*	getDPostingOutput(){return m_pDPostingOutput;};
			CIndexOutput*	getPPostingOutput(){return m_pPPostingOutput;};

			void			setVocOutput(CIndexOutput* pVocOutput){m_pVocOutput = pVocOutput;}
			void			setDPostingOutput(CIndexOutput* pDPostingOutput){m_pDPostingOutput = pDPostingOutput;};
			void			setPPostingOutput(CIndexOutput* pPPostingOutput){m_pPPostingOutput = pPPostingOutput;};
		protected:
			CIndexOutput*	m_pVocOutput;
			CIndexOutput*	m_pDPostingOutput;
			CIndexOutput*	m_pPPostingOutput;
			bool			m_bDestroy;
		};
	}
}

#endif
