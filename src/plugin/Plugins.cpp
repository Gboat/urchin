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
#include "plugin/Plugins.h"
#include "plugin/ComObject.h"
#include "utility/Config.h"
#include "com/comerror.h"
#include "utility/StringUtils.h"

namespace firtex
{
	namespace plugin
	{
		CPlugins	GlobalPlugins;

		CPlugins::CPlugins(void)
		{
			firtex::com::FX_HRESULT hRet = firtex::com::FX_CoInitializeEx( NULL, ::COINIT_APARTMENTTHREADED );
			if(hRet!=FX_S_OK)
				FIRTEX_CLOG(level::warn) << _T("CoInitialize Failed.") << FIRTEX_ENDL;
			enumerate();//遍历插件
		}

		CPlugins::~CPlugins(void)
		{
			clear();
		}
		void CPlugins::enumerate()
		{
			//plugin.xml格式：
			// <plugins>
			//	 <plugin>			
			//		<identifier>identifier</identifier>
			//		<clsid></clsid>
			//		<name></name>			
			//		<progid></progid>
			//		<versionindependentprogid></versionindependentprogid>
			//		<threadingmodel></threadingmodel>
			//		<filename></filename>	
			//		<enable>true/false</enable>
			//	 </plugin>
			// </plugins>

			CXMLElement* pXMLPlugin = NULL;
			try
			{
				pXMLPlugin = CXMLElement::fromFile(GlobalConfig.General.PluginPath + PATH_DELIMITERA + _T("plugins.xml"));
				if(!pXMLPlugin)
					return;
				FIRTEX_CLOG(level::dbg) << _T("Found plugins.xml:") << GlobalConfig.General.PluginPath + PATH_DELIMITERA + _T("plugins.xml") << FIRTEX_ENDL;

				CPlugin* pPlugin;
				CXMLElement* pPluginElem;
				ElementIterator iter = pXMLPlugin->getElementIterator();
				while (pXMLPlugin->hasNextElement(iter))
				{
					pPluginElem = pXMLPlugin->getNextElement(iter);
					pPlugin = new CPlugin();
					if(pPlugin->load(pPluginElem))
					{
						if(firtex::string_utils::startsWithNoCase(pPlugin->getIdentifier().c_str(),_T("parser")))
						{
							if(!ParserPlugin.loadPlugin(pPlugin))
							{
								FIRTEX_CLOG(level::warn) << _T("Install parser plugin failed:") << pPlugin->getName() << FIRTEX_ENDL;
								delete pPlugin;
								continue;
							}							
						}
						else if(firtex::string_utils::startsWithNoCase(pPlugin->getIdentifier().c_str(),_T("analyzer")))
						{
							if(!AnalyzerPlugin.loadPlugin(pPlugin))
							{
								FIRTEX_CLOG(level::warn) << _T("Install analyzer plugin failed:") << pPlugin->getName() << FIRTEX_ENDL;
								delete pPlugin;
								continue;
							}
						}
						else 
						{
							FIRTEX_CLOG(level::warn) << _T("Unkown plugin:") << pPlugin->getName() << FIRTEX_ENDL;
							delete pPlugin;
							continue;
						}			
						pPlugin->m_bInstalled = true;
						FIRTEX_CLOG(level::dbg) << _T("Load plugin success:") << pPlugin->getName() << FIRTEX_ENDL;
						m_plugins.push_back(pPlugin);
					}
					else
					{
						delete pPlugin;
					}
				}				
				delete pXMLPlugin;
				pXMLPlugin = NULL;
			}
			catch (CFirteXException& e)
			{
				if(pXMLPlugin)
					delete pXMLPlugin;
				FIRTEX_CLOG(level::warn) << _T("Enumerater plugins error: void CPlugins::enumerate() : ") << e.what() << FIRTEX_ENDL;
			}
		}
		void CPlugins::clear()
		{
			vector<CPlugin*>::iterator iter = m_plugins.begin();
			while (iter != m_plugins.end())
			{
				delete *iter;								
				iter++;
			}
			m_plugins.clear();
		}
		bool CPlugins::lookupCLSID(const tchar* identifier, firtex::com::FX_CLSID& pCLSID)
		{	
			CPlugin* pPlugin;
			vector<CPlugin*>::iterator iter = m_plugins.begin();
			while (iter != m_plugins.end())
			{
				pPlugin = *iter;
				if( *identifier != 0 &&  firtex::string_utils::endsWithNoCase(pPlugin->m_identifier.c_str(),identifier) )
				{
					pCLSID = pPlugin->m_clsid;
					return pPlugin->m_bEnable;
				}
				iter++;
			}
			return false;
		}
		bool CPlugins::lookupEnable(FX_REFCLSID pCLSID, bool bDefault)
		{
			CPlugin* pPlugin;
			vector<CPlugin*>::iterator iter = m_plugins.begin();
			while (iter != m_plugins.end())
			{
				pPlugin = *iter;
				if( FX_IsEqualCLSID(pCLSID,pPlugin->m_clsid) )
				{					
					return pPlugin->m_bEnable;
				}
				iter++;
			}			
			return bDefault;
		}
		bool CPlugins::lookupInstall(FX_REFCLSID pCLSID)
		{
			CPlugin* pPlugin;
			vector<CPlugin*>::iterator iter = m_plugins.begin();
			while (iter != m_plugins.end())
			{
				pPlugin = *iter;
				if( FX_IsEqualCLSID(pCLSID,pPlugin->m_clsid) )
				{					
					return pPlugin->m_bInstalled;
				}
				iter++;
			}			
			return false;
		}


		CPlugin* CPlugins::findByFileName(const tchar* filename)
		{
			vector<CPlugin*>::iterator iter = m_plugins.begin();
			CPlugin* pPlugin;
			while(iter!=m_plugins.end())
			{
				pPlugin = *iter;
				if(!_tcsicmp(pPlugin->m_filename.c_str(),filename))
					return (*iter);
				iter++;
			}
			return NULL;
		}
		//////////////////////////////////////////////////////////////////////////
		//CPlugin
		CPlugin::CPlugin()
			:m_bInstalled(false)
			,m_bEnable(false)
		{
		}
		CPlugin::~CPlugin()
		{
		}
		bool CPlugin::load(CXMLElement* pXMLPlugin)
		{
			//plugin.xml格式：
			// <plugins>
			//	 <plugin>			
			//		<identifier>identifier</identifier>
			//		<clsid></clsid>
			//		<name></name>			
			//		<progid></progid>
			//		<versionindependentprogid></versionindependentprogid>
			//		<threadingmodel></threadingmodel>
			//		<filename></filename>	
			//		<enable>true/false</enable>
			//	 </plugin>
			// </plugins>
			try
			{
				CXMLElement* pItem;				
				pItem = pXMLPlugin->getElementByName(_T("identifier"));
				m_identifier = pItem->getValue();
				pItem = pXMLPlugin->getElementByName(_T("clsid"));
				tstring str = pItem->getValue();
				GUIDX::decode(str.c_str(),&m_clsid);
				pItem = pXMLPlugin->getElementByName(_T("name"));
				m_name = pItem->getValue();
				pItem = pXMLPlugin->getElementByName(_T("progid"));
				m_progid = pItem->getValue();
				pItem = pXMLPlugin->getElementByName(_T("versionindependentprogid"));
				m_versionindependentprogid = pItem->getValue();
				pItem = pXMLPlugin->getElementByName(_T("threadingmodel"));
				m_threadingmodel = pItem->getValue();
				pItem = pXMLPlugin->getElementByName(_T("filename"));
				m_filename = pItem->getValue();
				pItem = pXMLPlugin->getElementByName(_T("enable"));
				str = pItem->getValue();
				m_bEnable = (!_tcsicmp(str.c_str(),("true")));

				m_bInstalled = false;				
			}
			catch (...)
			{
				FIRTEX_CLOG(level::warn) << _T("Load plugin.xml error.") << FIRTEX_ENDL;
				return false;
			}
			return true;			
		}

		bool CPlugin::install()
		{
			//tstring sext;
			tstring str = GlobalConfig.General.PluginPath;
			str += PATH_DELIMITERA;
			str += m_filename;
#ifdef WIN32
			str += _T(".dll");
#else 
			str += _T(".so");			
#endif
			
#ifdef _UNICODE
			firtex::com::FX_HRESULT hr = firtex::com::FX_RegSrv32W(str.c_str(),1);
#else 
			firtex::com::FX_HRESULT hr = firtex::com::FX_RegSrv32A(str.c_str(),1);
#endif
			if((hr==FX_S_OK))
				m_bInstalled = true;
			return (hr==FX_S_OK);
			
		}
		bool CPlugin::uninstall()
		{
			tstring str = GlobalConfig.General.PluginPath;
			str += PATH_DELIMITERA;
			str += m_filename;
#ifdef WIN32
			str += _T(".dll");
#else 
			str += _T(".so");
#endif
			
#ifdef _UNICODE
			firtex::com::FX_HRESULT hr = firtex::com::FX_RegSrv32W(str.c_str(),0);
#else 
			firtex::com::FX_HRESULT hr = firtex::com::FX_RegSrv32A(str.c_str(),0);
#endif
			if(hr==FX_S_OK)
				m_bInstalled = false;
			return (hr==FX_S_OK);
		}
	}
}
