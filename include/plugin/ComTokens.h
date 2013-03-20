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
// Created	: 2006/7/9
//
#ifndef __COMTOKENS_H
#define __COMTOKENS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FirteXCOM.h"
#include "ComObject.h"
#include "../analyzer/Tokens.h"
using namespace firtex::analyzer;

namespace firtex
{
	namespace plugin
	{
		class CComTokens :	public CComObject
		{
		public:
			CComTokens(CTokens* pTokens);
			virtual ~CComTokens(void);
		public:
			static ITokens*		wrap(CTokens* pTokens);
			static CTokens*		unwrap(ITokens* pInterface);
		public:
			BEGIN_INTERFACE_PART(Tokens, ITokens)										
				FX_STDMETHOD(getNumTokens)(/* [out] [retval]*/int32_t* nNumTokens);
				FX_STDMETHOD(getMaxTokens)(/* [out] [retval]*/int32_t* maxTokens);
				FX_STDMETHOD(setMaxTokens)(/* [in] */int32_t maxTokens);
				FX_STDMETHOD(clear)();
				FX_STDMETHOD(getBuffer)(/* [out] [retval] */byte** buf);
				FX_STDMETHOD(addToken)(/* [in] */char* pszToken,int32_t nTokenLen,/* [out] [retval] */firtex::com::bool_t* bSuc);				
			END_INTERFACE_PART(Tokens)
		private:
			CTokens* m_pTokens;
		};
	}
}

#endif
