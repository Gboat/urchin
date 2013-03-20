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
				tstring		name;		///������
				tstring		value;		///����ֵ
			};
		public:
			CParamParser();
			~CParamParser();
		public:
			/**
			 * ���������ַ�������ʽ��������=����ֵ;������=����ֵ;..
			 * @param szParamString �����ַ���
			 * @param szSep ��������ķָ�����Ĭ��Ϊ";"
			 * @param szEq �������Ͳ���ֵ�ķָ�����Ĭ��Ϊ"="
			 * @return true:�����ɹ� false:����ʧ��
			 */
			bool	parse(const tchar* szParamString,const tchar* szSep = _T(";"),const tchar* szEq = _T("="));

			/**
			 * ���������ַ�������ʽ������1;����2;..
			 * @param szParamString �����ַ���
			 * @param szSep ��������ķָ�����Ĭ��Ϊ";"			 
			 * @return true:�����ɹ� false:����ʧ��
			 */
			bool	parse2(const tchar* szParamString,const tchar* szSep = _T(":"));

			/** 
			 * ��ȡ��������
			 */
			size_t	getParamCount(){return m_items.size();}

			/**
			 * ��ȡ����ֵ�������ڵ���{@link parse()}�ɹ������
			 * @param name ��������
			 * @param value ����ֵ
			 * @return true:��ȡ�ɹ� false:��ȡʧ�ܣ����������������
			 */
			bool	getParam(const tchar* name,int32_t& value);
			bool	getParam(const tchar* name,int64_t& value);
			bool	getParam(const tchar* name,float& value);
			bool	getParam(const tchar* name,tstring& value);


			/**
			 * ��ȡ����ֵ�������ڵ���{@link parse()}�ɹ������
			 * @param _off �ڼ�������ֵ
			 * @param value ������
			 * @param value ����ֵ
			 * @return true:��ȡ�ɹ� false:��ȡʧ�ܣ����������������
			 */
			bool	getParam(size_t _off,tstring& name,tstring& value);

			/**
			 * ��ȡ����ֵ�������ڵ���{@link parse2()}�ɹ������
			 * @param _off �ڼ�������ֵ
			 * @param value ����ֵ
			 * @return true:��ȡ�ɹ� false:��ȡʧ�ܣ����������������
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
