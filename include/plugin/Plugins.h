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
#ifndef _PLUGINS_H
#define _PLUGINS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "../com/Com.h"
#include "../utility/StdHeader.h"
#include "../utility/XML.h"
#include "ParserPlugins.h"
#include "AnalyzerPlugins.h"
#include "ComObject.h"

using namespace firtex::com;
using namespace std;
using namespace firtex::utility;

namespace firtex
{
	namespace plugin
	{
		class CPlugin
		{
		public:
			CPlugin();
			~CPlugin();
		public:
			bool			load(CXMLElement* pXMLPlugin);			
			const tstring&	getIdentifier(){return m_identifier;}
			const FX_CLSID&	getCLSID(){return m_clsid;}
			tstring			getCLSIDAsString(){return GUIDX::encode(&m_clsid);}
			const tstring&	getName(){return m_name;}
			const tstring&	getProgid(){return m_progid;}
			bool			isEnabled(){return m_bEnable;}

			bool			isInstalled(){return m_bInstalled;}
		public:
			bool			install();
			bool			uninstall();
		protected:			
			tstring		m_identifier;
			FX_CLSID	m_clsid;
			tstring		m_name;			//²å¼þÃû³Æ
			tstring		m_progid;
			tstring		m_versionindependentprogid;
			tstring		m_threadingmodel;
			tstring		m_filename;		//²å¼þÂ·¾¶
			bool		m_bEnable;		//ÊÇ·ñ½ûÖ¹
			bool		m_bInstalled;
			friend class CPlugins;
		};

		
		class CPlugins
		{
		public:
			CPlugins(void);
			virtual ~CPlugins(void);
		public:
			void		enumerate();
			void		clear();
			bool		lookupCLSID(const tchar* identifier, firtex::com::FX_CLSID& pCLSID);
			bool		lookupEnable(FX_REFCLSID pCLSID, bool bDefault);
			bool		lookupInstall(FX_REFCLSID pCLSID);
		public:
			CPlugin*	findByFileName(const tchar* filename);
		public:
			void		startIterator();
			bool		hasNext();
			CPlugin*	next();
		public:
			CParserPlugins		ParserPlugin;
			CAnalyzerPlugins	AnalyzerPlugin;
		protected:
			vector<CPlugin*>	m_plugins;
			vector<CPlugin*>::iterator m_iterator;
		};		
		//////////////////////////////////////////////////////////////////////////
		//
		inline void CPlugins::startIterator()
		{
			m_iterator = m_plugins.begin();
		}
		inline bool CPlugins::hasNext()
		{
			return (m_iterator != m_plugins.end());
		}
		inline CPlugin* CPlugins::next()
		{
			return *m_iterator++;
		}

		extern CPlugins	GlobalPlugins;
	}
}

#endif
