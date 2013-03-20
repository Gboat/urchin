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
// Created	: 2006/5/19
//
#ifndef _DATETIME_H
#define _DATETIME_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "StdHeader.h"
#include <ostream>
#include <istream>
#include <string>

using namespace std;

namespace firtex
{
	namespace utility
	{
		class CDateTimeSpan
		{
		public:
			CDateTimeSpan() throw();
			CDateTimeSpan( int64_t time ) throw();
			CDateTimeSpan( int nDays, int nHours, int nMins, int nSecs ) throw();

			int64_t getDays() const throw();
			int64_t getTotalHours() const throw();
			int64_t	getHours() const throw();
			int64_t getTotalMinutes() const throw();
			int64_t	getMinutes() const throw();
			int64_t getTotalSeconds() const throw();
			int64_t	getSeconds() const throw();

			int64_t getSpan() const throw();

			CDateTimeSpan operator+( CDateTimeSpan span ) const throw();
			CDateTimeSpan operator-( CDateTimeSpan span ) const throw();
			CDateTimeSpan& operator+=( CDateTimeSpan span ) throw();
			CDateTimeSpan& operator-=( CDateTimeSpan span ) throw();
			bool operator==( CDateTimeSpan span ) const throw();
			bool operator!=( CDateTimeSpan span ) const throw();
			bool operator<( CDateTimeSpan span ) const throw();
			bool operator>( CDateTimeSpan span ) const throw();
			bool operator<=( CDateTimeSpan span ) const throw();
			bool operator>=( CDateTimeSpan span ) const throw();

		private:
			int64_t m_timeSpan;
		};


		class CDateTime
		{
		public:
			static CDateTime  getCurrentTime() throw();

			CDateTime() throw();
			CDateTime( int64_t time ) throw();
			CDateTime( int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec,int nDST = -1 );			

		public:
			CDateTime& operator=( int64_t time ) throw();

			CDateTime& operator+=( CDateTimeSpan span ) throw();
			CDateTime& operator-=( CDateTimeSpan span ) throw();

			CDateTimeSpan operator-( CDateTime time ) const throw();
			CDateTime operator-( CDateTimeSpan span ) const throw();
			CDateTime operator+( CDateTimeSpan span ) const throw();

			bool operator==( CDateTime time ) const throw();
			bool operator!=( CDateTime time ) const throw();
			bool operator<( CDateTime time ) const throw();
			bool operator>( CDateTime time ) const throw();
			bool operator<=( CDateTime time ) const throw();
			bool operator>=( CDateTime time ) const throw();

			int64_t		getTime() const throw();		struct tm*	getGmtTm( struct tm* ptm = NULL ) const throw();

#ifdef WIN32	
			struct tm*	getLocalTm( struct tm* ptm = NULL ) const throw();


			int getYear() const throw();
			int getMonth() const throw();
			int getDay() const throw();
			int getHour() const throw();
			int getMinute() const throw();
			int getSecond() const throw();
			int getDayOfWeek() const throw();

#endif
			tstring	format(const tstring& sformat = "%Y/%m/%d %H:%M:%S")const;
		
		public:
			static tstring	format(int64_t t,const tstring& sformat = "%Y/%m/%d %H:%M:%S");

			// Stream I/O operators
			friend ostream& operator<<(ostream& os, const CDateTime& d);

			friend istream& operator>>(istream& is, CDateTime& d);

			static int isleap(int y)
			{return y%4 == 0 && y%100 != 0 || y%400 == 0;}
		
		private:
			int64_t m_time;			
		};
		const int maxTimeBufferSize = 128;
		//////////////////////////////////////////////////////////////////////////
		//Inline functions	

	}
}

#endif
