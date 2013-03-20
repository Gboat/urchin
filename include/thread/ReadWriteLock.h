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
// Created	: 2007/6/7
//

#ifndef _READWRITELOCK_H
#define _READWRITELOCK_H

#include "thread/Lockable.h"
#include "thread/NonCopyable.h"

namespace firtex 
{
	namespace thread
	{
        class ReadWriteLock : public CNonCopyable
		{
		public:		
			ReadWriteLock() {}		
			virtual ~ReadWriteLock() {} 

			/**
			 * Get a reference to the read-only Lockable.
			 * @return <em>Lockable&</em> reference to a Lockable that provides read-only access.
			 */
			virtual CLockable& getReadLock() = 0;

			/**
			 * Get a reference to the read-write Lockable.
			 * @return <em>Lockable&</em> reference to a Lockable that provides read-write access.
			 */
			virtual CLockable& getWriteLock() = 0;
		};
	}
}

#endif
