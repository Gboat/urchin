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
// Created	: 2007/3/8
//

#include "analyzer/LowercaseAnteFilter.h"

namespace firtex
{
	namespace analyzer
	{
		CLowercaseAnteFilter::CLowercaseAnteFilter(void)			
		{
		}
		CLowercaseAnteFilter::CLowercaseAnteFilter(CTokenFilter* pPreFilter)
			:CTokenFilter(pPreFilter)
		{
		}

		CLowercaseAnteFilter::~CLowercaseAnteFilter(void)
		{
		}

		CTokens* CLowercaseAnteFilter::filterInternal(CTokens* pInputTokens)
		{
			/*size_t length;
			char* szText;
			register char* pBegin;
			char* pEnd ;
			char cDelt = 'a' - 'A';
			while (!pInputReader->isEof())
			{
				szText = pInputReader->readWithoutCopy(length);
				pBegin = szText;
				pEnd = szText + length;
				while (pBegin < pEnd)
				{
					if(*pBegin >= 'A' && *pBegin <= 'Z')
						*pBegin += cDelt;
					pBegin++;
				}
			}			
			pInputReader->seek(0);
			return pInputReader;*/
			return pInputTokens;
		}
	}
}

