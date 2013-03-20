//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: �����(GuoRuijie)
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
			 * ����Log����
			 * @param ����,��ȡ:"default_level","enable_all","disable_all","fatal","err","warn","info","dbg"
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
				tstring Path;		///��������·��
				tstring	PluginPath;	///���·��
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
				tstring level;				///��������,Ĭ��ΪWORD_LEVEL����
				int64_t	memory;				///����ʹ�õ��ڴ�Cache��С
				int32_t	numCachedDocs;		///������ĵ���Ŀ
				int32_t	maxIndexTerms;		///�����������				
			};
			sIndex Index;

			struct sSearch 
			{
				int64_t	memory;				///memory buffer size for search
			};
			sSearch Search;

			struct sMergeStrategy 
			{
				tstring strategy;			///�����ϲ�����
				tstring param;				///�ϲ�����
				tstring gcThreshold;		///�����ĵ���������ֵ����ɾ�����ĵ����ﵽ�������ֵʱ�Զ����������ĵ����ա�������ʽ��n=[deleted docs] �� r=[deleted docs/all docs]
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
						///1. const:n  const��ʾ�ڴ水������С���䣬n��ʾ������ڴ��Сֵ
						///2. exp:n:k ��max{n, ��p*(k-1)��}�Ĺ��ɵ���Posting Group ��С,n��ʾ����ĳ�ʼֵ��pΪ
						///   �ۼ��ܴ�С,exp:8:1.5,�������ڴ��С����Ϊ8,8,8,12,18,27,...
						///3.explimit:n:k:l ��min{l��max{n, ��p*(k-1)��}}�Ĺ��ɵ���Posting Group ��С,n,p,k����
						///  ͬ��,l��ʾ��������,����explimit:8:2:32,�������ڴ��С����Ϊ8,8,16,32,32,...
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

				///����������MemCache�ڴ�ľ�������ǰ�ĵ���δ������ϣ�����UPTIGHT MEMORY ALLOCATE ״̬��
				///��������ò���ָʾ�����״̬ʱʹ�õ��ڴ����
				struct sUptightAlloc 
				{
					int32_t chunkSize;		///chunk�����ڴ��С
					int32_t memSize;		///������ϵͳ�����ڴ��С��Ĭ��������ǣ�Index.maxIndexTerms*chunkSize/2
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
