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
// Created	: 2006/8/3
//
#ifndef _CONFIGURATOR_H
#define _CONFIGURATOR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdHeader.h"
#include "../document/Field.h"
#include "../document/DocumentSchema.h"
#include <vector>
using namespace std;
using namespace firtex::document;

namespace firtex
{
	namespace utility
	{
		 
		class CXMLElement;
		class Configurator
		{
		public:
			Configurator(void);
			Configurator(const Configurator& src);
			~Configurator(void);
		public:
			/**
			 * load a configure file
			 * @param config path of configure file
			 * @exception throw CFirteXException exception
			 * 			 
			 * 参数文件格式说明：( (.)表示必须出现，(?)表示出现0次或1次，(+)表示出现1次或多次,(*)表示出现0次或多次)
			 *
			 * <argument>
			 *		<!--文档集类型定义-->
			 *		<collection>
			 *			<!-- collection类型,可取directory,filelist,firtex,trec. (.) -->
			 * 			<identifier></identifier>
			 *			<!-- 参数 -->
			 *			<parameter></parameter>
			 *		</collection>
			 *
			 *		<!--文档格式解析器定义. (.) -->
			 *		<parser>
			 *			<!-- 文档格式解析器类型，可取：(parser.)plain,(parser.)html,(parser.)pdf 等 (.) -->
			 * 			<identifier></identifier>
			 *		</parser>
			 *
			 *		<!-- 文档内容分析器定义 (.) -->
			 *		<analyzer>
			 *			<!-- 默认解析器标识 (.)-->
			 * 			<identifier></identifier>
			 *
			 *			<!-- Analyzer的参数，格式示例：stopword=standard:stopword.stf;stemmer=snowball:english:utf8;.... (?) -->
			 * 			<parameter></parameter>
			 *			 
			 *		</analyzer>
			 *
			 *		<!-- 索引参数定义 (.) -->
			 *		<index>
			 *			<!-- 索引文档源路径 (.) -->
			 * 			<source></source>
			 *
			 *			<!-- 索引存放位置 (.) -->
			 * 			<indexlocation></indexlocation>
			 *		
			 *			<!-- 索引级别，可取0,1或2. (?)
			 *			 0：WORD_LEVEL,索引包含文档ID，词频和词位置
			 *			 1：DOCUMENT_LEVEL，索引包含文档ID，词频信息
			 *			 2：DOCUMENT_LEVEL_0，索引仅包含文档ID信息 
			 *			-->
			 *			<level></level>
			 *
			 *			<!-- 索引的存取方式，可取readonly/write/readwrite/append/readappend,参见{@link CIndex} (.)-->
			 * 			<access_mode></access_mode>
			 *
			 *			<!-- 单篇文档最多索引词总数 (.) -->
			 * 			<maxterms></maxterms>
			 *
			 *			<!-- 索引缓存大小 (.) -->
			 * 			<memory></memory>
			 *
			 *			<!-- 文档缓存个数 (.) -->
			 *			<cacheddocs></cacheddocs>
			 *		</index>
			 *
			 *		<!--索引合并策略定义-->
			 *		<merge_strategy>
			 *
			 *			<!-- 索引合并策略 (.) -->
			 *			<strategy></strategy>
			 *
			 *			<!-- 对应该策略的合并参数,参数格式：参数名=参数值;参数名=参数值;...，多个参数通过;分开 (.) -->
			 * 			<param></param>
			 *
			 *			<!-- 垃圾文档回收门限值，当删除的文档数达到这个门限值时自动启动垃圾文档回收。参数格式：n=[deleted docs] 或 r=[deleted docs/all docs] (?) -->
			 *			<gc_threshold></gc_threshold>
			 *		</merge_strategy>			
			 *			 
			 *		<!-- log级别，可取:"default_level","enable_all","disable_all","fatal","err","warn","info","dbg" (.) -->
			 *		<log></log>
			 *
			 *		<!-- 索引文档模式定义,格式说明参见document/DocumentSchema.h说明.(?) -->
			 *		<document_schema></document_schema>
			 * </argument>
			 */
			void			load(const tchar* config);

			/**
			 * save configure to a file
			 * @param config path of file to save. if NULL then save to file where the configure is loaded 
			 * @exception throw CFirteXException exception
			 */
 			void			save(const tchar* config = NULL);

			/**
			 * get file path where configure is loaded 
			 * @return 
			 */
			const tchar* 	getPath(){return m_path.c_str();}

			/** 
			 * extract document scheme
			 * @return if document scheme is defined, the extract it and return, otherwise return NULL
			 */
			CDocumentSchema*	getSchema(){return m_pSchema;}

			/**
			 * set document scheme for save
			 * @param pScheme document scheme
			 */
			void				setSchema(CDocumentSchema* pScheme);
		public:
			struct scollection
			{
				tstring		identifier;		///identifier of collection: (collection.)directory,(collection.)filelist,(collection.)trec [MUST]
				tstring		parameter;		///parameter of collection
			};
			scollection collection;

			struct sparser
			{
				tstring		identifier;		///identifier of parser: (parser.)plain,(parser.)html,(parser.)pdf and so on [MUST]
				tstring		parameter;		///parameter of parser
			};
			sparser parser;

			struct sanalyzer
			{
				tstring	identifier;			///analyzer标识：chineseanalyzer
				tstring parameter;			///parameter of parser,format:param_name=parm_value;param_name=parm_value,e.g. stemmer=snowball;stopword=stopword file;lowercase=true
			};
			sanalyzer analyzer;

			struct sindex
			{
				tstring	source;				///源文件路径 [必需]
				tstring	indexlocation;		///索引存放路径 [必需]
				int32_t level;				///索引级别,默认为2,即WORD_LEVEL级别 [可选]
				tstring	accessmode;			///索引存取方式。[必须]				
				int32_t	maxterms;			///最多索引词数，如果没提供则使用{@link #CConfig}提供的参数。[可选]
				int64_t	memory;				///索引使用的内存Cache大小，如果没提供则使用{@link #CConfig}提供的参数。[可选]
				int32_t	cachedocs;			///索引时缓存的文档数，可减少函数调用代价，但需要更多的内存[可选]
			};
			sindex index;

			struct smergestrategy 
			{
				tstring	strategy;			///索引合并策略，参见{@link CIndexWriter}
				tstring	param;				///合并策略的参数,参数格式：参数名=参数值，多个参数通过空格分开
				tstring gc_threshold;		///垃圾文档回收门限值，当删除的文档数达到这个门限值时自动启动垃圾文档回收。参数格式：n=[deleted docs] 或 r=[deleted docs/all docs]
			};
			smergestrategy mergestrategy;		
			
		protected:
			void	add(const tstring& sName, int64_t* pInt64, int64_t nDefault);
			void	add(const tstring& sName, int32_t* pInt32, int32_t nDefault);
			void	add(const tstring& sName, double* pFloat, double nDefault);
			void	add(const tstring& sName, tstring* pString, const tstring& pszDefault);
			void	add(const tstring& sName, bool* pBool, bool bDefault = true);		
		protected:
			class ConfiguratorItem
			{
			public:				
				ConfiguratorItem(const tstring& sName);
				ConfiguratorItem(const tstring& sName, int64_t* pInt64,int32_t* pInt32,double* pFloat, tstring* pString,bool* pBool);
				virtual void	load(CXMLElement* pXML);
				virtual void	save(CXMLElement* pXML);
			public:
				tstring		m_sName;
				int32_t*	m_pInt32;
				int64_t*	m_pInt64;
				double*		m_pFloat;
				tstring*	m_pString;
				bool*		m_pBool;
			};			
		protected:
			typedef vector<ConfiguratorItem*> item_vector;

			item_vector			m_items;
			tstring				m_path;
			CDocumentSchema*	m_pSchema;
		};
	}
}

#endif
