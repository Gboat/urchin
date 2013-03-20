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
#include "plugin/ComTokens.h"
#include "com/comerror.h"

namespace firtex
{
	namespace plugin
	{
		CComTokens::CComTokens(CTokens* pTokens) : m_pTokens(pTokens)
		{
		}

		CComTokens::~CComTokens(void)
		{
			m_pTokens = NULL;
		}
		ITokens* CComTokens::wrap(CTokens* pTokens)
		{
			CComTokens* pWrap = new CComTokens(pTokens);
			CLASS_TO_INTERFACE(CComTokens,Tokens,pWrap,pXThis);
			firtex::plugin::ITokens* pCom = NULL;
			pXThis->QueryInterface(firtex::plugin::ITokens::iid,(void**)&pCom);
			return pCom;
		}
		CTokens* CComTokens::unwrap(ITokens* pInterface)
		{
			if ( pInterface == NULL ) return NULL;
			INTERFACE_TO_CLASS(CComTokens, Tokens, pInterface, pWrapper)
				return (CTokens*)pWrapper->m_pTokens;			
		}	
		//////////////////////////////////////////////////////////////////////////
		//CComReader ITokens
		IMPLEMENT_UNKNOWN(CComTokens,Tokens)			
		
		FX_STDMETHODIMP CComTokens::XTokens::getNumTokens(/* [out] [retval]*/int32_t* nNumTokens)
		{
			FX_METHOD_PROLOGUE( CComTokens, Tokens)

			*nNumTokens = (int32_t)pThis->m_pTokens->getNumTokens();
			return FX_S_OK;
		}
		FX_STDMETHODIMP CComTokens::XTokens::getMaxTokens(/* [out] [retval]*/int32_t* maxTokens)
		{
			FX_METHOD_PROLOGUE( CComTokens, Tokens)

			*maxTokens = (int32_t)pThis->m_pTokens->getMaxTokens();
			return FX_S_OK;
		}
		FX_STDMETHODIMP CComTokens::XTokens::setMaxTokens(/* [in] */int32_t maxTokens)
		{
			FX_METHOD_PROLOGUE( CComTokens, Tokens)

			pThis->m_pTokens->setMaxTokens(maxTokens);
			return FX_S_OK;
		}
		FX_STDMETHODIMP CComTokens::XTokens::clear()
		{
			FX_METHOD_PROLOGUE( CComTokens, Tokens)

			pThis->m_pTokens->clear();
			return FX_S_OK;
		}
		FX_STDMETHODIMP CComTokens::XTokens::getBuffer(/* [out] [retval] */byte** buf)
		{
			FX_METHOD_PROLOGUE( CComTokens, Tokens)

			*buf = (byte*)pThis->m_pTokens->getTokenBuffer();
			return FX_S_OK;
		}
		FX_STDMETHODIMP CComTokens::XTokens::addToken(/* [in] */char* pszToken,int32_t nTokenLen,/* [out] [retval] */firtex::com::bool_t* bSuc)
		{
			FX_METHOD_PROLOGUE( CComTokens, Tokens)

			*bSuc = pThis->m_pTokens->addToken((const char*)pszToken,(size_t)nTokenLen);
			return FX_S_OK;
		}		
	}
}
