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
#include "plugin/ComDocumentInput.h"

#include "com/comerror.h"
#include "utility/Misc.h"
#include "plugin/ComDocumentSchema.h"

namespace firtex
{
	namespace plugin
	{	
		CComDocumentInput::CComDocumentInput(CDocumentInput* pDocInput)
			: m_pDocInput(pDocInput)
		{

		}

		CComDocumentInput::~CComDocumentInput(void)
		{
			m_pDocInput = NULL;
		}

		IDocumentInput* CComDocumentInput::wrap(CDocumentInput* pDocInput)
		{
			CComDocumentInput* pWrap = new CComDocumentInput(pDocInput);
			CLASS_TO_INTERFACE(CComDocumentInput,DocumentInput,pWrap,pXThis);
			firtex::plugin::IDocumentInput* pCom = NULL;
			pXThis->QueryInterface(firtex::plugin::IDocumentInput::iid,(void**)&pCom);
			return pCom;
		}
		CDocumentInput* CComDocumentInput::unwrap(IDocumentInput* pInterface)
		{
			if ( pInterface == NULL ) return NULL;
			INTERFACE_TO_CLASS(CComDocumentInput, DocumentInput, pInterface, pWrapper)
				return (CDocumentInput*)pWrapper->m_pDocInput;			
		}	
		//////////////////////////////////////////////////////////////////////////
		//CComDocument IDocument
		IMPLEMENT_UNKNOWN(CComDocumentInput,DocumentInput)	

		FX_STDMETHODIMP CComDocumentInput::XDocumentInput::getPathInput(/* [out] */PATH_INPUT** ppPathInput)
		{
			FX_METHOD_PROLOGUE( CComDocumentInput, DocumentInput)
				
			CDocumentInput::PathInput* pPI = pThis->m_pDocInput->getPathInput();
			*ppPathInput = reinterpret_cast<PATH_INPUT*> (pPI);///TODO:some problem?			

			return FX_S_OK;
		}	
		FX_STDMETHODIMP CComDocumentInput::XDocumentInput::getBufferInputs(/* [out] */BUFFER_INPUT*** pppBufferInput,/* [out] */int32_t* pNumBuffers)
		{
			FX_METHOD_PROLOGUE( CComDocumentInput, DocumentInput)

			CDocumentInput::BufferInput** ppBI = pThis->m_pDocInput->getBufferInputs(*pNumBuffers);
			*pppBufferInput = reinterpret_cast<BUFFER_INPUT**>(ppBI); ///TODO:some problem
			return FX_S_OK;
		}	
		FX_STDMETHODIMP CComDocumentInput::XDocumentInput::getReaderInputs(/* [out] */READER_INPUT*** pppReaderInput,/* [out] */int32_t* pNumReaders)
		{
			FX_METHOD_PROLOGUE( CComDocumentInput, DocumentInput)

			return FX_S_OK;
		}	
	}
}


