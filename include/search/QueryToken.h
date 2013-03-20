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
#ifndef _QUERYTOKEN_H
#define _QUERYTOKEN_H

#include <string>
using namespace std;
#include "QueryParserConstants.h"

namespace firtex
{
	namespace search
	{
		class CQueryToken
		{			
		public:					
			CQueryToken(const string& value, const int start, const int end, const QueryTokenTypes type);
			CQueryToken(const string& value, const QueryTokenTypes type);
			CQueryToken(const QueryTokenTypes type);
			~CQueryToken();
		public:
			string		m_value;
			const int	m_start;			
			int			m_end;
			const QueryTokenTypes m_type;
		};
	}
}

#endif
