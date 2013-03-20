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
			 * ��XML�ļ���װ��Schema ��Ϣ
			 * Example:
			 * <schema_item>
			 *		<!-- �ֶ����ƣ�[����] -->
			 *		<name>field name</name>
			 *
			 *		<!-- �ֶα�����[��ѡ] -->
			 *		<alias></alias>
			 *
			 *		<!-- �ֶδ洢���ԣ���ȡyes/no/compress���ֱ��ʾ�洢�����洢��ѹ���洢���ֶ�. [����] -->
			 *		<store>yes/no/compress</store>
			 *
			 *		<!-- �ֶ��������ԣ���ȡno/analyze/unanalyze���ֱ��ʾ�������������������������Ͳ��������������ֶ�. [����] -->
			 *		<index>no/analyze/unanalyze</index>
			 *
			 *		<!-- �ֶ�ǰ���������ԣ���ȡno/sequence/freq/position���ֱ��ʾ��ǰ������������ʽ�������������Ͳ��������������ֶ�. [����] -->
			 *		<forward_index>no/sequence/freq/position</forward_index>
			 *
			 *		<!-- �ֶ�ʹ�õ���� -->
			 *		<components>
			 *			<!-- �ֶ�ʹ�õ�Analyzer��� -->
			 *			<component name ="analyzer"></component>
			 *			<!-- �ֶ�ʹ�õ�Analyzer����Ĳ��� -->
			 *			<component name ="analyzer_parameter"></component>
			 *			<!-- �ֶ�ʹ�õ�Indexer��� -->			
			 *			<component name ="indexer"></component>			 			 
			 *		</components>
			 *			 
			 *		<!-- �������𣬿�ȡ0,1��2. (?)
			 *			 0��WORD_LEVEL,���������ĵ�ID����Ƶ�ʹ�λ��
			 *			 1��DOCUMENT_LEVEL�����������ĵ�ID����Ƶ��Ϣ
			 *			 2��DOCUMENT_LEVEL_0�������������ĵ�ID��Ϣ 
			 *      -->
			 *		<index_level>2</index_level>
			 *
			 *		<!-- �ֶε���Ҫ�� -->
			 *		<boost>1.0</boost>
			 * </schema_item>
			 * @param pXML schema item��XML Ԫ��
			 * @return true:success false:failed.
			 */
			bool			load(CXMLElement* pXML);
			
			/**
			 * ��Schema Item��Ϣд��XML�ļ�
			 * @param pXML schema item��XML Ԫ��
			 */
			void			save(CXMLElement* pXML);
		public:
			/** 
			 * �ж�����SchemaItem�Ƿ����,���Ƚ�m_id��Ա
			 * @return ��ȷ���true,����false
			 */			
			CSchemaItem& operator=(const CSchemaItem& right);			
		protected:
			void	clear();
			void	loadComponents(CXMLElement* pCompRoot);
			void	saveComponents(CXMLElement* pCompRoot);
		protected:
			fieldid_t		m_id;				///���
			CFXString		m_sName;				///����
			store_t			m_store;			///�洢��ʽ
			index_t			m_index;			///������ʽ
			forwardindex_t	m_forwardIndex;		///ǰ��������ʽ			
			float			m_fBoost;			///boostֵ

			INDEX_LEVEL		m_eIndexLevel;		///��������
			CFXString		m_sAliasName;		///�ֶα�������ѡ			
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
			 * ɾ����
			 * @param name ������
			 */
			void			deleteItem(const tchar* name);

			/**
			 * ����schema id������޸Ļ�ɾ��������Ҫ����ID �����Ա���IDһ��
			 */
			void			updateID();

			/**
			 * ��������Ϊname��Schema��Ϣ
			 * @param name schema����
			 */
			CSchemaItem*	findSchema(const tchar* name);

			/**
			 * ��ȡShema Item
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
			 * װ��schema�ļ�
			 * @param schemaFile �ļ�·��
			 * schema file format:
			 * <document_schema>
			 *
			 *		<!-- �ֶε�schema���� -->
			 *		<schemaitem></schemaitem>
			 * </document_schema>
			 */
			void	loadSchema(const tchar*	schemaFile);

			/**
			 * ��XMLƬ����װ��schema��Ϣ
			 * @param pXMLSchema XML��SchemaƬ��
			 */
			void	loadSchema(CXMLElement* pXMLSchema);

			/**
			 * ����schema��Ϣ���ļ�
			 * @param schemaFile �ļ�·��
			 */
			void	saveSchema(const tchar*	schemaFile);

			/**
			 * ����schema��Ϣ��XMLƬ��
			 * @param pXMLSchema XMLƬ��
			 */
			void	saveSchema(CXMLElement* pXMLSchema);

			/**
			 * ���schema��Ϣ
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
