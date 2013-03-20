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
// Created	: 2006/9/11
//
#ifndef _CPCOM_UNIX_DATABASE_H_
#define _CPCOM_UNIX_DATABASE_H_

#include <pthread.h>

namespace firtex
{
	namespace com 
	{
		namespace database 
		{		
			FX_HRESULT Initialize();
			void    Uninitialize();			
			FX_HRESULT	lookupInprocServer32( FX_REFCLSID rclsid, char* filename );
			FX_HRESULT	addInprocServer32( wchar_t const* filename, firtex::com::FX_REFCLSID rclsid, wchar_t const* name, THREADINGMODEL tm, wchar_t const* VersionIndependentProgId, wchar_t const* ProgId );
			FX_HRESULT	removeInprocServer32( com::FX_REFCLSID clsid_ );
		} // namespace database
	} // namespace com
}// namespace firtex


#endif

