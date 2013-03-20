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
// Created	: 2005/11/09
//

#ifndef _RUNNALBLE_H
#define _RUNNALBLE_H

namespace firtex
{
	namespace thread
	{
		class CRunnalble
		{
		public:

			CRunnalble(void)
			{
			}

			virtual ~CRunnalble(void)
			{
			}
		public:
			virtual void run() = 0;
		};
	}
}

#endif
