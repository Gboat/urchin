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
// Created	: 2007/1/7
//
#include "utility/ParamParser.h"
#include "utility/Config.h"
#include "utility/StringUtils.h"

namespace firtex
{
	namespace utility
	{
		//////////////////////////////////////////////////////////////////////////
		///CParamParser
		CParamParser::CParamParser()
		{

		}
		CParamParser::~CParamParser()
		{
			vector<Item*>::iterator iter = m_items.begin();
			while(iter != m_items.end())
			{
				delete (*iter);
				iter++;
			}
			m_items.clear();
		}	

		bool CParamParser::parse(const tchar* szParamString,const tchar* szSep,const tchar* szEq)
		{
			tstring sParam = szParamString;
			vector<tstring> params = firtex::string_utils::split(sParam,szSep);
			if(params.size() <= 0)
				return false;
			bool bSuc = false;
			vector<tstring>::iterator iter = params.begin();
			while(iter != params.end())
			{
				vector<tstring> item = firtex::string_utils::split(*iter,szEq);
				if(item.size() != 2)
				{
					iter++;
					continue;
				}
				bSuc = true;
				Item*  pItem = new Item(item[0],item[1]);
				m_items.push_back(pItem);
				iter++;
			}
			return bSuc;
		}
		bool CParamParser::parse2(const tchar* szParamString,const tchar* szSep)
		{
			tstring sParam = szParamString;
			vector<tstring> params = firtex::string_utils::split(sParam,szSep);
			if(params.size() <= 0)
				return false;			
			vector<tstring>::iterator iter = params.begin();
			while(iter != params.end())
			{							
				Item*  pItem = new Item(*iter,*iter);
				m_items.push_back(pItem);
				iter++;
			}
			return true;
		}


		bool CParamParser::getParam(const tchar* name,int32_t& value)
		{
			Item* pItem = getItem(name);
			if(!pItem)
				return false;
			try
			{
				value = _ttoi(pItem->value.c_str());
			}
			catch(...)
			{
				return false;
			}			
			return true;
		}
		bool CParamParser::getParam(const tchar* name,int64_t& value)
		{
			Item* pItem = getItem(name);
			if(!pItem)
				return false;
			try
			{
				value = _ttoi64(pItem->value.c_str());
			}
			catch(...)
			{
				return false;
			}			
			return true;
		}
		bool CParamParser::getParam(const tchar* name,float& value)
		{
			Item* pItem = getItem(name);
			if(!pItem)
				return false;
			try
			{
				value = (float)_tstof(pItem->value.c_str());
			}
			catch(...)
			{
				return false;
			}			
			return true;
		}
		bool CParamParser::getParam(const tchar* name,tstring& value)
		{
			Item* pItem = getItem(name);
			if(!pItem)
				return false;
			value = pItem->value;
			return true;
		}
		bool CParamParser::getParam(size_t _off,tstring& name,tstring& value)
		{
			if(m_items.size() <= _off)
				return false;
			name = m_items[_off]->getName();
			value = m_items[_off]->getValue();
			return true;
		}
		bool CParamParser::getParam2(size_t _off,tstring& value)
		{
			if(m_items.size() <= _off)
				return false;
			value = m_items[_off]->getName();			
			return true;
		}

		CParamParser::Item* CParamParser::getItem(const tchar* name)
		{
			vector<Item*>::iterator iter = m_items.begin();
			while(iter != m_items.end())
			{
				if(!_tcsicmp(name,(*iter)->name.c_str()))
					return (*iter);
				iter++;
			}
			return NULL;
		}
	}
}
