#ifndef _COM_H_
#define _COM_H_

#include "../utility/stdint.h"

//#include <stddef.h>
#include <memory.h>



#ifdef __cplusplus
	#define EXTERN_C    extern "C"
#else
	#define EXTERN_C    extern
#endif

#if defined(WIN32) 

	#define FX_STDMETHODCALLTYPE		__stdcall	
	#define FX_STDMETHODVCALLTYPE		__cdecl
	#define FX_STDAPICALLTYPE			__stdcall
	#define FX_STDAPIVCALLTYPE			__cdecl
	#define FX_WINOLEAPI				FX_STDAPI //EXTERN_C DECLSPEC_IMPORT FX_HRESULT FX_STDAPICALLTYPE
	#define FX_WINOLEAPI_(type)			FX_STDAPI_(type) //EXTERN_C DECLSPEC_IMPORT type FX_STDAPICALLTYPE
	#define FX_WINOLEAUTAPI				FX_STDAPI //EXTERN_C DECLSPEC_IMPORT FX_HRESULT FX_STDAPICALLTYPE
	#define FX_WINOLEAUTAPI_(type)		FX_STDAPI_(type) //EXTERN_C DECLSPEC_IMPORT type FX_STDAPICALLTYPE
	#define								__RPC_FAR


#else	//for linux

	#define FX_STDMETHODCALLTYPE
	#define FX_STDMETHODVCALLTYPE 
	#define FX_STDAPICALLTYPE          
	#define FX_STDAPIVCALLTYPE         
	#define FX_WINOLEAPI				FX_STDAPI //EXTERN_C DECLSPEC_IMPORT FX_HRESULT FX_STDAPICALLTYPE
	#define FX_WINOLEAPI_(type)			FX_STDAPI_(type) //EXTERN_C DECLSPEC_IMPORT type FX_STDAPICALLTYPE
	#define FX_WINOLEAUTAPI				FX_STDAPI
	#define FX_WINOLEAUTAPI_(type)		FX_STDAPI_(type)
	#define								__RPC_FAR

#endif

#ifdef __cplusplus

	#define FX_STDMETHOD(method)		virtual firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE method
	#define FX_STDMETHOD_(type,method)	virtual type FX_STDMETHODCALLTYPE method
	#define FX_STDMETHODIMP				firtex::com::FX_HRESULT FX_STDMETHODCALLTYPE
	#define FX_STDMETHODIMP_(type)		type FX_STDMETHODCALLTYPE
	#define FX_STDAPI					EXTERN_C firtex::com::FX_HRESULT FX_STDAPICALLTYPE
	#define FX_STDAPI_(type)			EXTERN_C type FX_STDAPICALLTYPE
	#define FX_STDAPIV					EXTERN_C firtex::com::FX_HRESULT FX_STDAPIVCALLTYPE
	#define FX_STDAPIV_(type)			EXTERN_C type FX_STDAPIVCALLTYPE

#else
	//#define FX_STDMETHOD(method)       virtual FX_HRESULT FX_STDMETHODCALLTYPE method
	//#define FX_STDMETHOD_(type,method) virtual type FX_STDMETHODCALLTYPE method

	#define FX_STDMETHODIMP				FX_HRESULT FX_STDMETHODCALLTYPE
	#define FX_STDMETHODIMP_(type)		type FX_STDMETHODCALLTYPE
	#define FX_STDAPI					EXTERN_C FX_HRESULT FX_STDAPICALLTYPE
	#define FX_STDAPI_(type)			EXTERN_C type FX_STDAPICALLTYPE
	#define FX_STDAPIV					EXTERN_C FX_HRESULT FX_STDAPIVCALLTYPE
	#define FX_STDAPIV_(type)			EXTERN_C type FX_STDAPIVCALLTYPE
	#define FX_BEGIN_INTERFACE	
	#define FX_END_INTERFACE

#endif

#ifdef __cplusplus
namespace firtex
{
	namespace com 
	{
#endif


		//////////////////////////////////////////////////////
		// Typedefs
		//////////////////////////////////////////////////////

		typedef int32_t	FX_HRESULT;
		typedef int16_t	bool_t;
		typedef wchar_t const* FX_BSTR;
		enum THREADINGMODEL 
		{
			ThreadingModelApartment = 1,
			ThreadingModelFree = 2,
			ThreadingModelBoth = 3,
			ThreadingModelNeutral = 4
		};


		//////////////////////////////////////////////////////
		// FX_GUID, FX_IID, FX_CLSID
		//////////////////////////////////////////////////////
//#ifndef GUID_DEFINED

		struct FX_GUID
		{
			uint32_t	Data1;
			uint16_t	Data2;
			uint16_t	Data3;
			uint8_t	Data4[8];
		};
//#endif //GUID_DEFINED

#ifdef __cplusplus
		typedef FX_GUID		const& FX_REFGUID;
		typedef FX_GUID		FX_IID;
		typedef FX_IID		const& FX_REFIID;
		typedef FX_GUID		FX_CLSID;
		typedef FX_CLSID	const& FX_REFCLSID;

		inline bool FX_IsEqualGUID(FX_REFGUID rguid1, FX_REFGUID rguid2)
		{
			return !memcmp(&rguid1, &rguid2, sizeof(FX_GUID));
		}
		inline bool FX_IsEqualIID(FX_REFIID rguid1, FX_REFIID rguid2)
		{
			return !memcmp(&rguid1, &rguid2, sizeof(FX_GUID));
		}
		inline bool FX_IsEqualCLSID(FX_REFCLSID rguid1, FX_REFCLSID rguid2)
		{
			return !memcmp(&rguid1, &rguid2, sizeof(FX_GUID));
		}
#else
		typedef struct FX_GUID const * FX_REFGUID;
		typedef struct FX_GUID FX_IID;
		typedef FX_IID const* FX_REFIID;
		typedef struct FX_GUID FX_CLSID;
		typedef FX_CLSID const* FX_REFCLSID;
#endif

		//////////////////////////////////////////////////////
		// Interface Forward Declares
		//////////////////////////////////////////////////////

#ifdef __cplusplus
#define FORWARD_DECLARE(x) \
		class x;
#else
#define FORWARD_DECLARE(x) \
		struct x; \
		typedef struct x x;
#endif

		FORWARD_DECLARE( IUnknown );

		FORWARD_DECLARE( ICreateErrorInfo );
		FORWARD_DECLARE( IClassFactory );
		FORWARD_DECLARE( IErrorInfo );
		FORWARD_DECLARE( IGlobalInterfaceTable );
		FORWARD_DECLARE( IMalloc );
		FORWARD_DECLARE( IMallocSpy );
		FORWARD_DECLARE( IMarshal );
		FORWARD_DECLARE( ISequentialStream );
		FORWARD_DECLARE( IStream );
		FORWARD_DECLARE( ISupportErrorInfo );

#undef FORWARD_DECLARE

		//////////////////////////////////////////////////////
		// Enums
		//////////////////////////////////////////////////////

		// COM initialization flags; passed to FX_CoInitialize.
		enum coinit_t
		{
			COINIT_APARTMENTTHREADED  = 0x2,      // Apartment model
			COINIT_MULTITHREADED      = 0x0,      // OLE calls objects on any thread.
			COINIT_DISABLE_OLE1DDE    = 0x4,      // Don't use DDE for Ole1 support.
			COINIT_SPEED_OVER_MEMORY  = 0x8       // Trade memory for speed.
		};

		//////////////////////////////////////////////////////
		// API Structures
		//////////////////////////////////////////////////////

		struct COAUTHIDENTITY 
		{ 
			uint16_t* User; 
			uint32_t	UserLength;
			uint16_t* Domain; 
			uint32_t	DomainLength; 
			uint16_t* Password; 
			uint32_t	PasswordLength; 
			uint32_t	Flags; 
		};
		struct COAUTHINFO 
		{
			uint32_t	dwAuthnSvc;
			uint32_t	dwAuthzSvc;
			wchar_t*	pwszServerPrincName;
			uint32_t	dwAuthnLevel;
			uint32_t	dwImpersonationLevel;
			struct COAUTHIDENTITY * pAuthIdentityData;
			uint32_t	dwCapabilities;
		};
		struct COSERVERINFO 
		{
			uint32_t dwReserved1;
			wchar_t* pwszName;
			struct COAUTHINFO* pAuthInfo;
			uint32_t dwReserved2;
		};
		struct MULTI_QI 
		{
			FX_IID const * pIID;
			IUnknown * pItf;
			FX_HRESULT hr;
		};

		//////////////////////////////////////////////////////
		// API Calls
		//////////////////////////////////////////////////////

		enum FX_CLSCTX 
		{
			FX_CLSCTX_INPROC_SERVER	= 0x1,
			FX_CLSCTX_INPROC_HANDLER	= 0x2,
			FX_CLSCTX_LOCAL_SERVER		= 0x4,
			FX_CLSCTX_INPROC_SERVER16	= 0x8,
			FX_CLSCTX_REMOTE_SERVER	= 0x10,
			FX_CLSCTX_INPROC_HANDLER16	= 0x20,
			FX_CLSCTX_INPROC_SERVERX86	= 0x40,
			FX_CLSCTX_INPROC_HANDLERX86	= 0x80,
			FX_CLSCTX_ESERVER_HANDLER	= 0x100,
			FX_CLSCTX_INPROC = FX_CLSCTX_INPROC_SERVER | FX_CLSCTX_INPROC_HANDLER,
		};
		enum MSHLFLAGS 
		{	
			MSHLFLAGS_NORMAL	= 0,
			MSHLFLAGS_TABLESTRONG	= 1,
			MSHLFLAGS_TABLEWEAK	= 2,
			MSHLFLAGS_NOPING	= 4
		};
		enum MSHCTX 
		{	
			MSHCTX_LOCAL	= 0,
			MSHCTX_NOSHAREDMEM	= 1,
			MSHCTX_DIFFERENTMACHINE	= 2,
			MSHCTX_INPROC	= 3
		};


		FX_WINOLEAPI			FX_CLSIDFromString(wchar_t const* idstr, FX_CLSID* id);
		FX_WINOLEAPI_(uint32_t)	FX_CoBuildVersion( void );
		FX_WINOLEAPI			FX_CoCreateGuid(struct FX_GUID* pguid);
		FX_WINOLEAPI			FX_CoCreateInstance(FX_REFCLSID rclsid, IUnknown * pUnkOuter,uint32_t dwClsContext, FX_REFIID riid, void __RPC_FAR* __RPC_FAR* ppv);
		FX_WINOLEAPI			FX_CoCreateInstanceEx(FX_REFCLSID rclsid, IUnknown * pUnkOuter,uint32_t dwClsContext, struct COSERVERINFO * pServerInfo,uint32_t cmq, struct MULTI_QI * pResults);
//		FX_WINOLEAPI			FX_CoCreateStandardMalloc(uint32_t memctx, IMalloc ** ppMalloc);
		FX_WINOLEAPI_(void)		FX_CoFreeAllLibraries( void );
		FX_WINOLEAPI_(void)		FX_CoFreeLibrary(void* hInst );
		FX_WINOLEAPI_(void)		FX_CoFreeUnusedLibraries( void );
		FX_WINOLEAPI			FX_CoGetClassObject(FX_REFCLSID rclsid,uint32_t dwClsContext, struct COSERVERINFO * pServerInfo,FX_REFIID riid,void** ppv);
		FX_WINOLEAPI_(uint32_t)	FX_CoGetCurrentProcess(void);
		FX_WINOLEAPI			FX_CoGetInterfaceAndReleaseStream(IStream * pStm, FX_REFIID riid, void** ppv);
		FX_WINOLEAPI			FX_CoGetMalloc(uint32_t dwMemContext, IMalloc ** lpMalloc);
		FX_WINOLEAPI			FX_CoGetMarshalSizeMax(uint32_t* pulSize, FX_REFIID riid, IUnknown * pUnk,uint32_t dwDestContext, void *pvDestContext, uint32_t mshlFlags);
		FX_WINOLEAPI			FX_CoGetStandardMarshal(FX_REFIID riid,IUnknown * pUnk,uint32_t dwDestContext,void* pvDestContext,uint32_t mshlflags,IMarshal ** ppMarshal);
		FX_WINOLEAPI_(void*)	FX_CoLoadLibrary(wchar_t const* lpszLibName, uint16_t bAutoFree /* ignored */ );
		FX_WINOLEAPI			FX_CoInitialize(void* pvReserved);
		FX_WINOLEAPI			FX_CoInitializeEx(void* pvReserved, uint32_t dwCoInit );
		FX_WINOLEAPI			FX_CoMarshalInterface( IStream * pStm, FX_REFIID riid, IUnknown * pUnk, uint32_t dwDestContext, void * pvDestContext, uint32_t mshlflags );
		FX_WINOLEAPI			FX_CoMarshalInterThreadInterfaceInStream( FX_REFIID riid, IUnknown * pUnk, IStream ** ppStm );
		FX_WINOLEAPI			FX_CoReleaseMarshalData( IStream * pStm );
		FX_WINOLEAPI			FX_CoRegisterMallocSpy(IMallocSpy * pMallocSpy);
		FX_WINOLEAPI			FX_CoRevokeMallocSpy(void);
		FX_WINOLEAPI_(void*)	FX_CoTaskMemAlloc(uint32_t size);
		FX_WINOLEAPI_(void)		FX_CoTaskMemFree(void* ptr);
		FX_WINOLEAPI_(void*)	FX_CoTaskMemRealloc(void* ptr,uint32_t size);
		FX_WINOLEAPI_(void)		FX_CoUninitialize(void);
		FX_WINOLEAPI			FX_CoUnmarshalInterface(IStream * pStm, FX_REFIID riid,void ** ppv );

		FX_WINOLEAPI_(int)		FX_StringFromGUID2(FX_REFGUID rguid, wchar_t* lpsz, int cbMax);

		//////////////////////////////////////////////////////
		// New Cross-platform calls
		// *Static link*
		//////////////////////////////////////////////////////
		FX_WINOLEAPI  DllRegisterServerImpl(FX_REFCLSID clsid, wchar_t const* name, enum THREADINGMODEL tm, wchar_t const* VersionIndependentProgId, wchar_t const* ProgId );
		FX_WINOLEAPI  DllUnregisterServerImpl(FX_REFCLSID clsid);
		FX_WINOLEAPI  DllCanUnloadNowImpl(void);
		FX_WINOLEAPI  DllLockServerImpl( int32_t fLock );

		//////////////////////////////////////////////////////
		// Automation Api
		//////////////////////////////////////////////////////

		FX_WINOLEAUTAPI		FX_CreateErrorInfo(ICreateErrorInfo ** pperrinfo);
		FX_WINOLEAUTAPI		FX_GetErrorInfo(uint32_t dwReserved, IErrorInfo ** pperrinfo);
		FX_WINOLEAUTAPI		FX_SetErrorInfo(uint32_t dwReserved, IErrorInfo * perrinfo);

		FX_WINOLEAUTAPI_(FX_BSTR)		FX_SysAllocString(const wchar_t *);
		FX_WINOLEAUTAPI_(int16_t)	FX_SysReAllocString(FX_BSTR *, const wchar_t *);
		FX_WINOLEAUTAPI_(FX_BSTR)		FX_SysAllocStringLen(const wchar_t *, uint16_t);
		FX_WINOLEAUTAPI_(int16_t)	FX_SysReAllocStringLen(FX_BSTR *, const wchar_t *, uint16_t);
		FX_WINOLEAUTAPI_(void)		FX_SysFreeString(FX_BSTR);
		FX_WINOLEAUTAPI_(uint16_t)	FX_SysStringLen(FX_BSTR);
		FX_WINOLEAUTAPI_(uint16_t)	FX_SysStringByteLen(FX_BSTR bstr);
		FX_WINOLEAUTAPI_(FX_BSTR)		FX_SysAllocStringByteLen(const char* psz, uint16_t len);

		//////////////////////////////////////////////////////
		// Non-standard FX_BSTR Api
		//////////////////////////////////////////////////////

		FX_WINOLEAUTAPI_(FX_BSTR)		FX_SysAllocStringA( char const* );
		FX_WINOLEAUTAPI_(FX_BSTR)		FX_SysAllocStringFromCLSID( FX_REFCLSID clsid );

		//////////////////////////////////////////////////////
		// regsrv32
		//////////////////////////////////////////////////////
		FX_WINOLEAPI				FX_RegSrv32A(const char* filename,bool_t bRegister);
		FX_WINOLEAPI				FX_RegSrv32W(const wchar_t* filename,bool_t bRegister);


#ifdef __cplusplus
	}// namespace com
}; // namespace firtex
#endif

#include "ComInterface.h"

#endif // _COM_H_
