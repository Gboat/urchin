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
// Created	: 2007/1/17
//
#ifndef _PARAMPARSER_H
#define _PARAMPARSER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdHeader.h"
#include <vector>
using namespace std;

namespace firtex
{
	namespace utility
	{		
		class CParamParser
		{
		public:
			class Item
			{
			public:
				Item(const tstring& _name,const tstring& _value)
					:name(_name)
					,value(_value)
				{}
			public:
				const tchar* getName(){return name.c_str();}
				const tchar* getValue(){return value.c_str();}
			public:
				tstring		name;		///参数名
				tstring		value;		///参数值
			};
		public:
			CParamParser();
			~CParamParser();
		public:
			/**
			 * 解析参数字符串，格式：参数名=参数值;参数名=参数值;..
			 * @param szParamString 参数字符串
			 * @param szSep 多个参数的分隔符，默认为";"
			 * @param szEq 参数名和参数值的分隔符，默认为"="
			 * @return true:解析成功 false:解析失败
			 */
			bool	parse(const tchar* szParamString,const tchar* szSep = _T(";"),const tchar* szEq = _T("="));

			/**
			 * 解析参数字符串，格式：参数1;参数2;..
			 * @param szParamString 参数字符串
			 * @param szSep 多个参数的分隔符，默认为";"			 
			 * @return true:解析成功 false:解析失败
			 */
			bool	parse2(const tchar* szParamString,const tchar* szSep = _T(":"));

			/** 
			 * 获取参数总数
			 */
			size_t	getParamCount(){return m_items.size();}

			/**
			 * 获取参数值，必须在调用{@link parse()}成功后调用
			 * @param name 参数名称
			 * @param value 参数值
			 * @return true:获取成功 false:获取失败，不存在这个参数名
			 */
			bool	getParam(const tchar* name,int32_t& value);
			bool	getParam(const tchar* name,int64_t& value);
			bool	getParam(const tchar* name,float& value);
			bool	getParam(const tchar* name,tstring& value);


			/**
			 * 获取参数值，必须在调用{@link parse()}成功后调用
			 * @param _off 第几个参数值
			 * @param value 参数名
			 * @param value 参数值
			 * @return true:获取成功 false:获取失败，不存在这个参数名
			 */
			bool	getParam(size_t _off,tstring& name,tstring& value);

			/**
			 * 获取参数值，必须在调用{@link parse2()}成功后调用
			 * @param _off 第几个参数值
			 * @param value 参数值
			 * @return true:获取成功 false:获取失败，不存在这个参数名
			 */
			bool	getParam2(size_t _off,tstring& value);
		protected:
			CParamParser::Item* getItem(const tchar* name);
		protected:
			vector<Item*> m_items;
		};
	}
}

#endif
