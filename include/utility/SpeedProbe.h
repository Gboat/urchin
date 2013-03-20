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
// Created	: 2005/10/19
//
#ifndef __SPEEDPROBE_H
#define __SPEEDPROBE_H

#include "StdHeader.h"
#include <string>
#include <time.h>

using namespace std;


namespace firtex
{
	namespace utility
	{
		class CSpeedProbe
		{
		public:
			CSpeedProbe(void);				
			virtual ~CSpeedProbe(void);
		public:	
		public:
			/** 
			 * begin to record
			 */ 
			clock_t	begin();

			/** 
			 * end record
			 */ 
			clock_t end();						

			/**
			 * get the time span (second),the time elapsed between begin() and end()
			 * @return time span (second)
			 */
			double	timeSpan();			

			/**
			 * get total time span (second)
			 * @return total time span (second)
			 */
			double	totalTimeSpan();
		protected:
			clock_t		m_lBegin;
			clock_t		m_lEnd;
			clock_t		m_lTotalElapsed;
		};
	}
}


#endif

