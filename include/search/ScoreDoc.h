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
#ifndef _SCOREDOC_H
#define _SCOREDOC_H
#include "../utility/typedefs.h"
using namespace firtex;
#include <queue>
using namespace std;


namespace firtex
{
	namespace search
	{
		class CScoreDoc
		{
		public:

			CScoreDoc(docid_t did,float score):m_docID(did),m_fScore(score)
			{				
			}

			~CScoreDoc(void)
			{
			}		
			
		public:
			docid_t		m_docID;
			float		m_fScore;
		};
	}
}


#endif
