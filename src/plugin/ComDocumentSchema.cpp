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
// Created	: 2006/7/2
//

#include "plugin/ComDocumentSchema.h"
#include "utility/Misc.h"
#include "com/comerror.h"
using namespace firtex::utility;

namespace firtex
{
	namespace plugin
	{
		CComDocumentSchema::CComDocumentSchema(CDocumentSchema* pDocSchema):m_pDocSchema(pDocSchema)
		{
		}

		CComDocumentSchema::~CComDocumentSchema(void)
		{
			m_pDocSchema = NULL;
		}
		IDocumentSchema* CComDocumentSchema::wrap(CDocumentSchema* pDocSchema)
		{
			CComDocumentSchema* pWrap = new CComDocumentSchema(pDocSchema);
			CLASS_TO_INTERFACE(CComDocumentSchema,DocumentSchema,pWrap,pXThis);
			firtex::plugin::IDocumentSchema* pCom = NULL;
			pXThis->QueryInterface(firtex::plugin::IDocumentSchema::iid,(void**)&pCom);
			return pCom;
		}
		CDocumentSchema* CComDocumentSchema::unwrap(IDocumentSchema* pInterface)
		{
			if ( pInterface == NULL ) return NULL;
			INTERFACE_TO_CLASS(CComDocumentSchema, DocumentSchema, pInterface, pWrapper)
			return (CDocumentSchema*)pWrapper->m_pDocSchema;			
		}

		//////////////////////////////////////////////////////////////////////////
		//CComDocumentSchema IDocument
		IMPLEMENT_UNKNOWN(CComDocumentSchema,DocumentSchema)	

		FX_STDMETHODIMP CComDocumentSchema::XDocumentSchema::addKeywordItem(/* [in] */firtex::com::FX_BSTR name,
																		 /* [out] */firtex::plugin::fieldid_t* pid)
		{
			FX_METHOD_PROLOGUE( CComDocumentSchema, DocumentSchema)

#ifndef _UNICODE
			size_t mlen = CMisc::_wcstombs(NULL,0,const_cast<wchar_t*>(name),wcslen(name));
			char* mbs = new char[mlen + 1];
			CMisc::_wcstombs(mbs,mlen,const_cast<wchar_t*>(name),wcslen(name));
			mbs[mlen] = 0;
			*pid = pThis->m_pDocSchema->addKeywordItem(mbs);
			delete[] mbs;
#else 
			*pid = pThis->m_pDocSchema->addKeywordItem(const_cast<wchar_t*>(name));
#endif

			return FX_S_OK;
		}		
		FX_STDMETHODIMP CComDocumentSchema::XDocumentSchema::addUnIndexedItem(/* [in] */firtex::com::FX_BSTR name,
																		   /* [out] */firtex::plugin::fieldid_t* pid)
		{
			FX_METHOD_PROLOGUE( CComDocumentSchema, DocumentSchema)

#ifndef _UNICODE
			size_t mlen = CMisc::_wcstombs(NULL,0,const_cast<wchar_t*>(name),wcslen(name));
			char* mbs = new char[mlen + 1];
			CMisc::_wcstombs(mbs,mlen,const_cast<wchar_t*>(name),wcslen(name));
			mbs[mlen] = 0;
			*pid = pThis->m_pDocSchema->addUnIndexedItem(mbs);
			delete[] mbs;
#else 
			*pid = pThis->m_pDocSchema->addUnIndexedItem(const_cast<wchar_t*>(name));
#endif

			return FX_S_OK;
		}

		FX_STDMETHODIMP CComDocumentSchema::XDocumentSchema::addTextItem(/* [in] */firtex::com::FX_BSTR name,
																	  /* [in] */firtex::plugin::store_t store,
																	  /* [in] */firtex::plugin::forwardindex_t termVector,
																	  /* [out] */firtex::plugin::fieldid_t* pid)
		{
			FX_METHOD_PROLOGUE( CComDocumentSchema, DocumentSchema)


#ifndef _UNICODE
			size_t mlen = CMisc::_wcstombs(NULL,0,const_cast<wchar_t*>(name),wcslen(name));
			char* mbs = new char[mlen + 1];
			CMisc::_wcstombs(mbs,mlen,const_cast<wchar_t*>(name),wcslen(name));
			mbs[mlen] = 0;
			*pid = pThis->m_pDocSchema->addTextItem(mbs,(firtex::document::store_t)store,(firtex::document::forwardindex_t)termVector);
			delete[] mbs;
#else 
			*pid = pThis->m_pDocSchema->addTextItem(const_cast<wchar_t*>(name),(firtex::document::store_t)store,(firtex::document::forwardindex_t)termVector);
#endif
			return FX_S_OK;
		}
		FX_STDMETHODIMP CComDocumentSchema::XDocumentSchema::addUnStoredItem(/* [in] */firtex::com::FX_BSTR name,
																		  /* [in] */firtex::plugin::forwardindex_t termVector,
																		  /* [out] */firtex::plugin::fieldid_t* pid)
		{
			FX_METHOD_PROLOGUE( CComDocumentSchema, DocumentSchema)
#ifndef _UNICODE
			size_t mlen = CMisc::_wcstombs(NULL,0,const_cast<wchar_t*>(name),wcslen(name));
			char* mbs = new char[mlen + 1];
			CMisc::_wcstombs(mbs,mlen,const_cast<wchar_t*>(name),wcslen(name));
			mbs[mlen] = 0;
			*pid = pThis->m_pDocSchema->addUnStoredItem(mbs,(firtex::document::forwardindex_t)termVector);
			delete[] mbs;
#else 
			*pid = pThis->m_pDocSchema->addUnStoredItem(const_cast<wchar_t*>(name),(firtex::document::forwardindex_t)termVector);
#endif

			return FX_S_OK;
		}

		FX_STDMETHODIMP CComDocumentSchema::XDocumentSchema::addBinaryItem(/* [in] */firtex::com::FX_BSTR name,
																	    /* [in] */firtex::com::bool_t bCompress,
																		/* [out] */firtex::plugin::fieldid_t* pid)
		{
			FX_METHOD_PROLOGUE( CComDocumentSchema, DocumentSchema)
#ifndef _UNICODE
				size_t mlen = CMisc::_wcstombs(NULL,0,const_cast<wchar_t*>(name),wcslen(name));
			char* mbs = new char[mlen + 1];
			CMisc::_wcstombs(mbs,mlen,const_cast<wchar_t*>(name),wcslen(name));
			mbs[mlen] = 0;
			*pid = pThis->m_pDocSchema->addBinaryItem(mbs,(bCompress!=0));
			delete[] mbs;
#else 
			*pid = pThis->m_pDocSchema->addUnStoredItem(const_cast<wchar_t*>(name),(bCompress!=0));
#endif

			return FX_S_OK;
		}
	}
}
