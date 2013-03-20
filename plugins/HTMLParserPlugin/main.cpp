#include "HTMLParserPlugin.h"

#include "com/Com.h"
#include "com/ComPtr.h"

using namespace firtex::plugin;
using namespace firtex::com;
/////////////////////////////////////////////////////////////////////////////
// Determines whether the DLL that implements this function is in use.
FX_STDAPI DllCanUnloadNow()
{
	return firtex::com::DllCanUnloadNowImpl();
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

FX_STDAPI DllGetClassObject(firtex::com::FX_REFCLSID rclsid, firtex::com::FX_REFIID riid, void** ppv)
{
	static CHTMLParserFactory factory;

	// find the right UUID
//	if ( firtex::com::FX_IsEqualCLSID( rclsid, firtex::plugin::IParserPlugin::clsid ) )
	{
		return factory.QueryInterface( riid, ppv );
	}

	return FX_CLASS_E_CLASSNOTAVAILABLE;
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

FX_STDAPI DllRegisterServer(void)
{
	// registers object, typelib and all interfaces in typelib
	return firtex::com::DllRegisterServerImpl( firtex::plugin::HTMLParserPlugin::clsid, 
		L"HTML Parser For FirteX",
		//firtex::com::ThreadingModelApartment, 
		firtex::com::ThreadingModelBoth, 
		firtex::plugin::HTMLParserPlugin::VersionIndependentProgId, 
		firtex::plugin::HTMLParserPlugin::ProgId
		);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

FX_STDAPI DllUnregisterServer(void)
{
	// _Module.UnregisterServer();
	return firtex::com::DllUnregisterServerImpl( firtex::plugin::HTMLParserPlugin::clsid );
}
