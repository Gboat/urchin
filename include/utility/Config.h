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
// Created	: 2005/12/3
//
#ifndef _CONFIG_H
#define _CONFIG_H

#include "StdHeader.h"
#include "logger.h"
#include <list>
#include "Starter.h"
using namespace std;

namespace firtex
{
	namespace utility
	{		
		class CXMLElement;
		class CConfig
		{
		public:
			CConfig(void);
			~CConfig(void);
		public:
			/**
			 * 设置Log级别
			 * @param 级别,可取:"default_level","enable_all","disable_all","fatal","err","warn","info","dbg"
			 */
			void		setLogLevel(const tchar* l);
			tstring&	getLogLevel(){return m_sLoglevel;}	
		protected:
			void		setup();
			void		terminate();
			void		initMMS();
			bool		getMMSConfig(tstring strConfig,int32_t& strategy,int32_t& n,int32_t& k,int32_t l);
		public:
			struct sGeneral
			{
				tstring Path;		///程序所在路径
				tstring	PluginPath;	///插件路径
			};
			sGeneral General;

			struct sParser 
			{
			};
			sParser Parser;

			struct sAnalyzer
			{
				tstring parameter;			///parameter of parser,format:param_name=parm_value;param_name=parm_value,e.g. stemmer=snowball;stopword=stopword file;lowercase=true
			};
			sAnalyzer Analyzer;

			struct sIndex
			{
				tstring level;				///索引级别,默认为WORD_LEVEL级别
				int64_t	memory;				///索引使用的内存Cache大小
				int32_t	numCachedDocs;		///缓存的文档数目
				int32_t	maxIndexTerms;		///最多索引词数				
			};
			sIndex Index;

			struct sSearch 
			{
				int64_t	memory;				///memory buffer size for search
			};
			sSearch Search;

			struct sMergeStrategy 
			{
				tstring strategy;			///索引合并策略
				tstring param;				///合并参数
				tstring gcThreshold;		///垃圾文档回收门限值，当删除的文档数达到这个门限值时自动启动垃圾文档回收。参数格式：n=[deleted docs] 或 r=[deleted docs/all docs]
			};
			sMergeStrategy MergeStrategy;

			tlogger	Logger;

			struct sAdvance 
			{
				///Memory Management Strategy
				struct sMMS
				{
					///strategy for WORD_LEVEL index
					struct sWordLevel
					{
						///format: strategy:param1:param2... currently,it can be:
						///1. const:n  const表示内存按常数大小分配，n表示分配的内存大小值
						///2. exp:n:k 以max{n, 「p*(k-1)」}的规律递增Posting Group 大小,n表示分配的初始值，p为
						///   累计总大小,exp:8:1.5,则分配的内存大小序列为8,8,8,12,18,27,...
						///3.explimit:n:k:l 以min{l，max{n, 「p*(k-1)」}}的规律递增Posting Group 大小,n,p,k含义
						///  同上,l表示分配上限,例如explimit:8:2:32,则分配的内存大小序列为8,8,16,32,32,...
						tstring strategy; 
					};
					sWordLevel WordLevel;

					///strategy for DOCUMENT_LEVEL index
					struct sDocumentLevel
					{
						tstring strategy;
					};
					sDocumentLevel DocumentLevel;

					///strategy for DOCUMENT_LEVEL_0 index
					struct sDocumentLevel0
					{
						tstring strategy;
					};
					sDocumentLevel0 DocumentLevel0;
				};
				sMMS MMS;

				///当建立索引MemCache内存耗尽，但当前文档尚未索引完毕，进入UPTIGHT MEMORY ALLOCATE 状态，
				///下面的配置参数指示进入该状态时使用的内存策略
				struct sUptightAlloc 
				{
					int32_t chunkSize;		///chunk分配内存大小
					int32_t memSize;		///重新向系统申请内存大小，默认情况下是：Index.maxIndexTerms*chunkSize/2
				};
				sUptightAlloc UptightAlloc;
			};
			sAdvance Advance;
		public:
			void	load(const tstring& sFile = "");
			void	save(const tstring& sFile = "");		
		protected:
			void	add(const tstring& sName, int64_t* pDword, int64_t nDefault);
			void	add(const tstring& sName, int32_t* pDword, int32_t nDefault);
			void	add(const tstring& sName, double* pFloat, double nDefault);
			void	add(const tstring& sName, tstring* pString, const tstring& pszDefault);
		protected:			
		public:
			class CParamItem
			{
			public:
				CParamItem(const tstring& sName, int64_t* pInt64,int32_t* pInt32,double* pFloat, tstring* pString);
				void	load(CXMLElement* pXML);
				void	save(CXMLElement* pXML);
			public:
				tstring		m_sName;
				int32_t*	m_pInt32;
				int64_t*	m_pInt64;
				double*		m_pFloat;
				tstring*	m_pString;
			};
		protected:
			list<CParamItem*>	m_items;
			tstring				m_sPath;

			tstring				m_sLoglevel;//disable_all,enable_all,default_level(info),fatal,err,warn,info,dbg
		protected:
			Starter				m_starter;
		};
		extern CConfig GlobalConfig;
		#define FIRTEX_CLOG(log_level) FIRTEX_LOG(GlobalConfig.Logger,log_level)
	}
}

using namespace firtex::utility;

#endif
