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
// Created	: 2007/3/7
//
#ifndef __SNOWBALL_H
#define __SNOWBALL_H


#ifdef WIN32
	#ifdef _DEBUG
		#pragma comment(lib,"SnowballD.lib")
		#pragma message("Automatically linking with SnowballD.lib") 
	#else
		#pragma comment(lib,"Snowball.lib")
		#pragma message("Automatically linking with Snowball.lib") 
	#endif
#endif


#include "SnowballFilter.h"


#endif
