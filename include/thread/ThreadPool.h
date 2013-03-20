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
#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include "Runnalble.h"
#include "Thread.h"
#include <vector>

using namespace std;

namespace firtex
{
	namespace thread
	{
		class CThreadPool
		{
		public:
			CThreadPool(void);
			virtual ~CThreadPool(void);
		public:
			virtual 	void	add(CRunnalble* aRun);
			virtual		void	wait();

			virtual		void	clear();
		protected:
			vector<CThread*>m_threads;
		};
	}
}

#endif
