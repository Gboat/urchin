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
// Created	: 2006/5/9
//
#include "search/QueryToken.h"

namespace firtex
{
	namespace search
	{
		CQueryToken::CQueryToken(const string& value, const int start, const int end, const QueryTokenTypes type):
		m_start(start),m_end(end),m_type(type),m_value(value)
		{
		}

		CQueryToken::~CQueryToken()
		{
		}
		CQueryToken::CQueryToken(const string& value, const QueryTokenTypes type):
		m_start(0),m_end(0),m_type(type),m_value(value)
		{
		}	
		CQueryToken::CQueryToken(const QueryTokenTypes type):m_start(0),m_end(0),m_type(type)
		{
		}
	}
}
