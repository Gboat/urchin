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
// Created	: 2007/5/27
//

#ifndef _NONCOPYABLE_H
#define _NONCOPYABLE_H

namespace firtex
{
	namespace thread
	{
		class CNonCopyable
		{
			//! Restrict the copy constructor
			CNonCopyable(const CNonCopyable&);

			//! Restrict the assignment operator
			const CNonCopyable& operator=(const CNonCopyable&);

		protected:

			//! Create a CNonCopyable object
			CNonCopyable() { }

			//! Destroy a CNonCopyable object
			~CNonCopyable() { }

		}; /* CNonCopyable */
	}
}


#endif
