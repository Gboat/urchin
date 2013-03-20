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

#ifndef _INPUTDESCRIPTOR_H
#define _INPUTDESCRIPTOR_H

#include "../store/IndexInput.h"

using namespace firtex::store;

namespace firtex
{
	namespace index
	{
		class CInputDescriptor
		{
		public:
			CInputDescriptor(void);
			CInputDescriptor(bool bDestroy);
			CInputDescriptor(CIndexInput* pVocInput,CIndexInput* pDPostingInput,CIndexInput* pPPostingInput,bool bDestroy);
			~CInputDescriptor(void);
		public:
			CIndexInput*	getVocInput(){return m_pVocInput;}
			CIndexInput*	getDPostingInput(){return m_pDPostingInput;};
			CIndexInput*	getPPostingInput(){return m_pPPostingInput;};

			void			setVocInput(CIndexInput* pVocInput){m_pVocInput = pVocInput;}
			void			setDPostingInput(CIndexInput* pDPostingInput){m_pDPostingInput = pDPostingInput;};
			void			setPPostingInput(CIndexInput* pPPostingInput){m_pPPostingInput = pPPostingInput;};
		protected:
			CIndexInput*	m_pVocInput;
			CIndexInput*	m_pDPostingInput;
			CIndexInput*	m_pPPostingInput;
			bool			m_bDestroy;
		};
	}
}

#endif
