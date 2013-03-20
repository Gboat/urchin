#include "com/Com.h"
#include "com/comerror.h"
#include <assert.h>
#include <dlfcn.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "com/linux/internal.h"
#include "com/linux/database.h"

using namespace firtex::com;

struct InprocServer32
{
	void*		library;
	firtex::com::FX_HRESULT FX_STDAPICALLTYPE		(*GetClassObject)(firtex::com::FX_REFCLSID rclsid, firtex::com::FX_REFIID riid, void** ppv);
	firtex::com::FX_HRESULT FX_STDAPICALLTYPE		(*CanUnloadNow)();
	InprocServer32*	next;
};

static InprocServer32* first = 0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

firtex::com::FX_HRESULT firtex::com::internal::GetInprocClassObject( firtex::com::FX_REFCLSID rclsid, firtex::com::IUnknown** ppUnk )
{
	char dllpath[FILENAME_MAX+1];
	firtex::com::FX_HRESULT hr = firtex::com::database::lookupInprocServer32( rclsid, dllpath );
	if ( FX_FAILED(hr) ) return hr;

	// load the library
	InprocServer32* entry = 0;

	pthread_mutex_lock lock( &mutex );

	// check to see if the library has already been loaded
	// note, this does not necessarily mean we have an entry in our table
	void* library = dlopen( dllpath, RTLD_LOCAL | RTLD_NOW | RTLD_NOLOAD );
	if ( library ) {
		// find the library if already open
		InprocServer32* lp = first;
		while ( lp ) {
			if ( lp->library == library ) break;
			lp = lp->next;
		}
		entry = lp;
	}

	// if we have not yet found an entry for the server, we need to load the dll
	// we also need to create an entry
	if ( !entry ) {
		// the library has not been loaded
		// load it now
		void* library = dlopen( dllpath, RTLD_LOCAL | RTLD_NOW );
		if ( ! library ) {
			log_printf( ERROR, "Could not open the library %s.\n%s\n", dllpath, dlerror() );
			return FX_CO_E_DLLNOTFOUND;  // FX_E_ACCESSDENIED?
		}

		FX_HRESULT (*func)(FX_REFCLSID rclsid, FX_REFIID riid, void** ppv) = 0;
		*(void**)&func = dlsym( library, "DllGetClassObject" );
		if ( !func ) {
			log_printf( ERROR, "Could not find the DllGetClassObject entry point in %s.\n%s\n", dllpath, dlerror() );
			dlclose( library );
			return FX_CO_E_DLLNOTFOUND;
		}

		entry = (InprocServer32*)malloc( sizeof(InprocServer32) );
		if ( !entry ) return FX_E_OUTOFMEMORY;
		entry->library = library;
		entry->GetClassObject = func;
		entry->next = 0;

		if ( first ) first->next = entry;
		first = entry;

		// try to load the DllCanUnloadNow at this time
		FX_HRESULT (*func2)() = 0;
		*(void**)&func2 = dlsym( library, "DllCanUnloadNow" );
		entry->CanUnloadNow = func2;

		// send information about our success to terminal
		log_printf( INFO, "(%x) - Loaded the shared object library.\n\t%s\n", library, dllpath );

	}


	assert( entry );
	return entry->GetClassObject( rclsid, IUnknown::iid, (void**)ppUnk );
}

void firtex::com::internal::FreeUnusedInprocServers()
{
	pthread_mutex_lock lock( &mutex );

	InprocServer32** lp = &first;
	while ( *lp ) {
		// check if the dll entry point exists
		if ( ! (*lp)->CanUnloadNow ) continue;
		// check if we can unload now
		if ( (*(*lp)->CanUnloadNow)() == FX_S_OK ) {
			int ret = dlclose( (*lp)->library );
			if ( ret ) {
				log_printf( ERROR, "Could not close library.\n%s\n", dlerror() );
			}
			InprocServer32* entry = *lp;
			(*lp) = entry->next;
			free( entry );
		}
		else {
			// move to next slot
			lp = &(*lp)->next;
		}
	}
}
