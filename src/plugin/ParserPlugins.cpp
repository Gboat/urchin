//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: 郭瑞杰(GuoRuijie)
// Email	: ruijieguo@software.ict.ac.cn,ruijieguo@gmail.com
// Created	: 2006/7/9
//
#include "plugin/ParserPlugins.h"
#include "plugin/Plugins.h"
#include "plugin/ComDocument.h"
#include "plugin/ComDocumentSchema.h"
#include "com/comerror.h"
#include "com/Com.h"
#include "com/ComPtr.h"
#include "document/doctypedef.h"

namespace firtex
{
	namespace plugin
	{
		//////////////////////////////////////////////////////////////////////////
		//CParserPlugin
		
		CParserPlugin::CParserPlugin(IParserPlugin* pParserInterface)
			: m_pParserInterface(pParserInterface)
		{			
			m_pParserInterface->AddRef();//?
		}
		CParserPlugin::~CParserPlugin()
		{					
			m_pParserInterface->Release();//?
			m_pParserInterface = NULL;
		}
		tstring CParserPlugin::getDocType()
		{
			firtex::com::CComStr tmp;
			firtex::com::FX_BSTR fileType;
			m_pParserInterface->getDocType(&fileType);
			tmp.attach(fileType);
			tstring str;
#ifndef _UNICODE
			size_t mblen = CMisc::_wcstombs(NULL,0,fileType,tmp.getLength());
			char* mb = new char[mblen + 1];
			CMisc::_wcstombs(mb,mblen,fileType,tmp.getLength());
			str = mb;
			delete[] mb;
#else
			str = fileType;
#endif
			return str;
		}
		
		void CParserPlugin::setParam(const tchar* pszParam)
		{
			firtex::com::CComStr tmp(pszParam);
			m_pParserInterface->setParam((firtex::com::FX_BSTR)tmp);
		}
		
		bool CParserPlugin::getParam(const tchar* pszParamName,tstring& paramValue)
		{
			return false;
		}

		bool CParserPlugin::parseInternal(document::CDocumentInput* pDocInput,document::CDocument* pResultDoc)
		{			
			return (m_pParserInterface->parseInternal(pDocInput->getInterface(),pResultDoc->getInterface())==FX_S_OK);
		} 
		schemaid_t CParserPlugin::defineSchema(CDocumentSchema* pSchema)
		{
			IDocumentSchema* pInterface = CComDocumentSchema::wrap(pSchema);
			schemaid_t _sid;
			m_pParserInterface->defineSchema(pInterface,&_sid);
			pInterface->Release();
			return _sid;
		}	

		//////////////////////////////////////////////////////////////////////////
		//CParserPlugins

		CParserPlugins::CParserPlugins(void)
			: m_bCOMInit(false)
		{
		}

		CParserPlugins::~CParserPlugins(void)
		{
			clear();
			if(m_bCOMInit)
			{
				firtex::com::FX_CoUninitialize();
			}
		}		
		CParserPlugin* CParserPlugins::getParserByIdentifier(const tchar* identifier)
		{
			CParserPlugin* pParser = NULL;
			map<tstring,IParserPlugin*>::iterator iter = m_pluginMap.find(identifier);
			if(iter != m_pluginMap.end())
			{
				pParser = new CParserPlugin(iter->second);
				return pParser;
			}
			FX_CLSID	clsid;
			IParserPlugin* pInterface = NULL;
			if(GlobalPlugins.lookupCLSID(identifier,clsid))
			{
				if(GlobalPlugins.lookupEnable(clsid,true))
				{
					firtex::com::FX_HRESULT hRet = firtex::com::FX_CoCreateInstance(clsid,NULL,firtex::com::FX_CLSCTX_INPROC_SERVER,IParserPlugin::iid,(void**)&pInterface);
					if(FX_FAILED(hRet) && !m_bCOMInit && GlobalPlugins.lookupInstall(clsid))
					{
						//可能是在另一个线程中被调用,COM在这个线程中还未初始化
						hRet = firtex::com::FX_CoInitializeEx( NULL, ::COINIT_APARTMENTTHREADED );
						if(FX_FAILED(hRet))
							FIRTEX_CLOG(level::warn) << _T("CoInitialize Failed.") << FIRTEX_ENDL
						else
						{							
							hRet = firtex::com::FX_CoCreateInstance(clsid,NULL,firtex::com::FX_CLSCTX_INPROC_SERVER,IParserPlugin::iid,(void**)&pInterface);
							m_bCOMInit = true;
						}
					}
					if(FX_SUCCEEDED(hRet))
					{
						m_pluginMap.insert(make_pair((tstring)identifier,pInterface));
						pParser = new CParserPlugin(pInterface);
						return pParser;
					}
				}
				else 
				{
					FIRTEX_CLOG(level::dbg) << _T("Pluin disnabled:") << GUIDX::encode(&clsid) << FIRTEX_ENDL;
				}
			}
			return NULL;
		}

		bool CParserPlugins::loadPlugin(CPlugin* pPlugin)
		{
			IParserPlugin* pInterface = NULL;
			firtex::com::FX_HRESULT hRet = firtex::com::FX_CoCreateInstance(pPlugin->getCLSID(),NULL,firtex::com::FX_CLSCTX_INPROC_SERVER,IParserPlugin::iid,(void**)&pInterface);
			if(FX_FAILED(hRet) && !m_bCOMInit)
			{
				/// may be COM is not registered
				hRet = firtex::com::FX_CoInitializeEx( NULL, ::COINIT_APARTMENTTHREADED );
				if(FX_FAILED(hRet))
				{
					FIRTEX_CLOG(level::warn) << _T("CoInitialize Failed.") << FIRTEX_ENDL;
					return false;
				}
				else
				{							
					hRet = firtex::com::FX_CoCreateInstance(pPlugin->getCLSID(),NULL,firtex::com::FX_CLSCTX_INPROC_SERVER,IParserPlugin::iid,(void**)&pInterface);
					m_bCOMInit = true;

					if(FX_FAILED(hRet)) ///still failed
					{
						if(pPlugin->isInstalled() == false) ///is installed?
						{
							pPlugin->install();
						}						
						if(!pPlugin->isInstalled())
						{
							return false;
						}		
						///create again
						hRet = firtex::com::FX_CoCreateInstance(pPlugin->getCLSID(),NULL,firtex::com::FX_CLSCTX_INPROC_SERVER,IParserPlugin::iid,(void**)&pInterface);
					}
				}
			}
			if(FX_SUCCEEDED(hRet))
			{
				m_pluginMap.insert(make_pair(pPlugin->getIdentifier(),pInterface));
				CParserPlugin* pParserPlugin = new CParserPlugin(pInterface);
				///register the document type
				firtex::document::CDocumentType::getInstance()->registerType(pParserPlugin->getDocType().c_str(),pPlugin->getIdentifier().c_str());
				delete pParserPlugin;

				return true;
			}
			return false;
		}		

		void CParserPlugins::clear()
		{
			map<tstring,IParserPlugin*>::iterator iter = m_pluginMap.begin();
			while(iter != m_pluginMap.end())
			{
				iter->second->Release();				
				iter++;
			}
			m_pluginMap.clear();
		}
	}
}

