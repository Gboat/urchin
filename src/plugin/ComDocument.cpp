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
#include "plugin/ComDocument.h"

#include "com/comerror.h"
#include "utility/Misc.h"
#include "plugin/ComDocumentSchema.h"

namespace firtex
{
	namespace plugin
	{
		CComDocument::CComDocument(CDocument* pDoc):m_pDoc(pDoc)
		{
		}

		CComDocument::~CComDocument(void)
		{
			m_pDoc = NULL;
		}
		IDocument* CComDocument::wrap(CDocument* pDoc)
		{
			CComDocument* pWrap = new CComDocument(pDoc);
			CLASS_TO_INTERFACE(CComDocument,Document,pWrap,pXThis);
			firtex::plugin::IDocument* pCom = NULL;
			pXThis->QueryInterface(firtex::plugin::IDocument::iid,(void**)&pCom);
			return pCom;
		}
		CDocument* CComDocument::unwrap(IDocument* pInterface)
		{
			if ( pInterface == NULL ) return NULL;
			INTERFACE_TO_CLASS(CComDocument, Document, pInterface, pWrapper)
			return (CDocument*)pWrapper->m_pDoc;			
		}	
		//////////////////////////////////////////////////////////////////////////
		//CComDocument IDocument
		IMPLEMENT_UNKNOWN(CComDocument,Document)	
		
		FX_STDMETHODIMP CComDocument::XDocument::addTextField(/* [in] */firtex::plugin::fieldid_t id,/* [in] */firtex::plugin::STR* pStr,firtex::com::bool_t bNoCopy)
		{
			FX_METHOD_PROLOGUE( CComDocument, Document)
			
			pThis->m_pDoc->addField(id,pStr->str,pStr->length,(bNoCopy!=0) ) ;
			return FX_S_OK;
		}
		FX_STDMETHODIMP CComDocument::XDocument::addBinaryField(/* [in] */firtex::plugin::fieldid_t id,/* [in] */firtex::plugin::DATARECORD* pBinary)
		{
			FX_METHOD_PROLOGUE( CComDocument, Document)

			datarecord dr;
			dr.data = pBinary->data;
			dr.length = pBinary->length;
			pThis->m_pDoc->addField(id,dr,(pBinary->nocopy != 0));
			return FX_S_OK;
		}
		FX_STDMETHODIMP CComDocument::XDocument::addNumberField(/* [in] */firtex::plugin::fieldid_t id,/* [in] */firtex::plugin::NUMBERVAR* pNumber)
		{
			FX_METHOD_PROLOGUE( CComDocument, Document)

			pThis->m_pDoc->addField(id,(const numbervar&)(*pNumber));
			return FX_S_OK;
		}				
		FX_STDMETHODIMP CComDocument::XDocument::setEncoding(/* [in] */firtex::plugin::CHARSET _charset)
		{
			FX_METHOD_PROLOGUE( CComDocument, Document)
			
			pThis->m_pDoc->setEncoding((charset_t)_charset);

			return FX_S_OK;
		}
		FX_STDMETHODIMP CComDocument::XDocument::getEncoding(/* [out] [retval] */firtex::plugin::CHARSET* retVal)
		{
			FX_METHOD_PROLOGUE( CComDocument, Document)

			*retVal = (firtex::plugin::CHARSET)pThis->m_pDoc->getEncoding();
			return FX_S_OK;
		}
	}
}

