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
// Email	: ruijieguo@software.ict.ac.cn
// Created	: 2007/6/09
//

#ifndef __STARTER_H
#define __STARTER_H

namespace firtex
{
	namespace utility
	{
		class Starter
		{
		public:
			Starter(void);
			virtual ~Starter(void);
		public:
			/**
			 * initialize global variables
			 */
			virtual void	initialize();

			/**
			 * clean global resources
			 */
			virtual	void	terminate();
		};
	}
}


#endif
