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
// Created	: 2006/5/9
//
#include "plugin/AnalyzerPlugins.h"
#include "plugin/Plugins.h"
#include "com/comerror.h"
#include "plugin/ComReader.h"
#include "plugin/ComTokens.h"
#include "com/comerror.h"
#include "com/Com.h"
#include "com/ComPtr.h"

namespace firtex
{
	namespace plugin
	{
		//////////////////////////////////////////////////////////////////////////
		//CAnalyzerPlugin
		CAnalyzerPlugin::CAnalyzerPlugin(IAnalyzerPlugin* pAnalyzerInterface)
			: m_pAnalyzerInterface(pAnalyzerInterface)
			, m_pAnalyzedTokenVectorInterface(NULL)
			, m_pAnalyzedTokenVector(NULL)
			, m_pUnAnalyzedTokenVectorInterface(NULL)
			, m_pLastUnAnalyzedTokenVector(NULL)
		{
			m_pAnalyzerInterface->AddRef();
		}
	
		CAnalyzerPlugin::~CAnalyzerPlugin()
		{
			m_pAnalyzerInterface->Release();
			m_pAnalyzerInterface = NULL;

			if(m_pUnAnalyzedTokenVectorInterface)
			{
				delete m_pUnAnalyzedTokenVectorInterface;
				m_pUnAnalyzedTokenVectorInterface = NULL;
			}
		}	
		
		const tchar* CAnalyzerPlugin::getRecommendIndexer()
		{
			return NULL;//TODO:
		}
		
		CTokens* CAnalyzerPlugin::newTokenVector()
		{
			return NULL;//TODO:
		}

		const tchar* CAnalyzerPlugin::acceptableEncoding()
		{
			//TODO:
			return _T("");			
		}
		CTokens* CAnalyzerPlugin::nextTokensInternal(CTokens* pUnAnalyzedTokens)
		{
			if(m_pLastUnAnalyzedTokenVector != pUnAnalyzedTokens)
			{
				m_pUnAnalyzedTokenVectorInterface = CComTokens::wrap(pUnAnalyzedTokens);
				m_pLastUnAnalyzedTokenVector = pUnAnalyzedTokens;
			}
			
			ITokens* pAnalyzedTokenVector;
			if(m_pAnalyzerInterface->nextTokensInternal(m_pUnAnalyzedTokenVectorInterface,&pAnalyzedTokenVector) != FX_S_OK)
				return NULL;
			if(pAnalyzedTokenVector != m_pAnalyzedTokenVectorInterface)
			{
				m_pAnalyzedTokenVector = CComTokens::unwrap(pAnalyzedTokenVector);
				m_pAnalyzedTokenVectorInterface = pAnalyzedTokenVector;
			}
			return m_pAnalyzedTokenVector;
		}

		const tchar* CAnalyzerPlugin::getIdentifier()
		{
			//m_pAnalyzerInterface->getIdentifier();
			//TODO:
			return NULL;
		}

		//////////////////////////////////////////////////////////////////////////
		//CAnalyzerPlugins
		CAnalyzerPlugins::CAnalyzerPlugins(void)
			: m_bCOMInit(false)
		{
		}

		CAnalyzerPlugins::~CAnalyzerPlugins(void)
		{
			clear();
			if(m_bCOMInit)
			{
				firtex::com::FX_CoUninitialize();
			}
		}
		CAnalyzer* CAnalyzerPlugins::getAnalyzer(const tchar* analyzerName)
		{
			CAnalyzerPlugin* pAnalyzer = NULL;
			map<tstring,IAnalyzerPlugin*>::iterator iter = m_pluginMap.find(analyzerName);
			if(iter != m_pluginMap.end())
			{
				pAnalyzer = new CAnalyzerPlugin(iter->second);
				return pAnalyzer;
			}
			firtex::com::FX_CLSID	clsid;
			IAnalyzerPlugin* pInterface = NULL;
			if(GlobalPlugins.lookupCLSID(analyzerName,clsid))
			{
				firtex::com::FX_HRESULT hRet = firtex::com::FX_CoCreateInstance(clsid,NULL,firtex::com::FX_CLSCTX_INPROC_SERVER,IAnalyzerPlugin::iid,(void**)&pInterface);
				if(hRet == FX_S_OK)
				{
					m_pluginMap.insert(make_pair((tstring)analyzerName,pInterface));
					pAnalyzer = new CAnalyzerPlugin(pInterface);
					return pAnalyzer;
				}
			}
			return NULL;
		}		

		bool CAnalyzerPlugins::loadPlugin(CPlugin* pPlugin)
		{
			IAnalyzerPlugin* pInterface = NULL;
			firtex::com::FX_HRESULT hRet = firtex::com::FX_CoCreateInstance(pPlugin->getCLSID(),NULL,firtex::com::FX_CLSCTX_INPROC_SERVER,IAnalyzerPlugin::iid,(void**)&pInterface);
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
					hRet = firtex::com::FX_CoCreateInstance(pPlugin->getCLSID(),NULL,firtex::com::FX_CLSCTX_INPROC_SERVER,IAnalyzerPlugin::iid,(void**)&pInterface);
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
						hRet = firtex::com::FX_CoCreateInstance(pPlugin->getCLSID(),NULL,firtex::com::FX_CLSCTX_INPROC_SERVER,IAnalyzerPlugin::iid,(void**)&pInterface);
					}
				}
			}
			if(FX_SUCCEEDED(hRet))
			{
				m_pluginMap.insert(make_pair(pPlugin->getIdentifier(),pInterface));				
				return true;
			}
			return false;
		}
		void CAnalyzerPlugins::clear()
		{
			map<tstring,IAnalyzerPlugin*>::iterator iter = m_pluginMap.begin();
			while(iter != m_pluginMap.end())
			{
				iter->second->Release();				
				iter++;
			}
			m_pluginMap.clear();
		}
	}
}

