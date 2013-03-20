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
// Created	: 2007/5/25
//

#ifndef _GUARD_H
#define _GUARD_H

#include "Lockable.h"

namespace firtex
{
	namespace thread
	{
		class Lock
		{
		public:
			/// Constructor stores a reference to the
			/// base class.
			Lock (CLockable &l)
				: lock_ (l)
			{
			};

			/// Acquire the lock by forwarding to the
			/// polymorphic acquire() method.
			void acquire (void) { lock_.acquire (); }

			/// Release the lock by forwarding to the
			/// polymorphic release() method.
			void release (void) { lock_.release (); }
		private:
			// Maintain a reference to the polymorphic lock.
			CLockable &lock_;
		};

		template <class LOCK>
		class Guard 
		{
		public:
			Guard (LOCK &lock)
				: lock_ (lock) 
			{
				lock_.acquire();
			}
			// Release the lock when the guard goes out of scope.xc
			~Guard (void) 
			{
				lock_.release ();
			}
		private:
			// Reference to the lock we¡¯re managing.
			LOCK &lock_;
		};		
	}
}
#endif

