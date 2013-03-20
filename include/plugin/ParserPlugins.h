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
#ifndef _PARSERPLUGINS_H
#define _PARSERPLUGINS_H

#include "FirteXCOM.h"
#include "../utility/StdHeader.h"
#include "../parser/Parser.h"
#include <map>

namespace firtex
{
	namespace plugin
	{		
		class CPlugin;
		class CParserPlugin : public firtex::parser::CParser
		{
		public:
			CParserPlugin(IParserPlugin* pParserInterface);
			virtual ~CParserPlugin();
		public:			
			/**
			 * set parameter of parser
			 * @param pszParam parameter string, format: [field.]param_name=param_value;[field.]param_name2=param_value2
			 */
			void		setParam(const tchar* pszParam);

			/**
			 * get parameter of parser
			 * @param pszParamName parameter name
			 * @param paramValue parameter value
			 */
			bool		getParam(const tchar* pszParamName,tstring& paramValue);
		protected:
			/**
			 * 解析文档,子类必须实现此接口
			 * @param pDocInput 文档输入对象
			 * @param pResultDoc 文档对象
			 * @return 解释成功则返回true,否则false
			 */			
			bool		parseInternal(firtex::document::CDocumentInput* pDocInput,document::CDocument* pResultDoc);

			/**
			 * define the schema of document, MUST implement by sub-class
			 * @param pSchema document schema 
			 * @return schema id of main text
			 */			
			schemaid_t	defineSchema(firtex::document::CDocumentSchema* pSchema);

			/**
			 * 获取这个parser可以处理的文件类型
			 */
			tstring	getDocType();		
		protected:
			IParserPlugin*		m_pParserInterface;

			friend class CParserPlugins;
		};

		class CParserPlugins
		{
		public:
			CParserPlugins(void);
			virtual~CParserPlugins(void);
		public:	
			/**
			 * create a parser plugin by identifier
			 * @param identifier identifier of parser
			 * @return a parser object			 
			 */
			CParserPlugin*	getParserByIdentifier(const tchar* identifier);

			/**
			 * load a parser plugin
			 * @param pPlugin the plugin 
			 * @return the plugin is loaded or not
			 */
			bool			loadPlugin(CPlugin* pPlugin);			

			/**
			 * clear all plugins
			 */
			void			clear();
		protected:
			map<tstring,IParserPlugin*>	m_pluginMap;
			bool				m_bCOMInit;
		};
	}
}


#endif

