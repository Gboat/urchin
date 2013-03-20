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
// Created	: 2006/5/15
//
#ifndef __COMDOCUMENT_H
#define __COMDOCUMENT_H

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
		class CComDocument : public CComObject
		{
		public:
			CComDocument(CDocument* pDoc);
			virtual ~CComDocument(void);
		public:
			static IDocument*	wrap(CDocument* pDoc);
			static CDocument*	unwrap(IDocument* pInterface);
		public:
			BEGIN_INTERFACE_PART(Document, IDocument)								
				FX_STDMETHOD(addTextField)(/* [in] */firtex::plugin::fieldid_t id,/* [in] */firtex::plugin::STR* pStr,firtex::com::bool_t bNoCopy);
				FX_STDMETHOD(addBinaryField)(/* [in] */firtex::plugin::fieldid_t id,/* [in] */firtex::plugin::DATARECORD* pBinary);
				FX_STDMETHOD(addNumberField)(/* [in] */firtex::plugin::fieldid_t id,/* [in] */firtex::plugin::NUMBERVAR* pNumber);
				FX_STDMETHOD(setEncoding)(/* [in] */firtex::plugin::CHARSET _charset);
				FX_STDMETHOD(getEncoding)(/* [out] [retval] */firtex::plugin::CHARSET* retVal);
			END_INTERFACE_PART(Document)

		protected:
			CDocument*	 m_pDoc;
			
		};
	}
}

#endif
