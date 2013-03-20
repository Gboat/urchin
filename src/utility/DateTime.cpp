//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: 郭瑞杰(GuoRuijie)
// Email	: ruijieguo@software.ict.ac.cn,ruijieguo@gmail.com
// Created	: 2006/5/19
//
#include "utility/DateTime.h"

#include <assert.h>
#include <time.h>

namespace firtex
{
	namespace utility
	{
		/////////////////////////////////////////////////////////////////////////////
		// CDateTimeSpan
		/////////////////////////////////////////////////////////////////////////////

		CDateTimeSpan::CDateTimeSpan() throw() :	m_timeSpan(0)
		{
		}

		CDateTimeSpan::CDateTimeSpan( int64_t time ) throw() :	m_timeSpan( time )
		{
		}

		CDateTimeSpan::CDateTimeSpan(int nDays, int nHours, int nMins, int nSecs) throw()
		{
			m_timeSpan = nSecs + 60* (nMins + 60* (nHours + int64_t(24) * (int64_t)nDays));
		}

		int64_t CDateTimeSpan::getDays() const throw()
		{
			return( m_timeSpan/(24*3600) );
		}

		int64_t CDateTimeSpan::getTotalHours() const throw()
		{
			return( m_timeSpan/3600 );
		}

		int64_t CDateTimeSpan::getHours() const throw()
		{
			return( int64_t( getTotalHours()-(getDays()*24) ) );
		}

		int64_t CDateTimeSpan::getTotalMinutes() const throw()
		{
			return( m_timeSpan/60 );
		}

		int64_t CDateTimeSpan::getMinutes() const throw()
		{
			return( int64_t( getTotalMinutes()-(getTotalHours()*60) ) );
		}

		int64_t CDateTimeSpan::getTotalSeconds() const throw()
		{
			return( m_timeSpan );
		}

		int64_t CDateTimeSpan::getSeconds() const throw()
		{
			return( int64_t( getTotalSeconds()-(getTotalMinutes()*60) ) );
		}

		int64_t CDateTimeSpan::getSpan() const throw()
		{
			return( m_timeSpan );
		}

		CDateTimeSpan CDateTimeSpan::operator+( CDateTimeSpan span ) const throw()
		{
			return( CDateTimeSpan( m_timeSpan+span.m_timeSpan ) );
		}

		CDateTimeSpan CDateTimeSpan::operator-( CDateTimeSpan span ) const throw()
		{
			return( CDateTimeSpan( m_timeSpan-span.m_timeSpan ) );
		}

		CDateTimeSpan& CDateTimeSpan::operator+=( CDateTimeSpan span ) throw()
		{
			m_timeSpan += span.m_timeSpan;
			return( *this );
		}

		CDateTimeSpan& CDateTimeSpan::operator-=( CDateTimeSpan span ) throw()
		{
			m_timeSpan -= span.m_timeSpan;
			return( *this );
		}

		bool CDateTimeSpan::operator==( CDateTimeSpan span ) const throw()
		{
			return( m_timeSpan == span.m_timeSpan );
		}

		bool CDateTimeSpan::operator!=( CDateTimeSpan span ) const throw()
		{
			return( m_timeSpan != span.m_timeSpan );
		}

		bool CDateTimeSpan::operator<( CDateTimeSpan span ) const throw()
		{
			return( m_timeSpan < span.m_timeSpan );
		}

		bool CDateTimeSpan::operator>( CDateTimeSpan span ) const throw()
		{
			return( m_timeSpan > span.m_timeSpan );
		}

		bool CDateTimeSpan::operator<=( CDateTimeSpan span ) const throw()
		{
			return( m_timeSpan <= span.m_timeSpan );
		}

		bool CDateTimeSpan::operator>=( CDateTimeSpan span ) const throw()
		{
			return( m_timeSpan >= span.m_timeSpan );
		}
		/////////////////////////////////////////////////////////////////////////////
		// CDateTime
		/////////////////////////////////////////////////////////////////////////////

		CDateTime  CDateTime::getCurrentTime() throw()
		{
			return( CDateTime( ::_time64( NULL ) ) );
		}

		CDateTime::CDateTime() throw() :	m_time(0)
		{
		}

		CDateTime::CDateTime( int64_t time )  throw(): m_time( time )
		{
		}

		CDateTime::CDateTime(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec,int nDST)
		{
			struct tm atm;
			atm.tm_sec = nSec;
			atm.tm_min = nMin;
			atm.tm_hour = nHour;
			assert(nDay >= 1 && nDay <= 31);
			atm.tm_mday = nDay;
			assert(nMonth >= 1 && nMonth <= 12);
			atm.tm_mon = nMonth - 1;        // tm_mon is 0 based
			assert(nYear >= 1900);
			atm.tm_year = nYear - 1900;     // tm_year is 1900 based
			atm.tm_isdst = nDST;
#ifdef WIN32
			m_time = _mktime64(&atm);
#else
			m_time = mktime(&atm);
#endif
			
			FIRTEX_ASSERT((m_time != -1),_T("CDateTime:error in constructor."));       // indicates an illegal input time

			if(m_time == -1)
				FIRTEX_THROW2(BAD_PARAMETER_ERROR,"CDateTime::CDateTime(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec,int nDST)");
		}

		CDateTime& CDateTime::operator=( int64_t time ) throw()
		{
			m_time = time;

			return( *this );
		}

		CDateTime& CDateTime::operator+=( CDateTimeSpan span ) throw()
		{
			m_time += span.getSpan();

			return( *this );
		}

		CDateTime& CDateTime::operator-=( CDateTimeSpan span ) throw()
		{
			m_time -= span.getSpan();

			return( *this );
		}

		CDateTimeSpan CDateTime::operator-( CDateTime time ) const throw()
		{
			return( CDateTimeSpan( m_time-time.m_time ) );
		}

		CDateTime CDateTime::operator-( CDateTimeSpan span ) const throw()
		{
			return( CDateTime( m_time-span.getSpan() ) );
		}

		CDateTime CDateTime::operator+( CDateTimeSpan span ) const throw()
		{
			return( CDateTime( m_time+span.getSpan() ) );
		}

		bool CDateTime::operator==( CDateTime time ) const throw()
		{
			return( m_time == time.m_time );
		}

		bool CDateTime::operator!=( CDateTime time ) const throw()
		{
			return( m_time != time.m_time );
		}

		bool CDateTime::operator<( CDateTime time ) const throw()
		{
			return( m_time < time.m_time );
		}

		bool CDateTime::operator>( CDateTime time ) const throw()
		{
			return( m_time > time.m_time );
		}

		bool CDateTime::operator<=( CDateTime time ) const throw()
		{
			return( m_time <= time.m_time );
		}

		bool CDateTime::operator>=( CDateTime time ) const throw()
		{
			return( m_time >= time.m_time );
		}
		int64_t CDateTime::getTime() const throw()
		{
			return( m_time );
		}

#ifdef WIN32
		struct tm* CDateTime::getGmtTm(struct tm* ptm) const throw()
		{
			if (ptm != NULL)
			{
				struct tm * ptmTemp;
				ptmTemp = _gmtime64(&m_time);

				if(ptmTemp == NULL)
					return NULL;

				*ptm = *ptmTemp;

				return ptm;
			}
			else
				return _gmtime64(&m_time);
		}

		struct tm* CDateTime::getLocalTm(struct tm* ptm) const throw()
		{
			if (ptm != NULL)
			{
				struct tm* ptmTemp = _localtime64(&m_time);
				if (ptmTemp == NULL)
					return NULL;    // indicates the m_time was not initialized!

				*ptm = *ptmTemp;
				return ptm;
			}
			else
				return _localtime64(&m_time);
		}
		
		int CDateTime::getYear() const throw()
		{ 
			struct tm * ptm;
			ptm = getLocalTm();
			return ptm ? (ptm->tm_year) + 1900 : 0 ; 
		}

		int CDateTime::getMonth() const throw()
		{ 
			struct tm * ptm;
			ptm = getLocalTm();
			return ptm ? ptm->tm_mon + 1 : 0;
		}

		int CDateTime::getDay() const throw()
		{
			struct tm * ptm;
			ptm = getLocalTm();
			return ptm ? ptm->tm_mday : 0 ; 
		}

		int CDateTime::getHour() const throw()
		{
			struct tm * ptm;
			ptm = getLocalTm();
			return ptm ? ptm->tm_hour : -1 ; 
		}

		int CDateTime::getMinute() const throw()
		{ 
			struct tm * ptm;
			ptm = getLocalTm();
			return ptm ? ptm->tm_min : -1 ; 
		}

		int CDateTime::getSecond() const throw()
		{ 
			struct tm * ptm;
			ptm = getLocalTm();
			return ptm ? ptm->tm_sec : -1 ;
		}

		int CDateTime::getDayOfWeek() const throw()
		{ 
			struct tm * ptm;
			ptm = getLocalTm();
			return ptm ? ptm->tm_wday + 1 : 0 ;
		}
#endif
		tstring CDateTime::format(const tstring& sformat)const
		{
			if(sformat == _T(""))
				return _T("");

			char szBuffer[maxTimeBufferSize];

#ifdef WIN32
			struct tm* ptmTemp = _localtime64(&m_time);
#else
			time_t t = (time_t)m_time;
			struct tm* ptmTemp = localtime(&t);

#endif
			
			if (ptmTemp == NULL ||	!_tcsftime(szBuffer, maxTimeBufferSize, sformat.c_str(),ptmTemp))
				szBuffer[0] = '\0';
			return szBuffer;
		}
		tstring CDateTime::format(int64_t t,const tstring& sformat)
		{
			if(sformat == _T(""))
				return _T("");

			char szBuffer[maxTimeBufferSize];

#ifdef WIN32
			struct tm* ptmTemp = _localtime64(&t);
#else
			time_t tt = (time_t)t;
			struct tm* ptmTemp = localtime(&tt);

#endif
			if (ptmTemp == NULL ||	!_tcsftime(szBuffer, maxTimeBufferSize, sformat.c_str(),ptmTemp))
				szBuffer[0] = '\0';
			return szBuffer;
		}

		ostream& operator<<(ostream& os, const CDateTime& d)
		{
			//年/月/日 时:分:秒
		//	os << d.getYear() << '/' << d.getMonth()<< '/' << d.getDay() << ' ' << d.getHour() << ':' << d.getMinute() << ':' << d.getSecond();
			//TODO:
			return os;
		}

		istream& operator>>(istream& is, CDateTime& d)
		{
			int y,m,dd,h,min,s;
			char slash;
			is >> y >> slash >> m >> slash >> dd >> slash >> h >> slash >> min >> slash >> s ;
			//年/月/日 时:分:秒
			return is;
		}		

	}
}
