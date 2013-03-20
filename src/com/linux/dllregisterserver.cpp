#include "com/Com.h"
#include <assert.h>
//#include <sqlite.h>
#include <stdio.h>
#include <string.h>
#include "com/linux/internal.h"
#include "com/linux/database.h"

firtex::com::FX_HRESULT firtex::com::DllRegisterServerImpl(firtex::com::FX_REFCLSID clsid, wchar_t const* name, THREADINGMODEL tm, wchar_t const* VersionIndependentProgId, wchar_t const* ProgId )
{
	return firtex::com::database::addInprocServer32( internal::regsvr32_get_filename(), clsid, name, tm, VersionIndependentProgId, ProgId );
}

firtex::com::FX_HRESULT firtex::com::DllUnregisterServerImpl(firtex::com::FX_REFCLSID clsid)
{
	return firtex::com::database::removeInprocServer32( clsid );
}

