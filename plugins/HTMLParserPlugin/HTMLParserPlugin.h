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
// Created	: 2006/6/9
//
#ifndef _HTMLPARSERPLUGIN_H
#define _HTMLPARSERPLUGIN_H

#include "com/comerror.h"
#include "plugin/FirteXCOM.h"

class CHtmlTextStripper;

namespace firtex
{
	namespace plugin
	{
#define HTMLPARSERPLUGIN_TITLE			0
#define HTMLPARSERPLUGIN_KEYWORDS		1
#define HTMLPARSERPLUGIN_DESCRIPTION	2
#define HTMLPARSERPLUGIN_CONTENT		3
		class CHTMLParserPlugin : public IParserPlugin
		{
		public:
			CHTMLParserPlugin();
			~CHTMLParserPlugin();
		public:
			FX_STDMETHOD_(uint32_t,AddRef)();
			FX_STDMETHOD_(uint32_t,Release)();
			FX_STDMETHOD(QueryInterface)( firtex::com::FX_IID const& riid, void** ptr );
			
			FX_STDMETHOD(getIdentifier)(/* [out] [retval] */firtex::com::FX_BSTR* retVal);
			FX_STDMETHOD(getDocType)(/* [out] [retval] */firtex::com::FX_BSTR* retVal);			
			FX_STDMETHOD(setParam)(/* [in] */firtex::com::FX_BSTR pszParam);
			FX_STDMETHOD(getParam)(/* [in] */firtex::com::FX_BSTR pszParamName,/* [out] [retval] */firtex::com::FX_BSTR* pszParamValue,/* [out] [retval] */firtex::com::bool_t* bSuc);
			FX_STDMETHOD(defineSchema)(/* [in] [out] [retval] */IDocumentSchema* pSchema,/* [out] */firtex::plugin::fieldid_t* pid);
			FX_STDMETHOD(parseInternal)(/* [in] */IDocumentInput* pDocInput,/* [in] */IDocument* pResultDoc);
		protected:
			static firtex::plugin::CHARSET getCharset(const char* strCharset,size_t len);
		private:
			unsigned long m_count;

			firtex::plugin::fieldid_t	m_schemaIds[4];
			firtex::plugin::forwardindex_t	m_termVectors[4];

			bool	m_bDetectCharset;
			bool	m_bIndexTitle;

			CHtmlTextStripper*	m_pTextStripper;
		};

		/* uuid:  {6810F205-F830-463c-B514-F7F91D613B0C} */
		/* VersionIndependentProgId: IParserPlugin.HTMLParserPlugin */
		/* ProgId: IParserPlugin.HTMLParserPlugin.1 */
		namespace HTMLParserPlugin 
		{
			extern const firtex::com::FX_CLSID clsid;
			extern const wchar_t VersionIndependentProgId[];
			extern const wchar_t ProgId[];

			inline firtex::com::FX_HRESULT create( firtex::plugin::IParserPlugin** retVal )
			{ return firtex::com::FX_CoCreateInstance( clsid, 0, firtex::com::FX_CLSCTX_INPROC, firtex::plugin::IParserPlugin::iid, reinterpret_cast<void**>(retVal) ); };
		};


		class CHTMLParserFactory: public firtex::com::IClassFactory
		{
		public:
			CHTMLParserFactory() { m_count = 0; };

			FX_STDMETHOD_(uint32_t,AddRef)();
			FX_STDMETHOD_(uint32_t,Release)();
			FX_STDMETHOD(QueryInterface)( firtex::com::FX_IID const& riid, void** ptr );
			FX_STDMETHOD(LockServer)(int32_t fLock);
			FX_STDMETHOD(CreateInstance)( firtex::com::IUnknown* pUnkOuter, firtex::com::FX_REFIID riid, void** ppvObject );
		private:
			unsigned long m_count;
		};
	}
}

#endif
