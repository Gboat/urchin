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
#include "com/Com.h"
#include <stdlib.h>
#include "com/linux/internal.h"

static void ThreadSpecificDataDestructor( void* data )
{
	free( data );
}

firtex::com::internal::thread_data_t* firtex::com::internal::GetThreadSpecificData()
{
	static bool is_initialized = false;
	static pthread_key_t key;

	// create the key used to access thread specific data
	if ( !is_initialized ) {
		int ret = pthread_key_create( &key, &ThreadSpecificDataDestructor );
		if ( ret ) {
			internal::log_printf( internal::ERROR, "Could not create pthread key.\n" );
			exit(-1);
		}
		is_initialized = true;
	}

	// get the data
	void* ptr = pthread_getspecific( key );
	if ( ptr ) return reinterpret_cast<firtex::com::internal::thread_data_t*>(ptr);

	// create the thread specific data
	ptr = malloc( sizeof( thread_data_t ) );
	if ( !ptr ) {
		internal::log_printf( internal::ERROR, "Could not allocate memory for thread specific data.\n" );
		exit(-1);
	}
	memset( ptr, 0, sizeof(thread_data_t) );

	// set the data
	int ret = pthread_setspecific( key, ptr );
	if ( ret ) {
		internal::log_printf( internal::ERROR, "Could not set thread specific data.\n" );
		exit(-1);
	}

	return reinterpret_cast<firtex::com::internal::thread_data_t*>(ptr);
}

