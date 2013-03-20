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
#include "HTMLParserPlugin.h"
#include "HtmlTextStripper.h"
#include "com/ComPtr.h"

#ifdef WIN32
#define wcsncasecmp wcsnicmp
#define strncasecmp strnicmp
#endif


namespace firtex
{
	namespace plugin
	{	

		namespace HTMLParserPlugin
		{				
			firtex::com::FX_CLSID const clsid = { 0x6810f205, 0xf830, 0x463c, { 0xb5, 0x14, 0xf7, 0xf9, 0x1d, 0x61, 0x3b, 0xc } };
			wchar_t const VersionIndependentProgId[] = L"FirteX.HTMLParserPlugin";
			wchar_t const ProgId[] = L"FirteX.HTMLParserPlugin.1";
		}; // namespace HTMLParserPlugin 

		//////////////////////////////////////////////////////////////////////////
		//CHTMLParserPlugin
		CHTMLParserPlugin::CHTMLParserPlugin()
			:m_count(0)
		{
			m_pTextStripper = NULL;
			m_termVectors[0] = firtex::plugin::FORWARDINDEX_NO;
			m_termVectors[1] = firtex::plugin::FORWARDINDEX_NO;
			m_schemaIds[HTMLPARSERPLUGIN_TITLE] = -1;
			m_schemaIds[HTMLPARSERPLUGIN_KEYWORDS] = -1;
			m_schemaIds[HTMLPARSERPLUGIN_DESCRIPTION] = -1;
			m_schemaIds[HTMLPARSERPLUGIN_CONTENT] = -1;
		}
		CHTMLParserPlugin::~CHTMLParserPlugin()
		{
			if(m_pTextStripper)
			{
				delete m_pTextStripper;
				m_pTextStripper = NULL;
			}	

		}
		FX_STDMETHODIMP_(uint32_t) CHTMLParserPlugin::AddRef()
		{ 
			if ( !m_count ) 
				firtex::com::DllLockServerImpl(true); 
			return ++m_count; 
		};

		FX_STDMETHODIMP_(uint32_t) CHTMLParserPlugin::Release()
		{ 
			uint32_t c = --m_count; 
			if ( !c ) 
			{
				firtex::com::DllLockServerImpl(false); 
				delete this;
			}
			return c; 
		};

		FX_STDMETHODIMP CHTMLParserPlugin::QueryInterface( firtex::com::FX_IID const& riid, void** ptr )
		{
			if ( firtex::com::FX_IsEqualIID( riid, firtex::com::IUnknown::iid ) ) 
			{
				*ptr = static_cast<firtex::com::IUnknown*>( this );
				AddRef();
				return FX_S_OK;
			}

			if ( firtex::com::FX_IsEqualIID( riid, firtex::plugin::IParserPlugin::iid ) ) 
			{
				*ptr = static_cast<firtex::plugin::IParserPlugin*>( this );
				AddRef();
				return FX_S_OK;
			}

			return FX_E_NOINTERFACE;
		}
		
		FX_STDMETHODIMP CHTMLParserPlugin::getIdentifier(/* [out] [retval] */firtex::com::FX_BSTR* retVal)
		{
			if ( !retVal )
				return FX_E_POINTER;
			firtex::com::CComStr tmp( L"parser.html" );
			if ( !tmp )
				return FX_E_OUTOFMEMORY;			
			*retVal = tmp.detach();
			return FX_S_OK;
		}
		FX_STDMETHODIMP CHTMLParserPlugin::getDocType(/* [out] [retval] */firtex::com::FX_BSTR* retVal)
		{
			if ( !retVal )
				return FX_E_POINTER;
			firtex::com::CComStr tmp( L"html" );
			if ( !tmp )
				return FX_E_OUTOFMEMORY;			
			*retVal = tmp.detach();
			return FX_S_OK;
		}		
		
		FX_STDMETHODIMP CHTMLParserPlugin::setParam(/* [in] */firtex::com::FX_BSTR pszParam)
		{
			if(!pszParam)
				return FX_E_INVALIDARG;

			if(m_pTextStripper == NULL)
				m_pTextStripper = new CHtmlTextStripper();
			firtex::com::FX_BSTR pBeg = pszParam;
			firtex::com::FX_BSTR pEnd = pszParam + wcslen(pszParam);
			while (pBeg && (pBeg < pEnd))
			{
				if(!wcsncasecmp(pBeg,L"title.forwardindex",18))
				{
					if(!wcsncasecmp(pBeg + 18,L"=freq",5))
						m_termVectors[HTMLPARSERPLUGIN_TITLE] = firtex::plugin::FORWARDINDEX_FREQ;
					else if(!wcsncasecmp(pBeg + 18,L"=sequence",9))
						m_termVectors[HTMLPARSERPLUGIN_TITLE] = firtex::plugin::FORWARDINDEX_SEQUENCE;
					else if(!wcsncasecmp(pBeg + 18,L"=position",9))
						m_termVectors[HTMLPARSERPLUGIN_TITLE] = firtex::plugin::FORWARDINDEX_WITH_POSITION;
					else m_termVectors[HTMLPARSERPLUGIN_TITLE] = firtex::plugin::FORWARDINDEX_NO;
				}
				if(!wcsncasecmp(pBeg,L"text.forwardindex",17))
				{
					if(!wcsncasecmp(pBeg + 17,L"=freq",5))
						m_termVectors[HTMLPARSERPLUGIN_CONTENT] = firtex::plugin::FORWARDINDEX_FREQ;
					else if(!wcsncasecmp(pBeg + 17,L"=sequence",9))
						m_termVectors[HTMLPARSERPLUGIN_CONTENT] = firtex::plugin::FORWARDINDEX_SEQUENCE;
					else if(!wcsncasecmp(pBeg + 17,L"=position",9))
						m_termVectors[HTMLPARSERPLUGIN_CONTENT] = firtex::plugin::FORWARDINDEX_WITH_POSITION;
					else m_termVectors[HTMLPARSERPLUGIN_CONTENT] = firtex::plugin::FORWARDINDEX_NO;
				}
				if(!wcsncasecmp(pBeg,L"charsetdetect",7))
				{
					if(!wcsncasecmp(pBeg + 7,L"=true",5))
						m_pTextStripper->setMetaPresent(CHtmlTextStripper::META_CHARSET,true);
					else
						m_pTextStripper->setMetaPresent(CHtmlTextStripper::META_CHARSET,false);
				}
				if(!wcsncasecmp(pBeg,L"title=",6))
				{
					if(!wcsncasecmp(pBeg + 6,L"true",4))
						m_pTextStripper->setMetaPresent(CHtmlTextStripper::META_TITLE,true);
					else
						m_pTextStripper->setMetaPresent(CHtmlTextStripper::META_TITLE,false);
				}
				if(!wcsncasecmp(pBeg,L"keywords=",9))
				{
					if(!wcsncasecmp(pBeg + 9,L"true",4))
						m_pTextStripper->setMetaPresent(CHtmlTextStripper::META_KEYWORDS,true);
					else
						m_pTextStripper->setMetaPresent(CHtmlTextStripper::META_KEYWORDS,false);
				}
				if(!wcsncasecmp(pBeg,L"description=",12))
				{
					if(!wcsncasecmp(pBeg + 12,L"true",4))
						m_pTextStripper->setMetaPresent(CHtmlTextStripper::META_DESCRIPTION,true);
					else
						m_pTextStripper->setMetaPresent(CHtmlTextStripper::META_DESCRIPTION,false);
				}
				pBeg = wcsstr(pBeg,L";");
				if(pBeg)
					pBeg++;
			}
			
			return FX_S_OK;
		}
		FX_STDMETHODIMP CHTMLParserPlugin::getParam(/* [in] */firtex::com::FX_BSTR pszParamName,/* [out] [retval] */firtex::com::FX_BSTR* pszParamValue,/* [out] [retval] */firtex::com::bool_t* bSuc)
		{
			return FX_S_OK;
		}

		FX_STDMETHODIMP CHTMLParserPlugin::defineSchema(/* [in] [out] [retval] */IDocumentSchema* pSchema,/* [out] */firtex::plugin::fieldid_t* pid)
		{	
			if(!pSchema)
				return FX_E_INVALIDARG;		
			if(m_pTextStripper == NULL)
				m_pTextStripper = new CHtmlTextStripper();
			firtex::com::FX_HRESULT hret;
			if(m_pTextStripper->getMetaPresent(CHtmlTextStripper::META_TITLE))
			{
				hret = pSchema->addUnStoredItem(L"TITLE",m_termVectors[HTMLPARSERPLUGIN_TITLE],pid);
				m_schemaIds[HTMLPARSERPLUGIN_TITLE] = *pid;
			}
			if(m_pTextStripper->getMetaPresent(CHtmlTextStripper::META_KEYWORDS))
			{
				hret = pSchema->addUnStoredItem(L"KEYWORDS",m_termVectors[HTMLPARSERPLUGIN_KEYWORDS],pid);
				m_schemaIds[HTMLPARSERPLUGIN_KEYWORDS] = *pid;
			}
			if(m_pTextStripper->getMetaPresent(CHtmlTextStripper::META_DESCRIPTION))
			{
				hret = pSchema->addUnStoredItem(L"DESCRIPTION",m_termVectors[HTMLPARSERPLUGIN_DESCRIPTION],pid);
				m_schemaIds[HTMLPARSERPLUGIN_DESCRIPTION] = *pid;
			}				
			hret = pSchema->addUnStoredItem(L"TEXT",m_termVectors[1],pid);
			m_schemaIds[HTMLPARSERPLUGIN_CONTENT] = *pid;

			return hret;
		}


		FX_STDMETHODIMP CHTMLParserPlugin::parseInternal(/* [in] */IDocumentInput* pDocInput,/* [in] */IDocument* pResultDoc)
		{
			if(m_pTextStripper == NULL)
				m_pTextStripper = new CHtmlTextStripper();
			else
			{
				m_pTextStripper->reset();
			}
			
			firtex::plugin::PATH_INPUT* pPathInput = NULL;
			if(pDocInput->getPathInput(&pPathInput) == FX_S_OK)
			{
				if(pPathInput)
				{
					if(m_pTextStripper->parse(pPathInput->path) == false)
					{
						//FIRTEX_CLOG(level::warn) << "pa rse html file error: " << p.param.file.str << endl;
						return FX_E_FAIL;
					}	
					firtex::plugin::STR str;
					size_t length = 0;
					str.str = (char*)m_pTextStripper->getText(length);
					str.length = length;
					
					pResultDoc->addTextField(m_schemaIds[HTMLPARSERPLUGIN_CONTENT],&str,1);
					if(m_schemaIds[HTMLPARSERPLUGIN_TITLE] != -1)
					{					
						str.str = (char*)m_pTextStripper->getMeta(CHtmlTextStripper::META_TITLE,str.length);
						pResultDoc->addTextField(m_schemaIds[HTMLPARSERPLUGIN_TITLE],&str,1);
					}
					if(m_schemaIds[HTMLPARSERPLUGIN_KEYWORDS] != -1)
					{					
						str.str = (char*)m_pTextStripper->getMeta(CHtmlTextStripper::META_KEYWORDS,str.length);
						pResultDoc->addTextField(m_schemaIds[HTMLPARSERPLUGIN_KEYWORDS],&str,1);
					}
					if(m_schemaIds[HTMLPARSERPLUGIN_DESCRIPTION] != -1)
					{					
						str.str = (char*)m_pTextStripper->getMeta(CHtmlTextStripper::META_DESCRIPTION,str.length);
						pResultDoc->addTextField(m_schemaIds[HTMLPARSERPLUGIN_DESCRIPTION],&str,1);
					}
					if(m_pTextStripper->getMetaPresent(CHtmlTextStripper::META_CHARSET))
					{					
						const char* szEnc = m_pTextStripper->getMeta(CHtmlTextStripper::META_CHARSET,length);						
						pResultDoc->setEncoding(getCharset(szEnc,length));
					}
				}
			}

			firtex::plugin::BUFFER_INPUT** pBufferInputs = NULL;
			int32_t numBuffers = 0;

			if(pDocInput->getBufferInputs(&pBufferInputs,&numBuffers) == FX_S_OK)
			{
				for (int32_t i = 0;i<numBuffers;i++)
				{
					if(m_pTextStripper->parse(pBufferInputs[i]->buffer,pBufferInputs[i]->length) == false)
					{
						//FIRTEX_CLOG(level::warn) << "parse html buffer error: " << endl;
						return FX_E_FAIL;
					}	
					firtex::plugin::STR str;
					size_t length = 0;
					str.str = (char*)m_pTextStripper->getText(length);
					str.length = length;

					pResultDoc->addTextField(m_schemaIds[HTMLPARSERPLUGIN_CONTENT],&str,1);
					if(m_pTextStripper->getMetaPresent(CHtmlTextStripper::META_CHARSET))
					{					
						const char* szEnc = m_pTextStripper->getMeta(CHtmlTextStripper::META_CHARSET,length);						
						pResultDoc->setEncoding(getCharset(szEnc,length));
					}

					return FX_S_OK;				
				}
			}

			firtex::plugin::READER_INPUT** pReaderInputs = NULL;
			int32_t numReaders = 0;

			if(pDocInput->getReaderInputs(&pReaderInputs,&numReaders) == FX_S_OK)
			{
				//TODO:
			}		
			return FX_S_OK;
		}	

		firtex::plugin::CHARSET CHTMLParserPlugin::getCharset(const char* strCharset,size_t len)
		{			
			if (strCharset == NULL) 
			{
				return firtex::plugin::CHARSET_UNKNOWN;
			}

			if (len >= 10 && !strncasecmp(strCharset, "ISO-8859-1", 10)) 
			{
				return firtex::plugin::CHARSET_ISO_8859_1;
			}
			else if (len >= 5 && !strncasecmp(strCharset, "UTF-8", 5)) 
			{
				return firtex::plugin::CHARSET_UTF_8;
			}
			else if (len >= 6 && (!strncasecmp(strCharset, "EUC-KR", 6) || !strncasecmp(strCharset, "EUC_KR", 6))) 
			{
				return firtex::plugin::CHARSET_EUC_KR;
			}
			else if (len >= 5 && !strncasecmp(strCharset, "EUCKR", 5)) 
			{
				return firtex::plugin::CHARSET_EUC_KR;
			}
			else if (len >= 11 && !strncasecmp(strCharset, "ISO-8859-15", 11)) 
			{
				return firtex::plugin::CHARSET_ISO_8859_15;
			}
			else if (len >= 12 && !strncasecmp(strCharset, "windows-1252", 12)) 
			{
				return firtex::plugin::CHARSET_WINDOWS_1252;
			}
			else if (len >= 6 && !strncasecmp(strCharset, "UTF-16", 6)) 
			{
				return firtex::plugin::CHARSET_UTF_16;
			}
			else if (len >= 6 && (!strncasecmp(strCharset, "EUC_JP", 6) || !strncasecmp(strCharset, "EUC-JP", 6))) 
			{
				return firtex::plugin::CHARSET_EUC_JP;
			}
			else if (len >= 5 && !strncasecmp(strCharset, "EUCJP", 5)) 
			{
				return firtex::plugin::CHARSET_EUC_JP;
			}
			else if (len >= 6 && !strncasecmp(strCharset, "KOI8-R", 6)) 
			{
				return firtex::plugin::CHARSET_KOI8_R;
			}
			else if (len >= 6 && !strncasecmp(strCharset, "KOI8-U", 6)) 
			{
				return firtex::plugin::CHARSET_KOI8_U;
			}
			else if (len >= 10 && !strncasecmp(strCharset, "ISO-8859-2", 10)) 
			{
				return firtex::plugin::CHARSET_ISO_8859_2;
			}
			else if (len >= 10 && !strncasecmp(strCharset, "ISO-8859-3", 10)) 
			{
				return firtex::plugin::CHARSET_ISO_8859_3;
			}
			else if (len >= 10 && !strncasecmp(strCharset, "ISO-8859-4", 10)) 
			{
				return firtex::plugin::CHARSET_ISO_8859_4;
			}
			else if (len >= 10 && !strncasecmp(strCharset, "ISO-8859-5", 10)) 
			{
				return firtex::plugin::CHARSET_ISO_8859_5;
			}
			else if (len >= 10 && !strncasecmp(strCharset, "ISO-8859-6", 10)) 
			{
				return firtex::plugin::CHARSET_ISO_8859_6;
			}
			else if (len >= 12 && !strncasecmp(strCharset, "ISO-8859-6-I", 12)) 
			{
				return firtex::plugin::CHARSET_ISO_8859_6_I;
			}
			else if (len >= 12 && !strncasecmp(strCharset, "ISO-8859-6-E", 12)) 
			{
				return firtex::plugin::CHARSET_ISO_8859_6_E;
			}
			else if (len >= 10 && !strncasecmp(strCharset, "ISO-8859-7", 10)) 
			{
				return firtex::plugin::CHARSET_ISO_8859_7;
			}
			else if (len >= 10 && !strncasecmp(strCharset, "ISO-8859-8", 10)) 
			{
				return firtex::plugin::CHARSET_ISO_8859_8;
			}
			else if (len >= 12 && !strncasecmp(strCharset, "ISO-8859-8-I", 12)) 
			{
				return firtex::plugin::CHARSET_ISO_8859_8_I;
			}
			else if (len >= 12 && !strncasecmp(strCharset, "ISO-8859-8-E", 12)) 
			{
				return firtex::plugin::CHARSET_ISO_8859_8_E;
			}
			else if (len >= 10 && !strncasecmp(strCharset, "ISO-8859-9", 10))
			{
				return firtex::plugin::CHARSET_ISO_8859_9;
			}
			else if (len >= 11 && !strncasecmp(strCharset, "ISO-8859-10", 11))
			{
				return firtex::plugin::CHARSET_ISO_8859_10;
			}
			else if (len >= 11 && !strncasecmp(strCharset, "ISO-8859-11", 11)) 
			{
				return firtex::plugin::CHARSET_ISO_8859_11;
			}
			else if (len >= 11 && !strncasecmp(strCharset, "ISO-8859-13", 11))
			{
				return firtex::plugin::CHARSET_ISO_8859_13;
			}
			else if (len >= 11 && !strncasecmp(strCharset, "ISO-8859-14", 11))
			{
				return firtex::plugin::CHARSET_ISO_8859_14;
			}
			else if (len >= 11 && !strncasecmp(strCharset, "ISO-8859-16", 11)) 
			{
				return firtex::plugin::CHARSET_ISO_8859_16;
			}
			else if (len >= 10 && !strncasecmp(strCharset, "ISO-IR-111", 10)) 
			{
				return firtex::plugin::CHARSET_ISO_IR_111;
			}
			else if (len >= 11 && !strncasecmp(strCharset, "ISO-2022-CN", 11))
			{
				return firtex::plugin::CHARSET_ISO_2022_CN;
			}
			else if (len >= 11 && !strncasecmp(strCharset, "ISO-2022-CN", 11)) 
			{
				return firtex::plugin::CHARSET_ISO_2022_CN;
			}
			else if (len >= 11 && !strncasecmp(strCharset, "ISO-2022-KR", 11))
			{
				return firtex::plugin::CHARSET_ISO_2022_KR;
			}
			else if (len >= 11 && !strncasecmp(strCharset, "ISO-2022-JP", 11))
			{
				return firtex::plugin::CHARSET_ISO_2022_JP;
			}
			else if (len >= 8 && !strncasecmp(strCharset, "us-ascii", 8)) 
			{
				return firtex::plugin::CHARSET_US_ASCII;
			}
			else if (len >= 8 && !strncasecmp(strCharset, "UTF-32BE", 8)) 
			{
				return firtex::plugin::CHARSET_UTF_32BE;
			}
			else if (len >= 8 && !strncasecmp(strCharset, "UTF-32LE", 8)) 
			{
				return firtex::plugin::CHARSET_UTF_32LE;
			}
			else if (len >= 8 && !strncasecmp(strCharset, "UTF-16BE", 8)) 
			{
				return firtex::plugin::CHARSET_UTF_16BE;
			}
			else if (len >= 8 && !strncasecmp(strCharset, "UTF-16LE", 8)) 
			{
				return firtex::plugin::CHARSET_UTF_16LE;
			}
			else if (len >= 12 && !strncasecmp(strCharset, "windows-1250", 12))
			{
				return firtex::plugin::CHARSET_WINDOWS_1250;
			}
			else if (len >= 12 && !strncasecmp(strCharset, "windows-1251", 12))
			{
				return firtex::plugin::CHARSET_WINDOWS_1251;
			}
			else if (len >= 12 && !strncasecmp(strCharset, "windows-1253", 12)) 
			{
				return firtex::plugin::CHARSET_WINDOWS_1253;
			}
			else if (len >= 12 && !strncasecmp(strCharset, "windows-1254", 12))
			{
				return firtex::plugin::CHARSET_WINDOWS_1254;
			}
			else if (len >= 12 && !strncasecmp(strCharset, "windows-1255", 12))
			{
				return firtex::plugin::CHARSET_WINDOWS_1255;
			}
			else if (len >= 12 && !strncasecmp(strCharset, "windows-1256", 12)) 
			{
				return firtex::plugin::CHARSET_WINDOWS_1256;
			}
			else if (len >= 12 && !strncasecmp(strCharset, "windows-1257", 12))
			{
				return firtex::plugin::CHARSET_WINDOWS_1257;
			}
			else if (len >= 12 && !strncasecmp(strCharset, "windows-1258", 12)) 
			{
				return firtex::plugin::CHARSET_WINDOWS_1258;
			}
			else if (len >= 6 && !strncasecmp(strCharset, "IBM866", 6)) 
			{
				return firtex::plugin::CHARSET_IBM866;
			}
			else if (len >= 6 && !strncasecmp(strCharset, "IBM850", 6)) 
			{
				return firtex::plugin::CHARSET_IBM850;
			}
			else if (len >= 6 && !strncasecmp(strCharset, "IBM852", 6)) 
			{
				return firtex::plugin::CHARSET_IBM852;
			}
			else if (len >= 6 && !strncasecmp(strCharset, "IBM855", 6))
			{
				return firtex::plugin::CHARSET_IBM855;
			}
			else if (len >= 6 && !strncasecmp(strCharset, "IBM857", 6)) 
			{
				return firtex::plugin::CHARSET_IBM857;
			}
			else if (len >= 6 && !strncasecmp(strCharset, "IBM862", 6))
			{
				return firtex::plugin::CHARSET_IBM862;
			}
			else if (len >= 6 && !strncasecmp(strCharset, "IBM864", 6)) 
			{
				return firtex::plugin::CHARSET_IBM864;
			}
			else if (len >= 7 && !strncasecmp(strCharset, "IBM864i", 7)) 
			{
				return firtex::plugin::CHARSET_IBM864I;
			}
			else if (len >= 5 && !strncasecmp(strCharset, "UTF-7", 5)) 
			{
				return firtex::plugin::CHARSET_UTF_7;
			}
			else if (len >= 9 && !strncasecmp(strCharset, "Shift_JIS", 9))
			{
				return firtex::plugin::CHARSET_SHIFT_JIS;
			}
			else if (len >= 4 && !strncasecmp(strCharset, "Big5", 4)) 
			{
				return firtex::plugin::CHARSET_BIG5;
			}
			else if (len >= 6 && !strncasecmp(strCharset, "GB2312", 6)) 
			{
				return firtex::plugin::CHARSET_GB2312;
			}
			else if (len >= 7 && !strncasecmp(strCharset, "gb18030", 7)) 
			{
				return firtex::plugin::CHARSET_GB18030;
			}
			else if (len >= 6 && !strncasecmp(strCharset, "VISCII", 6)) 
			{
				return firtex::plugin::CHARSET_VISCII;
			}
			else if (len >= 7 && !strncasecmp(strCharset, "TIS-620", 7))
			{
				return firtex::plugin::CHARSET_TIS_620;
			}
			else if (len >= 10 && !strncasecmp(strCharset, "HZ-GB-2312", 10))
			{
				return firtex::plugin::CHARSET_HZ_GB_2312;
			}
			else if (len >= 10 && !strncasecmp(strCharset, "Big5-HKSCS", 10)) 
			{
				return firtex::plugin::CHARSET_BIG5_HKSCS;
			}
			else if (len >= 5 && !strncasecmp(strCharset, "x-gbk", 5)) 
			{
				return firtex::plugin::CHARSET_X_GBK;
			}
			else if (len >= 8 && !strncasecmp(strCharset, "x-euc-tw", 8)) 
			{
				return firtex::plugin::CHARSET_X_EUC_TW;
			}
			else 
			{					
				return firtex::plugin::CHARSET_UNKNOWN;
			}
		}
	}
}
