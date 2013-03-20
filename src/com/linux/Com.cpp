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
#include "com/Com.h"
#include "com/comerror.h"
#include "com/linux/internal.h"
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <unistd.h>

namespace firtex
{
	namespace com
	{		
		//static char* normpath(const char *path, char *resolved_path )
		//{
		//	char buffer[PATH_MAX+1];
		//	char const* abspath;

		//	// strip leading whitespace
		//	while ( isspace(*path) ) ++path;

		//	// check for leading slash
		//	if ( path[0] != '/' ) 
		//	{
		//		abspath = getcwd( buffer, sizeof(buffer)/sizeof(*buffer) );
		//		if ( !abspath ) return 0;
		//		assert( abspath == buffer );
		//		if ( strlen(buffer) + strlen(path) + 2 > sizeof(buffer)/sizeof(*buffer) )
		//		{
		//			errno = ERANGE;
		//			return 0;
		//		}
		//		strcat( buffer, "/" );			
		//		strcat( buffer, path );			
		//	}
		//	else 
		//	{
		//		abspath = path ;
		//	}

		//	// remove symbolic links from path
		//	return realpath(  abspath, resolved_path );
		//}

		FX_WINOLEAPI				FX_RegSrv32A(const char* filename,bool_t bRegister)
		{			
			return FX_S_OK;
			//FX_HRESULT hr = FX_S_OK;				
			//char nfilename[PATH_MAX+1];
			//// get the absolute path to the file
			//normpath( filename, nfilename );
			//
			//void* handle = dlopen( nfilename, RTLD_LAZY | RTLD_LOCAL );
			//if ( !handle )
			//{				
			//	return FX_E_FAIL;
			//}

			//firtex::com::FX_HRESULT (*func)() = 0;
			//*(void**)&func = dlsym( handle, bRegister ? "DllRegisterServer" : "DllUnregisterServer" );
			//if ( !func )
			//{				
			//	dlclose( handle );
			//	return FX_E_FAIL;
			//}

			//// UNIX necessary since shared object don't have a instance handle
			//firtex::com::internal::wzstr wfilename( filename );
			//if ( !wfilename ) 
			//{				
			//	dlclose( handle );
			//	return FX_E_OUTOFMEMORY;
			//}
			//firtex::com::FX_HRESULT hr2 = firtex::com::internal::regsvr32_set_filename( wfilename );
			//if ( FX_FAILED(hr2) ) 
			//{				
			//	dlclose( handle );
			//	return hr2;
			//}

			//// call the function
			//hr = (*func)();
			//if ( FX_FAILED(hr) ) 
			//{				
			//	dlclose( handle );
			//	return hr;
			//}
			//// UNIX necessary since shared object don't have a instance handle
			//hr = firtex::com::internal::regsvr32_set_filename( L"" );
			//assert( FX_SUCCEEDED(hr) );

			//// close the shared object
			//dlclose( handle );
			//return hr;
		}
		FX_WINOLEAPI				FX_RegSrv32W(const wchar_t* filename,bool_t bRegister)
		{
			FX_HRESULT hr = FX_S_OK;		
			return hr;
		}
	}
}
