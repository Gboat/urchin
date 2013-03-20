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

#ifndef _FIRTEX_THREADS_H
#define _FIRTEX_THREADS_H

#include "Lockable.h"
#include "Mutex.h"
#include "Guard.h"
#include "Thread.h"

#ifdef FIRTEX_MULTITHREAD	
	#define SCOPED_LOCK(theLock) firtex::thread::Guard<firtex::thread::CLockable> theGuard(theLock);
	#define SCOPED_LOCK2(theLock,n) firtex::thread::Guard<firtex::thread::CLockable> theGuard##n(theLock);
	
	#define DEFINE_MUTEX(theMutex) 	firtex::thread::CMutex theMutex;

	#define STATIC_DEFINE_MUTEX(theMutex) 	static firtex::thread::CMutex theMutex;
	#define STATIC_DECLARE_MUTEX(theClass,theMutex) firtex::thread::CMutex theClass::theMutex;

class LOCK_BASE
{
public:
	LOCK_BASE(){}
	virtual ~LOCK_BASE(){}
private:
	LOCK_BASE(const LOCK_BASE&);
	const LOCK_BASE& operator=(const LOCK_BASE&);
public:
	firtex::thread::CMutex LOCK;
};
	
#else
	#define SCOPED_LOCK(theLock)
	#define SCOPED_LOCK2(theLock,n)

	#define DEFINE_MUTEX(theMutex) 	firtex::thread::CNullMutex theMutex;

	#define STATIC_DEFINE_MUTEX(theMutex) 	static firtex::thread::CNullMutex theMutex;

	#define STATIC_DECLARE_MUTEX(theClass,theMutex) firtex::thread::CNullMutex theClass::theMutex;

class LOCK_BASE
{
public:
	LOCK_BASE(){}
	virtual ~LOCK_BASE(){}
private:
	LOCK_BASE(const LOCK_BASE&);
	const LOCK_BASE& operator=(const LOCK_BASE&);
public:
	firtex::thread::CNullMutex LOCK;
};

#endif

#define FIRTEX_LOCK public LOCK_BASE


#endif
