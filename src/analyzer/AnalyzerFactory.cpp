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
// Created	: 2005/11/20
//
#include "utility/Config.h"
#include "analyzer/AnalyzerFactory.h"
#include "analyzer/StandardAnalyzer.h"
#include "plugin//AnalyzerPlugins.h"
#include "plugin/Plugins.h"
#include "analyzer/StandardStopFilter.h"
#include "utility/StringUtils.h"
#include "utility/ParamParser.h"
#include "analyzer/LowercaseAnteFilter.h"
#include <memory>

#ifdef HAS_SNOWBALL_STEMMER
	#include "snowball/include/Snowball.h"
#endif



namespace firtex
{
	namespace analyzer
	{
		CAnalyzerFactory::CAnalyzerFactory(void)
		{
		}

		CAnalyzerFactory::~CAnalyzerFactory(void)
		{
		}		
		CAnalyzer* CAnalyzerFactory::createAnalyzer(const tchar* szIdentifier,const tchar* szParam)
		{
			CAnalyzer* pAnalyzer = firtex::plugin::GlobalPlugins.AnalyzerPlugin.getAnalyzer(szIdentifier);
			if(!pAnalyzer)				
			{
				CAnalyzer::Creator* pCreator = CAnalyzerFactory::getCreator(szIdentifier);
				if(pCreator)
				{
					pAnalyzer = pCreator->create();
				}
			}
			if(!pAnalyzer)
				return NULL;		
			
			if(szParam && *szParam)
			{
				pAnalyzer->setParam(szParam);
			}			
			return pAnalyzer;
		}				

		CAnalyzer* CAnalyzerFactory::standardAnalyzer(const tchar* szParam,const tchar* szLanguage)
		{
			if(!szLanguage || !_tcscmp(szLanguage,_T("chinese")) || !_tcscmp(szLanguage,_T("english")))
			{
				CAnalyzer* pAnalyzer =  new CStandardAnalyzer();
				if(szParam && *szParam)
				{
					CParamParser pp;
					pp.parse(szParam,_T(";"));
					tstring str;
					if(pp.getParam(_T("stopword"),str) && !str.empty())
					{						
					}
				}				
				return pAnalyzer;
			}
			return NULL;
		}

		CTokenFilter* CAnalyzerFactory::createStemmerFilter(const tchar* szIdendifier)
		{
			CParamParser pp;
			if(pp.parse2(szIdendifier))
			{
				tstring iden;
				if(pp.getParam2(0,iden))
				{
#ifdef HAS_SNOWBALL_STEMMER
					if(firtex::string_utils::endsWithNoCase(iden.c_str(),_T("snowball")))
					{		
						tstring lang,enc;
						pp.getParam2(1,lang);
						pp.getParam2(2,enc);
						if(lang.empty())
							lang = _T("english");
						else lang = firtex::string_utils::toLower(lang);
						return new CSnowballFilter(lang.c_str(),enc.empty()?NULL:enc.c_str());
					}	
#endif
				}

			}
			return NULL;
		}

		
		CTokenFilter* CAnalyzerFactory::createStopFilter(const tchar* szIdendifier)
		{
			CParamParser pp;
			if(pp.parse2(szIdendifier))
			{
				tstring iden;
				if(pp.getParam2(0,iden))
				{
					if(firtex::string_utils::endsWithNoCase(iden.c_str(),_T("standard")))
					{
						tstring file;
						pp.getParam2(1,file);					
						tstring str;
						if(!_tcsicmp(file.c_str(),_T("stopword.stf")))
						{
							str = GlobalConfig.General.Path + _T("\\data\\");
							str += file;
						}
						else str = file;
						if(CMisc::dirExists(str.c_str()))
						{
							FIRTEX_CLOG(level::info) << _T("Use stopword:") << str << FIRTEX_ENDL;
							auto_ptr<CStandardStopFilter> filter_ptr(new CStandardStopFilter());
							try
							{
								filter_ptr->load(str.c_str());							
								return filter_ptr.release();
							}
							catch(...)
							{
								FIRTEX_CLOG(level::warn) << _T("Load stop word file failed:") << str << FIRTEX_ENDL;
							}
						}
						else 
							FIRTEX_CLOG(level::info) << _T("Use stop word,but can't find stop word file :") << str << FIRTEX_ENDL;

						return NULL;
					}
				}				
			}	
			return NULL;
		}
		void CAnalyzerFactory::registerCreator(CAnalyzer::Creator* pCreator)
		{
			tstring str = pCreator->getIdentifier();
			getCreatorMap().insert(make_pair(str,pCreator));
		}
		CAnalyzer::Creator* CAnalyzerFactory::getCreator(const tchar* _identifier)
		{
			tstring str = _identifier;
			creator_map& am = getCreatorMap();
			creator_map::iterator iter = am.find(str);
			if(iter != am.end())
				return iter->second;
			return NULL;
		}		
		creator_map& CAnalyzerFactory::getCreatorMap()
		{
			static creator_map INSTANCE;
			return INSTANCE;
		}

		analyzer_list CAnalyzerFactory::getRegisteredAnalyzers()
		{			
			analyzer_list  al;
			creator_map& am = getCreatorMap();
			creator_map::iterator iter = am.begin();
			while(iter != am.end())
			{
				al.push_back(iter->first);
				iter++;
			}
			return al;
		}
	}
}
