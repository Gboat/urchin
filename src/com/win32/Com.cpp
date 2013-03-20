#include "com/Com.h"

#ifndef _WIN32_WINNT		// 允许使用 Windows NT 4 或更高版本的特定功能。
#define _WIN32_WINNT 0x0400		//为 Windows98 和 Windows 2000 及更新版本改变为适当的值。
#endif	

#include <objbase.h>
#include "com/comerror.h"

		

namespace firtex
{
	namespace com
	{
		FX_WINOLEAPI			FX_CLSIDFromString(wchar_t const* idstr, FX_CLSID* id)
		{
			return ::CLSIDFromString((LPOLESTR)idstr,(CLSID*)id);
		}
		FX_WINOLEAPI_(uint32_t)	FX_CoBuildVersion( void )
		{
			return ::CoBuildVersion();
		}
		FX_WINOLEAPI			FX_CoCreateGuid(struct FX_GUID* pguid)
		{
			return ::CoCreateGuid((GUID*)pguid);
		}
		FX_WINOLEAPI			FX_CoCreateInstance(FX_REFCLSID rclsid, IUnknown * pUnkOuter,uint32_t dwClsContext, FX_REFIID riid, void __RPC_FAR* __RPC_FAR* ppv)
		{
			return ::CoCreateInstance((REFCLSID)rclsid,(LPUNKNOWN)pUnkOuter,dwClsContext,(REFIID)riid,ppv);
		}

	#if (_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM) // DCOM
		FX_WINOLEAPI			FX_CoCreateInstanceEx(FX_REFCLSID rclsid, IUnknown * pUnkOuter,uint32_t dwClsContext, struct COSERVERINFO * pServerInfo,uint32_t cmq, struct MULTI_QI * pResults)
		{
			return ::CoCreateInstanceEx((REFCLSID)rclsid,(LPUNKNOWN)pUnkOuter,dwClsContext, (::COSERVERINFO*)pServerInfo,cmq,(::MULTI_QI*)pResults);
		}
	#endif
		
		FX_WINOLEAPI_(void)		FX_CoFreeAllLibraries( void )
		{
			::CoFreeAllLibraries();
		}
		FX_WINOLEAPI_(void)		FX_CoFreeLibrary(void* hInst )
		{
			::CoFreeLibrary((HINSTANCE)hInst);
		}
		FX_WINOLEAPI_(void)		FX_CoFreeUnusedLibraries( void )
		{
			::CoFreeUnusedLibraries();
		}
		FX_WINOLEAPI			FX_CoGetClassObject(FX_REFCLSID rclsid,uint32_t dwClsContext, struct COSERVERINFO * pServerInfo,FX_REFIID riid,void** ppv)
		{
			return ::CoGetClassObject((REFCLSID)rclsid,dwClsContext,pServerInfo,(REFIID)riid,ppv);
		}
		FX_WINOLEAPI_(uint32_t)	FX_CoGetCurrentProcess(void)
		{
			return ::CoGetCurrentProcess();
		}
		FX_WINOLEAPI			FX_CoGetInterfaceAndReleaseStream(IStream * pStm, FX_REFIID riid, void** ppv)
		{
			return ::CoGetInterfaceAndReleaseStream((LPSTREAM)pStm,(REFIID)riid, ppv);
		}
		FX_WINOLEAPI			FX_CoGetMalloc(uint32_t dwMemContext, IMalloc ** lpMalloc)
		{
			return ::CoGetMalloc(dwMemContext,(::IMalloc**)lpMalloc);
		}

		FX_WINOLEAPI			FX_CoGetMarshalSizeMax(uint32_t* pulSize, FX_REFIID riid, IUnknown * pUnk,uint32_t dwDestContext, void *pvDestContext, uint32_t mshlFlags)
		{
			return ::CoGetMarshalSizeMax(pulSize,(REFIID)riid,(LPUNKNOWN)pUnk,dwDestContext,pvDestContext,mshlFlags);
		}
		FX_WINOLEAPI			FX_CoGetStandardMarshal(FX_REFIID riid,IUnknown * pUnk,uint32_t dwDestContext,void* pvDestContext,uint32_t mshlflags,IMarshal ** ppMarshal)
		{
			return ::CoGetStandardMarshal((REFIID)riid,(LPUNKNOWN)pUnk,dwDestContext,pvDestContext,mshlflags,(LPMARSHAL*)ppMarshal);
		}
		FX_WINOLEAPI_(void*)	FX_CoLoadLibrary(wchar_t const* lpszLibName, uint16_t bAutoFree /* ignored */ )
		{
			return ::CoLoadLibrary((LPOLESTR)lpszLibName, bAutoFree);
		}
			
		FX_WINOLEAPI			FX_CoInitialize(void* pvReserved)
		{
			return ::CoInitialize(pvReserved);
		}

	#if (_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM) // DCOM
		FX_WINOLEAPI			FX_CoInitializeEx(void* pvReserved, uint32_t dwCoInit )
		{
			return ::CoInitializeEx(pvReserved,dwCoInit);
		}
	#endif
		FX_WINOLEAPI			FX_CoMarshalInterface( IStream * pStm, FX_REFIID riid, IUnknown * pUnk, uint32_t dwDestContext, void * pvDestContext, uint32_t mshlflags )
		{
			return ::CoMarshalInterface((LPSTREAM)pStm,(REFIID)riid,(LPUNKNOWN)pUnk,dwDestContext,pvDestContext,mshlflags);
		}
		FX_WINOLEAPI			FX_CoMarshalInterThreadInterfaceInStream( FX_REFIID riid, IUnknown * pUnk, IStream ** ppStm )
		{
			return ::CoMarshalInterThreadInterfaceInStream((REFIID)riid,(LPUNKNOWN)pUnk,(LPSTREAM*)ppStm );
		}
		FX_WINOLEAPI			FX_CoReleaseMarshalData( IStream * pStm )
		{
			return ::CoReleaseMarshalData((LPSTREAM) pStm );
		}
		FX_WINOLEAPI			FX_CoRegisterMallocSpy(IMallocSpy * pMallocSpy)
		{
			return ::CoRegisterMallocSpy((LPMALLOCSPY)pMallocSpy);
		}
		FX_WINOLEAPI			FX_CoRevokeMallocSpy(void)
		{
			return ::CoRevokeMallocSpy();
		}
		FX_WINOLEAPI_(void*)	FX_CoTaskMemAlloc(uint32_t size)
		{
			return ::CoTaskMemAlloc(size);
		}
		FX_WINOLEAPI_(void)		FX_CoTaskMemFree(void* ptr)
		{
			::CoTaskMemFree(ptr);
		}
		FX_WINOLEAPI_(void*)	FX_CoTaskMemRealloc(void* ptr,uint32_t size)
		{
			return ::CoTaskMemRealloc(ptr,size);
		}
		FX_WINOLEAPI_(void)		FX_CoUninitialize(void)
		{
			CoUninitialize();
		}
		FX_WINOLEAPI			FX_CoUnmarshalInterface(IStream * pStm, FX_REFIID riid,void ** ppv )
		{
			return ::CoUnmarshalInterface((LPSTREAM)pStm,(REFIID)riid,ppv );
		}

		FX_WINOLEAPI_(int)		FX_StringFromGUID2(FX_REFGUID rguid, wchar_t* lpsz, int cbMax)
		{
			return ::StringFromGUID2((REFIID)rguid,lpsz,cbMax);
		}
		

		//////////////////////////////////////////////////////
		// Automation Api
		//////////////////////////////////////////////////////

		FX_WINOLEAUTAPI		FX_CreateErrorInfo(ICreateErrorInfo ** pperrinfo)
		{
			return ::CreateErrorInfo((::ICreateErrorInfo**)pperrinfo);
		}
		FX_WINOLEAUTAPI		FX_GetErrorInfo(uint32_t dwReserved, IErrorInfo ** pperrinfo)
		{
			return ::GetErrorInfo(dwReserved,(::IErrorInfo**) pperrinfo);
		}
		FX_WINOLEAUTAPI		FX_SetErrorInfo(uint32_t dwReserved, IErrorInfo * perrinfo)
		{
			return ::SetErrorInfo(dwReserved, (::IErrorInfo *)perrinfo);
		}

		FX_WINOLEAUTAPI_(FX_BSTR)		FX_SysAllocString(const wchar_t * str)
		{
			return ::SysAllocString(str);
		}
		FX_WINOLEAUTAPI_(int16_t)	FX_SysReAllocString(FX_BSTR *str1, const wchar_t *str2)
		{
			return ::SysReAllocString((::BSTR*)str1,str2);
		}
		FX_WINOLEAUTAPI_(FX_BSTR)		FX_SysAllocStringLen(const wchar_t *str, uint16_t len)
		{
			return ::SysAllocStringLen(str, len);
		}
		FX_WINOLEAUTAPI_(int16_t)	FX_SysReAllocStringLen(FX_BSTR *str1, const wchar_t *str2, uint16_t len)
		{
			return ::SysReAllocStringLen((::BSTR*)str1, str2, len);
		}
		FX_WINOLEAUTAPI_(void)		FX_SysFreeString(FX_BSTR str)
		{
			return ::SysFreeString((::BSTR)str);
		}
		FX_WINOLEAUTAPI_(uint16_t)	FX_SysStringLen(FX_BSTR str)
		{
			return ::SysStringLen((::BSTR)str);
		}
		FX_WINOLEAUTAPI_(uint16_t)	FX_SysStringByteLen(FX_BSTR bstr)
		{
			return ::SysStringByteLen((::BSTR)bstr);
		}
		FX_WINOLEAUTAPI_(FX_BSTR)		FX_SysAllocStringByteLen(const char* psz, uint16_t len)
		{
			return ::SysAllocStringByteLen(psz, len);
		}
		FX_WINOLEAPI				FX_RegSrv32A(const char* filename,bool_t bRegister)
		{
			HMODULE hMod = LoadLibraryA(filename);
			if(!hMod)
			{				
				return (FX_HRESULT) HRESULT_FROM_WIN32(::GetLastError());
			}

			FX_HRESULT hr = FX_S_OK;
			typedef FX_HRESULT (_stdcall *DLLPROC)();
			DLLPROC pfnDllProc = reinterpret_cast<DLLPROC>(GetProcAddress(hMod, bRegister ? "DllRegisterServer" : "DllUnregisterServer"));
			if (pfnDllProc)
			{
				if (SUCCEEDED(hr))
					hr = (*pfnDllProc)();
			}
			else
				hr = (FX_HRESULT)HRESULT_FROM_WIN32(::GetLastError());
			return hr;
		}
		FX_WINOLEAPI				FX_RegSrv32W(const wchar_t* filename,bool_t bRegister)
		{			
			HMODULE hMod = LoadLibraryW(filename);
			if(!hMod)
			{				
				return (FX_HRESULT)HRESULT_FROM_WIN32(::GetLastError());
			}

			FX_HRESULT hr = FX_S_OK;
			typedef FX_HRESULT (_stdcall *DLLPROC)();
			DLLPROC pfnDllProc = reinterpret_cast<DLLPROC>(GetProcAddress(hMod, bRegister ? "DllRegisterServer" : "DllUnregisterServer"));
			if (pfnDllProc)
			{
				if (SUCCEEDED(hr))
					hr = (*pfnDllProc)();
			}
			else
				hr = (FX_HRESULT)HRESULT_FROM_WIN32(::GetLastError());
			return hr;
		}
	}
}
