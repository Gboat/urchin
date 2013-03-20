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
// Created	: 2006/10/27
//
#ifndef __COMDOCUMENTINPUT_H
#define __COMDOCUMENTINPUT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FirteXCOM.h"
#include "ComObject.h"
#include "../document/Document.h"

using namespace firtex::document;

namespace firtex
{	
	namespace plugin
	{
		class CComDocumentInput : public CComObject
		{
		public:
			CComDocumentInput(CDocumentInput* pDocInput);
			virtual ~CComDocumentInput(void);		
		public:
			static IDocumentInput*	wrap(CDocumentInput* pDocInput);
			static CDocumentInput*	unwrap(IDocumentInput* pInterface);
		public:
			BEGIN_INTERFACE_PART(DocumentInput, IDocumentInput)				
				FX_STDMETHOD(getPathInput)(/* [out] */PATH_INPUT** ppPathInput);
				FX_STDMETHOD(getBufferInputs)(/* [out] */BUFFER_INPUT*** pppBufferInput,/* [out] */int32_t* pNumBuffers);
				FX_STDMETHOD(getReaderInputs)(/* [out] */READER_INPUT*** pppReaderInput,/* [out] */int32_t* pNumReaders);				
			END_INTERFACE_PART(DocumentInput)

		protected:
			CDocumentInput*	 m_pDocInput;
		};
	}
}

#endif
