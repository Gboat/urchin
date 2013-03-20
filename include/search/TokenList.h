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
// Created	: 2005/12/28
//
#ifndef _TOKENLIST_H
#define _TOKENLIST_H

#include <vector>
#include "QueryToken.h"

using namespace std;

namespace firtex
{
	namespace search
	{
		class CTokenList
		{		
		public:
			CTokenList();
			~CTokenList();

			void add(CQueryToken* token);

			void push(CQueryToken* token);

			CQueryToken* peek();

			CQueryToken* extract();

			int			count();
		private:
			vector<CQueryToken*>m_tokens;
		};

	}
}



#endif
