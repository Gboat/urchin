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
// Created	: 2006/6/27
//
#ifndef _DOCUMENTSCHEMA_H
#define _DOCUMENTSCHEMA_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Field.h"
#include "../utility/FXString.h"
#include "../utility/XML.h"
#include "../index/Posting.h"
#include <vector>
#include <map>

using namespace std;
using namespace firtex::utility;
using namespace firtex::index;

namespace firtex
{
	namespace document
	{
		typedef map<tstring,tstring> CompMap;
		class CSchemaItem
		{
		public:
			CSchemaItem();
			CSchemaItem(fieldid_t id,const tchar* name,store_t store,index_t index,forwardindex_t fi,float fBoost = 1.0);			
			CSchemaItem(const CSchemaItem& src);
			~CSchemaItem();

		public:
			fieldid_t		getId(){return m_id;}
			const tchar*	getName(){return m_sName;}			
			store_t			getStore(){return m_store;}
			index_t			getIndex(){return m_index;}
			forwardindex_t	getForwardIndex(){return m_forwardIndex;}
			INDEX_LEVEL		getIndexLevel(){return m_eIndexLevel;}
			float			getBoost(){return m_fBoost;}

			const tchar*	getAlias(){return m_sAliasName.c_str();}

			const tchar*	getComponent(const tchar* szCompName);

			void			setId(schemaid_t sid){m_id = sid;}
			void			setName(const tchar* name){m_sName = name;}			
			void			setStore(store_t store){m_store = store;}
			void			setIndex(index_t index){m_index = index;}
			void			setForwardIndex(forwardindex_t fi){m_forwardIndex = fi;}
			void			setIndexLevel(INDEX_LEVEL il){m_eIndexLevel = il;}
			void			setBoost(float fBoost){m_fBoost = fBoost;}

			void			setAlias(const tchar* pszAlias);

			void			setComponent(const tchar* szCompName,const tchar* szCompValue);
			
		public:
			/**
			 * 从XML文件中装入Schema 信息
			 * Example:
			 * <schema_item>
			 *		<!-- 字段名称，[必须] -->
			 *		<name>field name</name>
			 *
			 *		<!-- 字段别名，[可选] -->
			 *		<alias></alias>
			 *
			 *		<!-- 字段存储属性，可取yes/no/compress，分别表示存储、不存储和压缩存储该字段. [必须] -->
			 *		<store>yes/no/compress</store>
			 *
			 *		<!-- 字段索引属性，可取no/analyze/unanalyze，分别表示不分析不索引、分析并索引和不分析但索引该字段. [必须] -->
			 *		<index>no/analyze/unanalyze</index>
			 *
			 *		<!-- 字段前向索引属性，可取no/sequence/freq/position，分别表示无前向索引，序列式、分析并索引和不分析但索引该字段. [必须] -->
			 *		<forward_index>no/sequence/freq/position</forward_index>
			 *
			 *		<!-- 字段使用的组件 -->
			 *		<components>
			 *			<!-- 字段使用的Analyzer组件 -->
			 *			<component name ="analyzer"></component>
			 *			<!-- 字段使用的Analyzer组件的参数 -->
			 *			<component name ="analyzer_parameter"></component>
			 *			<!-- 字段使用的Indexer组件 -->			
			 *			<component name ="indexer"></component>			 			 
			 *		</components>
			 *			 
			 *		<!-- 索引级别，可取0,1或2. (?)
			 *			 0：WORD_LEVEL,索引包含文档ID，词频和词位置
			 *			 1：DOCUMENT_LEVEL，索引包含文档ID，词频信息
			 *			 2：DOCUMENT_LEVEL_0，索引仅包含文档ID信息 
			 *      -->
			 *		<index_level>2</index_level>
			 *
			 *		<!-- 字段的重要度 -->
			 *		<boost>1.0</boost>
			 * </schema_item>
			 * @param pXML schema item的XML 元素
			 * @return true:success false:failed.
			 */
			bool			load(CXMLElement* pXML);
			
			/**
			 * 将Schema Item信息写入XML文件
			 * @param pXML schema item的XML 元素
			 */
			void			save(CXMLElement* pXML);
		public:
			/** 
			 * 判断两个SchemaItem是否相等,不比较m_id成员
			 * @return 相等返回true,否则false
			 */			
			CSchemaItem& operator=(const CSchemaItem& right);			
		protected:
			void	clear();
			void	loadComponents(CXMLElement* pCompRoot);
			void	saveComponents(CXMLElement* pCompRoot);
		protected:
			fieldid_t		m_id;				///编号
			CFXString		m_sName;				///名称
			store_t			m_store;			///存储方式
			index_t			m_index;			///索引方式
			forwardindex_t	m_forwardIndex;		///前向索引方式			
			float			m_fBoost;			///boost值

			INDEX_LEVEL		m_eIndexLevel;		///索引级别
			CFXString		m_sAliasName;		///字段别名，可选			
			CompMap			m_components;

			friend class CDocumentSchema;
		};

		class CDocumentSchema
		{
		public:
			CDocumentSchema(void);
			CDocumentSchema(const CDocumentSchema& src);
			virtual ~CDocumentSchema(void);
		public:
			fieldid_t		addKeywordItem(const tchar* name);
			fieldid_t		addUnIndexedItem(const tchar* name);
			fieldid_t		addTextItem(const tchar* name,store_t store=Store::NO,forwardindex_t termVector=ForwardIndex::NO);
			fieldid_t		addUnStoredItem(const tchar* name,forwardindex_t termVector=ForwardIndex::NO);
			fieldid_t		addBinaryItem(const tchar* name,bool bCompress);

			fieldid_t		addItem(const tchar* name,index_t index,store_t store,forwardindex_t termVector);
			fieldid_t		addItem(const CSchemaItem& item);

			/**
			 * 删除项
			 * @param name 项名称
			 */
			void			deleteItem(const tchar* name);

			/**
			 * 更新schema id，如果修改或删除了项需要进行ID 更新以保持ID一致
			 */
			void			updateID();

			/**
			 * 查找名称为name的Schema信息
			 * @param name schema名称
			 */
			CSchemaItem*	findSchema(const tchar* name);

			/**
			 * 获取Shema Item
			 * @param sid schema id
			 * @return schema item
			 */
			CSchemaItem*	getSchema(schemaid_t sid);
		public:
			int32_t			getCount()const{return (int32_t)m_items.size();}

			void			startIterator(){m_iterator = m_items.begin();}
			bool			hasNext(){return (m_iterator != m_items.end());}
			CSchemaItem*	next(){return *m_iterator++;}
			
			CDocumentSchema& operator= (const CDocumentSchema& right);		
		public:
			/**
			 * 装入schema文件
			 * @param schemaFile 文件路径
			 * schema file format:
			 * <document_schema>
			 *
			 *		<!-- 字段的schema定义 -->
			 *		<schemaitem></schemaitem>
			 * </document_schema>
			 */
			void	loadSchema(const tchar*	schemaFile);

			/**
			 * 从XML片段中装入schema信息
			 * @param pXMLSchema XML的Schema片段
			 */
			void	loadSchema(CXMLElement* pXMLSchema);

			/**
			 * 保存schema信息至文件
			 * @param schemaFile 文件路径
			 */
			void	saveSchema(const tchar*	schemaFile);

			/**
			 * 保存schema信息至XML片段
			 * @param pXMLSchema XML片段
			 */
			void	saveSchema(CXMLElement* pXMLSchema);

			/**
			 * 清空schema信息
			 */
			void	clear();
		private:
			vector<CSchemaItem*>	m_items;
			vector<CSchemaItem*>::iterator	m_iterator;
			tstring					m_schemaFile;
			schemaid_t				m_schemaCounter;
		};
		

		//////////////////////////////////////////////////////////////////////////
		//CDocumentSchema's inline function
				
		inline CDocumentSchema& CDocumentSchema::operator= (const CDocumentSchema& right)
		{
			clear();
			vector<CSchemaItem*>::const_iterator iter = right.m_items.begin();
			while (iter != right.m_items.end())
			{
				m_items.push_back(new CSchemaItem(*(*iter)));
				iter++;
			}
			m_schemaCounter = right.m_schemaCounter;
			return *this;
		}
	}
}


#endif
