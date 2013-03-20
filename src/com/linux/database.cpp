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
#include <assert.h>
#include <errno.h>
#include <memory>

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include "com/linux/internal.h"
#include "com/linux/database.h"

#include "utility/XML.h"

#define FIRTEX_COMDB "FIRTEX_COMDB_PATH"

namespace firtex
{
	namespace com 
	{
		namespace database
		{
			static int32_t initialize_count = 0;
			static char	dbPath[512] = "./plugins.xml";
			static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

			FX_HRESULT Initialize()
			{
				firtex::com::internal::pthread_mutex_lock lock( &mutex );

				// check if we need to do initialization
				if ( initialize_count == 0 )
				{
					// check that the environment variable has been set
					if ( ! getenv( FIRTEX_COMDB ) || !*getenv(FIRTEX_COMDB) )
					{
						internal::log_printf( internal::ERROR, "Please set the environment variable " FIRTEX_COMDB " to point to the COM registry database(comdb.xml).\n" );
						return FX_E_FAIL;
					}
					strcpy(dbPath,getenv( FIRTEX_COMDB ));
					strcat(dbPath,"/plugins.xml");
				}

				// update count
				++initialize_count;
				// and we are done
				return FX_S_OK;
			}

			void Uninitialize()
			{
				// lock this module
				internal::pthread_mutex_lock lock( &mutex );

				// make sure everything is in good order
				assert( initialize_count > 0 );				

				// decrement the lock count
				--initialize_count;
				if ( initialize_count > 0 ) return;				
			}

			static char const* tmfield( THREADINGMODEL tm )
			{
				switch (tm) 
				{
				case ThreadingModelApartment: return "1";
				case ThreadingModelFree: return "2";
				case ThreadingModelBoth: return "3";
				case ThreadingModelNeutral: return "4";
				default: return "0";
				}
			}

			firtex::utility::CXMLElement* loadXMLDB(const char* dbname)
			{
				try
				{
					firtex::utility::CXMLElement* db;
					internal::pthread_mutex_lock lock( &mutex );					
					db = firtex::utility::CXMLElement::fromFile(dbname);					
					return db;
				}
				catch (...)
				{
					internal::log_printf(internal::ERROR,"load com database error.\n");						
				}			
				return NULL;
			}
			FX_HRESULT lookupInprocServer32( FX_REFCLSID rclsid, char* filename )
			{
				// need string from clsid
				wchar_t wcs_clsid[64];
				FX_StringFromGUID2( rclsid, wcs_clsid, sizeof(wcs_clsid)/sizeof(*wcs_clsid) );
				char clsid[64];
				mbstate_t state;
				memset( &state, 0, sizeof(state) );
				wchar_t * wcs_clsid_ptr = wcs_clsid;
				wcsrtombs( clsid, (const wchar_t**)&wcs_clsid_ptr, sizeof(clsid)/sizeof(*clsid), &state);

				std::auto_ptr<firtex::utility::CXMLElement> db_ptr(loadXMLDB(dbPath));				
				if(!db_ptr.get())
					return FX_E_FAIL;
				firtex::utility::CXMLElement* pluginsElem = db_ptr.get();			
				
				for ( firtex::utility::ElementIterator iter = pluginsElem->getElementIterator() ; pluginsElem->hasNextElement(iter) ; )
				{
					firtex::utility::CXMLElement* pElement = pluginsElem->getNextElement( iter );
					firtex::utility::CXMLElement* clsidElem = pElement->getElementByName(_T("clsid"));
					if(!clsidElem)
						continue;
					if ( _tcsicmp(clsidElem->getValue().c_str(),clsid ))
					{
						firtex::utility::CXMLElement* fnElem = pElement->getElementByName(_T("filename"));
						if(fnElem)
						{
							strcpy(filename,getenv(FIRTEX_COMDB));
							strcat(filename,"/");
							strcat(filename,fnElem->getValue().c_str());
							strcat(filename,".so");
							return FX_S_OK;
						}						
					}

				}
				return FX_E_FAIL;			
			}

			FX_HRESULT addInprocServer32( wchar_t const* filename, com::FX_REFCLSID rclsid, wchar_t const* name, THREADINGMODEL tm, wchar_t const* VersionIndependentProgId, wchar_t const* ProgId )
			{
//				internal::log_printf( internal::INFO, "Adding InprocServer32\n" );				
//
//				try
//				{
//					// need string from clsid
//					wchar_t clsid[64];
//					FX_StringFromGUID2( rclsid, clsid, sizeof(clsid)/sizeof(*clsid) );
//
//					internal::pthread_mutex_lock lock( &mutex );
//					firtex::utility::CXMLElement* db = loadXMLDB(dbPath);
//					assert(db);
//					firtex::utility::CXMLElement* clsidsElem = db->getElementByName(_T("clsids"),true);
//					firtex::utility::CXMLElement* clsidElem = clsidsElem->addElement(_T("clsid"));
//					firtex::utility::CXMLElement* pElem = clsidElem->addElement(_T("clsid"));
//
//#ifndef _UNICODE
//					internal::zstr c_clsid( clsid);
//					char const* temp = ((char const*)c_clsid) ? ((char const*)c_clsid) : "";
//					pElem->setValue(temp);
//#else 
//					pElem->setValue(clsid);
//#endif
//
//					pElem = clsidElem->addElement(_T("name"));
//#ifndef _UNICODE
//					internal::zstr c_name(name);
//					temp = ((char const*)c_name) ? ((char const*)c_name) : "";
//					pElem->setValue(temp);
//#else
//					pElem->setValue(name);
//#endif
//					pElem = clsidElem->addElement(_T("progid"));
//#ifndef _UNICODE
//					internal::zstr c_progid( ProgId);
//					temp = ((char const*)c_progid) ? ((char const*)c_progid) : "";
//					pElem->setValue(temp);
//#else 
//					pElem->setValue(ProgId);
//#endif
//					pElem = clsidElem->addElement(_T("versionindependentprogid"));
//#ifndef _UNICODE
//					internal::zstr c_vprogid( VersionIndependentProgId);
//					temp = ((char const*)c_vprogid) ? ((char const*)c_vprogid) : "";
//					pElem->setValue(temp);
//#else 
//					pElem->setValue(VersionIndependentProgId);
//#endif
//					pElem = clsidElem->addElement(_T("threadingmodel"));
//					pElem->setValue(tmfield(tm));
//
//					pElem = clsidElem->addElement(_T("filename"));
//#ifndef _UNICODE
//					internal::zstr c_fn( filename);
//					temp = ((char const*)c_fn) ? ((char const*)c_fn) : "";
//					pElem->setValue(temp);
//#else 
//					pElem->setValue(filename);
//#endif
//					db->toFile(dbPath,true);
//					delete db;
//
//				}
//				catch (...)
//				{
//					internal::zstr c_filename( filename );
//					char const* tmp = ((char const*)c_filename) ? ((char const*)c_filename) : "";
//					internal::log_printf( internal::ERROR, "Could not register InprocServer32 (%s).\n", tmp ); 
//					return FX_E_FAIL;
//				}
				return FX_S_OK;
			}
			FX_HRESULT removeInprocServer32( com::FX_REFCLSID clsid_)
			{
			//	// need string from clsid
			//	wchar_t wcs_clsid[64];
			//	FX_StringFromGUID2( clsid_, wcs_clsid, sizeof(wcs_clsid)/sizeof(*wcs_clsid) );
			//	char clsid[64];
			//	wchar_t* wcs_clsid_ptr = wcs_clsid;
			//	wcsrtombs( clsid, (const wchar_t**)&wcs_clsid_ptr, sizeof(clsid)/sizeof(*clsid), 0 );

			//	internal::pthread_mutex_lock lock( &mutex );
			//	firtex::utility::CXMLElement* db = loadXMLDB(dbPath);
			//	assert(db);
			//	firtex::utility::CXMLElement* clsidsElem = db->getElementByName(_T("clsids"));
			//	if(!clsidsElem)
			//		return FX_S_OK;

			//	
			//	for ( firtex::utility::ElementIterator iter = clsidsElem->getElementIterator() ; clsidsElem->hasNextElement(iter) ; )
			//	{
			//		firtex::utility::CXMLElement* pElement = clsidsElem->getNextElement( iter );
			//		for ( firtex::utility::ElementIterator iter2 = pElement->getElementIterator() ; pElement->hasNextElement(iter) ; )
			//		{
			//			firtex::utility::CXMLElement* pElement2 = clsidsElem->getNextElement( iter );
			//			if ( _tcsicmp(pElement2->getValue().c_str(),clsid ) == 0 )
			//			{
			//				clsidsElem->removeElement(pElement);
			//				return FX_S_OK;
			//			}
			//		}

			//	}			
			//	db->toFile(dbPath,true);
			//	delete db;
			//	return FX_S_OK;
			}					
		}	
		
	}
}

static wchar_t regsvr32_filename[260*2+1];

extern wchar_t const* firtex::com::internal::regsvr32_get_filename()
{
	return regsvr32_filename;
}

extern firtex::com::FX_HRESULT firtex::com::internal::regsvr32_set_filename( wchar_t const* filename )
{
	if ( !filename ) filename = L"";
	if ( wcslen(filename) > sizeof(regsvr32_filename)/sizeof(*regsvr32_filename) - 1 ) return FX_E_OUTOFMEMORY;
	wcscpy( regsvr32_filename, filename );
	return FX_S_OK;
}

