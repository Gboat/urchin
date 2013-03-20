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
			 * �����ļ���ʽ˵����( (.)��ʾ������֣�(?)��ʾ����0�λ�1�Σ�(+)��ʾ����1�λ���,(*)��ʾ����0�λ���)
			 *
			 * <argument>
			 *		<!--�ĵ������Ͷ���-->
			 *		<collection>
			 *			<!-- collection����,��ȡdirectory,filelist,firtex,trec. (.) -->
			 * 			<identifier></identifier>
			 *			<!-- ���� -->
			 *			<parameter></parameter>
			 *		</collection>
			 *
			 *		<!--�ĵ���ʽ����������. (.) -->
			 *		<parser>
			 *			<!-- �ĵ���ʽ���������ͣ���ȡ��(parser.)plain,(parser.)html,(parser.)pdf �� (.) -->
			 * 			<identifier></identifier>
			 *		</parser>
			 *
			 *		<!-- �ĵ����ݷ��������� (.) -->
			 *		<analyzer>
			 *			<!-- Ĭ�Ͻ�������ʶ (.)-->
			 * 			<identifier></identifier>
			 *
			 *			<!-- Analyzer�Ĳ�������ʽʾ����stopword=standard:stopword.stf;stemmer=snowball:english:utf8;.... (?) -->
			 * 			<parameter></parameter>
			 *			 
			 *		</analyzer>
			 *
			 *		<!-- ������������ (.) -->
			 *		<index>
			 *			<!-- �����ĵ�Դ·�� (.) -->
			 * 			<source></source>
			 *
			 *			<!-- �������λ�� (.) -->
			 * 			<indexlocation></indexlocation>
			 *		
			 *			<!-- �������𣬿�ȡ0,1��2. (?)
			 *			 0��WORD_LEVEL,���������ĵ�ID����Ƶ�ʹ�λ��
			 *			 1��DOCUMENT_LEVEL�����������ĵ�ID����Ƶ��Ϣ
			 *			 2��DOCUMENT_LEVEL_0�������������ĵ�ID��Ϣ 
			 *			-->
			 *			<level></level>
			 *
			 *			<!-- �����Ĵ�ȡ��ʽ����ȡreadonly/write/readwrite/append/readappend,�μ�{@link CIndex} (.)-->
			 * 			<access_mode></access_mode>
			 *
			 *			<!-- ��ƪ�ĵ�������������� (.) -->
			 * 			<maxterms></maxterms>
			 *
			 *			<!-- ���������С (.) -->
			 * 			<memory></memory>
			 *
			 *			<!-- �ĵ�������� (.) -->
			 *			<cacheddocs></cacheddocs>
			 *		</index>
			 *
			 *		<!--�����ϲ����Զ���-->
			 *		<merge_strategy>
			 *
			 *			<!-- �����ϲ����� (.) -->
			 *			<strategy></strategy>
			 *
			 *			<!-- ��Ӧ�ò��Եĺϲ�����,������ʽ��������=����ֵ;������=����ֵ;...���������ͨ��;�ֿ� (.) -->
			 * 			<param></param>
			 *
			 *			<!-- �����ĵ���������ֵ����ɾ�����ĵ����ﵽ�������ֵʱ�Զ����������ĵ����ա�������ʽ��n=[deleted docs] �� r=[deleted docs/all docs] (?) -->
			 *			<gc_threshold></gc_threshold>
			 *		</merge_strategy>			
			 *			 
			 *		<!-- log���𣬿�ȡ:"default_level","enable_all","disable_all","fatal","err","warn","info","dbg" (.) -->
			 *		<log></log>
			 *
			 *		<!-- �����ĵ�ģʽ����,��ʽ˵���μ�document/DocumentSchema.h˵��.(?) -->
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
				tstring	identifier;			///analyzer��ʶ��chineseanalyzer
				tstring parameter;			///parameter of parser,format:param_name=parm_value;param_name=parm_value,e.g. stemmer=snowball;stopword=stopword file;lowercase=true
			};
			sanalyzer analyzer;

			struct sindex
			{
				tstring	source;				///Դ�ļ�·�� [����]
				tstring	indexlocation;		///�������·�� [����]
				int32_t level;				///��������,Ĭ��Ϊ2,��WORD_LEVEL���� [��ѡ]
				tstring	accessmode;			///������ȡ��ʽ��[����]				
				int32_t	maxterms;			///����������������û�ṩ��ʹ��{@link #CConfig}�ṩ�Ĳ�����[��ѡ]
				int64_t	memory;				///����ʹ�õ��ڴ�Cache��С�����û�ṩ��ʹ��{@link #CConfig}�ṩ�Ĳ�����[��ѡ]
				int32_t	cachedocs;			///����ʱ������ĵ������ɼ��ٺ������ô��ۣ�����Ҫ������ڴ�[��ѡ]
			};
			sindex index;

			struct smergestrategy 
			{
				tstring	strategy;			///�����ϲ����ԣ��μ�{@link CIndexWriter}
				tstring	param;				///�ϲ����ԵĲ���,������ʽ��������=����ֵ���������ͨ���ո�ֿ�
				tstring gc_threshold;		///�����ĵ���������ֵ����ɾ�����ĵ����ﵽ�������ֵʱ�Զ����������ĵ����ա�������ʽ��n=[deleted docs] �� r=[deleted docs/all docs]
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
