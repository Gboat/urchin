/************************************************************************************/
/* Description	: FirteX software													*/
/* Author		: ¹ùÈð½Ü															*/
/* Version		: Ver1.0															*/
/* Date			: 2005/10/19														*/
/*																					*/
/* Copyright(C) 2005--2006 Software Division,Institute of Computing Technology,CAS  */
/*																					*/
/* Contact infomation:																*/
/*        ruijieguo@software.ict.ac.cn												*/
/*																					*/
/*																					*/
/************************************************************************************/
#include "utility/SpeedProbe.h"
#include "utility/Exception.h"

#include <fstream>
#include <time.h>

namespace firtex
{
	namespace utility
	{
		CSpeedProbe::CSpeedProbe(void)
		{
			m_lBegin = m_lEnd = m_lTotalElapsed = 0;
		}
		
		CSpeedProbe::~CSpeedProbe(void)
		{
		}

		clock_t CSpeedProbe::begin()
		{			
			m_lBegin = clock();
			return m_lBegin;
		}
		clock_t CSpeedProbe::end()
		{
			m_lEnd = clock();
			if(m_lEnd - m_lBegin > 0)
				m_lTotalElapsed += (m_lEnd - m_lBegin);
			return m_lEnd;
		}

		double CSpeedProbe::timeSpan()
		{
			return (double)(m_lEnd - m_lBegin)/CLOCKS_PER_SEC;
		}
		double CSpeedProbe::totalTimeSpan()
		{
			return (double)(m_lTotalElapsed)/CLOCKS_PER_SEC;
		}		
	}
}
