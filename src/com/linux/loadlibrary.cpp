#include "com/Com.h"
#include <dlfcn.h>
#include <stdio.h>
#include <wchar.h>
#include "com/linux/internal.h"

/***********************************************************************
 *           FX_CoLoadLibrary (OLE32.@)
 */
void* FX_STDAPICALLTYPE firtex::com::FX_CoLoadLibrary(wchar_t const* lpszLibName, uint16_t bAutoFree)
{
	// convert the text to ascii
	internal::zstr filename_( lpszLibName );
	if ( !filename_ ) return 0;
	char const* filename = filename_;

	// load the library
	void* ret = dlopen( filename, RTLD_LOCAL | RTLD_NOW );
	if ( !ret ) internal::log_printf( internal::ERROR, "Could not open library %s.\n%s\n", filename, dlerror() );
}

/***********************************************************************
 *           FX_CoFreeLibrary [OLE32.@]
 *
 * NOTES: don't believe the documentation
 */
void FX_STDAPICALLTYPE firtex::com::FX_CoFreeLibrary(void* hLibrary)
{
    int ret = dlclose( hLibrary );
	if ( ret ) {
		internal::log_printf( internal::ERROR, "Could not close library.\n%s\n", dlerror() );
	}
}

/***********************************************************************
 *           FX_CoFreeAllLibraries [OLE32.@]
 *
 * NOTES: don't believe the documentation
 */
void FX_STDAPICALLTYPE firtex::com::FX_CoFreeAllLibraries(void)
{
    /* NOP */
}

/***********************************************************************
 *           FX_CoFreeUnusedLibraries [COMPOBJ.17]
 *           FX_CoFreeUnusedLibraries [OLE32.@]
 *
 * FIXME: Calls to FX_CoFreeUnusedLibraries from any thread always route
 * through the main apartment's thread to call DllCanUnloadNow
 */
void FX_STDAPICALLTYPE firtex::com::FX_CoFreeUnusedLibraries(void)
{
	internal::FreeUnusedInprocServers();
}

