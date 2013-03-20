#include "com/Com.h"
#include "com/comerror.h"

#undef FX_STDAPI
#undef FX_STDAPIV
#undef FX_STDMETHOD
#undef FX_STDMETHODCALLTYPE
#undef FX_STDMETHODIMP
#undef FX_WINOLEAPI
#undef FX_WINOLEAPI_
#undef FX_WINOLEAUTAPI
#undef FX_WINOLEAUTAPI_

#define STRICT
#include <windows.h>

extern HINSTANCE hDllInstance;

static firtex::com::FX_HRESULT RegSetValueW( HKEY hKey, LPCWSTR lpValueName, LPCWSTR lpValue )
{
	return ::RegSetValueW( hKey, lpValueName, REG_SZ, lpValue, (DWORD)wcslen(lpValue) );
}

static firtex::com::FX_HRESULT SetThreadingModel( HKEY hKey, firtex::com::THREADINGMODEL tm )
{
	// look up the value
	wchar_t const* text = 0;
	switch (tm) {
	case firtex::com::ThreadingModelApartment: text = L"Apartment"; break;
	case firtex::com::ThreadingModelFree:  text = L"Free"; break;
	case firtex::com::ThreadingModelBoth:  text = L"Both"; break;
	case firtex::com::ThreadingModelNeutral:text = L"Neutral"; break;
	};
	if ( !text ) return ERROR_SUCCESS;

	// open up the sub-key
	HKEY hSubKey = 0;
	firtex::com::FX_HRESULT hr = RegOpenKeyExW( hKey, 
		L"InprocServer32", 
		0, 
		KEY_ALL_ACCESS, 
		&hSubKey );
	if ( hr != ERROR_SUCCESS ) return MAKE_HRESULT(SEVERITY_ERROR,FACILITY_WIN32,hr);

	// set the subvalue
	hr = ::RegSetValueExW( hSubKey, L"ThreadingModel", 0, REG_SZ, 
		reinterpret_cast<CONST byte*>(text), (DWORD)wcslen(text)*sizeof(*text) );
	RegCloseKey( hSubKey );
	return hr;
}

firtex::com::FX_HRESULT FX_STDAPICALLTYPE firtex::com::DllRegisterServerImpl(firtex::com::FX_REFCLSID clsid_, wchar_t const* name, THREADINGMODEL tm, wchar_t const* VersionIndependentProgId, wchar_t const* ProgId )
{
	// First, we are going to need the DLL filename
	wchar_t filename[MAX_PATH+1];
	firtex::com::FX_HRESULT hr = GetModuleFileNameW(hDllInstance,filename,sizeof(filename)/sizeof(*filename));
	if ( hr == 0 ) return MAKE_HRESULT(SEVERITY_ERROR,FACILITY_WIN32,GetLastError());

	// need string from clsid
	wchar_t clsid[64];
	int clsid_len = FX_StringFromGUID2( clsid_, clsid, sizeof(clsid)/sizeof(*clsid) );
	clsid_len -= 1;

	// setup version independent prog id key
	if ( VersionIndependentProgId )
	{
		HKEY hKey = 0;
		DWORD dw;
		firtex::com::FX_HRESULT hr = RegCreateKeyExW( HKEY_CLASSES_ROOT, 
			VersionIndependentProgId, 
			0, 
			NULL, 
			REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS,
			NULL,
			&hKey,
			&dw );
		if ( hr != ERROR_SUCCESS ) return MAKE_HRESULT(SEVERITY_ERROR,FACILITY_WIN32,hr);
		RegSetValueW(	hKey, NULL, REG_SZ, name, (DWORD)wcslen(name) );
		RegSetValueW( hKey, L"CLSID", REG_SZ, clsid, clsid_len );
		if ( ProgId )
			RegSetValueW( hKey, L"CurVer", REG_SZ, ProgId, (DWORD)wcslen(ProgId) );
		RegCloseKey( hKey );
	}
	// setup prog id key
	if ( ProgId )
	{
		HKEY hKey = 0;
		DWORD dw;
		firtex::com::FX_HRESULT hr = RegCreateKeyExW( HKEY_CLASSES_ROOT, 
			ProgId, 
			0, 
			NULL, 
			REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS,
			NULL,
			&hKey,
			&dw );
		if ( hr != ERROR_SUCCESS ) return MAKE_HRESULT(SEVERITY_ERROR,FACILITY_WIN32,hr);
		RegSetValueW( hKey, NULL, REG_SZ, name, (DWORD)wcslen(name) );
		RegSetValueW( hKey, L"CLSID", REG_SZ, clsid, clsid_len );
		RegCloseKey( hKey );
	}

	// construct key path
	wchar_t subkey[6+sizeof(clsid)] = L"CLSID\\";
	wcscat( subkey, clsid );
	// setup CLSID key
	HKEY hKey = 0;
	DWORD dw;
	hr = RegCreateKeyExW( HKEY_CLASSES_ROOT, 
		subkey, 
		0, 
		NULL, 
		REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&hKey,
		&dw );
	if ( hr != ERROR_SUCCESS ) return MAKE_HRESULT(SEVERITY_ERROR,FACILITY_WIN32,hr);
	RegSetValueW( hKey, NULL, REG_SZ, name, (DWORD)wcslen(name) );
	RegSetValueW( hKey, L"InprocServer32", filename );
	SetThreadingModel( hKey, tm );
	if ( ProgId )
		RegSetValueW( hKey, L"ProgID", ProgId );
	if ( VersionIndependentProgId )
		RegSetValueW( hKey, L"VersionIndependentProgID", VersionIndependentProgId );
	RegCloseKey( hKey );

	return FX_S_OK;
}

static firtex::com::FX_HRESULT FX_STDAPICALLTYPE MyRegDeleteKey( HKEY hKey, wchar_t const* lpSubKey )
{
	// open sub key, to check for children
	HKEY hSubKey = 0;
	firtex::com::FX_HRESULT hr = RegOpenKeyExW(hKey,lpSubKey,0,KEY_ALL_ACCESS,&hSubKey);
	if ( hr != ERROR_SUCCESS ) return MAKE_HRESULT(SEVERITY_ERROR,FACILITY_WIN32,hr);

	// first, delete all sub keys
	wchar_t name[MAX_PATH];
	DWORD   name_len = sizeof(name)/sizeof(*name);
	hr = RegEnumKeyExW( hSubKey, 0, name, &name_len, 0, NULL, NULL, NULL );
	while ( hr == ERROR_SUCCESS )
	{
		// delete the subkey
		hr = MyRegDeleteKey( hSubKey, name );
		if ( FX_FAILED(hr) ) return hr;
		// get next subkey
		DWORD   name_len = sizeof(name)/sizeof(*name);
		hr = RegEnumKeyExW( hSubKey, 0, name, &name_len, 0, NULL, NULL, NULL );
	}
	if ( hr != ERROR_NO_MORE_ITEMS ) return MAKE_HRESULT(SEVERITY_ERROR,FACILITY_WIN32,hr);;

	// close the key, and then delete
	RegCloseKey( hSubKey );
	hr = RegDeleteKeyW( hKey, lpSubKey );
	if ( hr != ERROR_SUCCESS ) return MAKE_HRESULT(SEVERITY_ERROR,FACILITY_WIN32,hr);

	return FX_S_OK;
}

firtex::com::FX_HRESULT FX_STDAPICALLTYPE firtex::com::DllUnregisterServerImpl(firtex::com::FX_REFCLSID clsid_)
{
	// first extract prog ids
	wchar_t	progid[MAX_PATH] = L"";
	wchar_t versionindependentprogid[MAX_PATH] = L"";

	// need string from clsid
	wchar_t clsid[64];
	FX_StringFromGUID2( clsid_, clsid, sizeof(clsid)/sizeof(*clsid) );
	// construct key path
	wchar_t subkey[6+sizeof(clsid)] = L"CLSID\\";
	wcscat( subkey, clsid );

	HKEY hKey = 0;
	firtex::com::FX_HRESULT hr = RegOpenKeyExW(HKEY_CLASSES_ROOT,subkey,0,KEY_ALL_ACCESS,&hKey);
	if ( hr != ERROR_SUCCESS ) return MAKE_HRESULT(SEVERITY_ERROR,FACILITY_WIN32,hr);

	// extract prog ids
	LONG lng = sizeof(progid)/sizeof(*progid);
	hr = RegQueryValueW(hKey,L"ProgID",progid,&lng);
	if ( hr != ERROR_SUCCESS ) progid[0] = 0;
	lng = sizeof(versionindependentprogid)/sizeof(*versionindependentprogid);
	hr = RegQueryValueW(hKey,L"VersionIndependentProgID",versionindependentprogid,&lng);
	if ( hr != ERROR_SUCCESS ) progid[0] = 0;

	// delete the extra info
	if ( progid[0] ) 
		MyRegDeleteKey( HKEY_CLASSES_ROOT, progid );
	if ( versionindependentprogid[0] ) 
		MyRegDeleteKey( HKEY_CLASSES_ROOT, versionindependentprogid );

	return MyRegDeleteKey( HKEY_CLASSES_ROOT, subkey );
}
