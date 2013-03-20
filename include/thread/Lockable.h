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
// Created	: 2005/11/20
//
#ifndef _LOCKABLE_H
#define _LOCKABLE_H

namespace firtex
{
	namespace thread
	{
		class CLockable
		{
		public:
			CLockable(void){}
			virtual ~CLockable() {}
		public:
			virtual void acquire() = 0;

			virtual bool tryAcquire(unsigned long timeout) = 0;

			virtual void release() = 0;
		};
	}
}


#endif
