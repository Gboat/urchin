#include "com/Com.h"
#include "com/comerror.h"
#include <assert.h>
#include <pthread.h>
#include <stdlib.h>
#include "com/linux/internal.h"

namespace firtex
{
	namespace com 
	{
		namespace internal 
		{

			class ErrorInfo : public IErrorInfo, public ICreateErrorInfo, ISupportErrorInfo {
			public:
				ErrorInfo();
				~ErrorInfo();

				FX_STDMETHOD(QueryInterface)( firtex::com::FX_IID const& riid, void** ptr );
				uint32_t   AddRef();
				uint32_t   Release();
				FX_STDMETHOD(GetGUID)( firtex::com::FX_GUID* retVal );
				FX_STDMETHOD(GetSource)( firtex::com::FX_BSTR* retVal );
				FX_STDMETHOD(GetDescription)( firtex::com::FX_BSTR* retVal );
				FX_STDMETHOD(GetHelpFile)( firtex::com::FX_BSTR* retVal );
				FX_STDMETHOD(GetHelpContext)( uint32_t* retVal );
				FX_STDMETHOD(SetGUID)( firtex::com::FX_REFGUID newVal );
				FX_STDMETHOD(SetSource)( wchar_t const*  newVal );
				FX_STDMETHOD(SetDescription)( wchar_t const*  newVal );
				FX_STDMETHOD(SetHelpFile)( wchar_t const*  newVal );
				FX_STDMETHOD(SetHelpContext)( uint32_t newVal );
				FX_STDMETHOD(InterfaceSupportsErrorInfo)( firtex::com::FX_REFIID riid );

			private:
				uint32_t volatile	ref;
				firtex::com::FX_GUID		guid;
				firtex::com::FX_BSTR		source;
				firtex::com::FX_BSTR		description;
				firtex::com::FX_BSTR		helpfile;
				uint32_t	helpcontext;
				pthread_mutex_t mutex;
			};

		};
	};
};

//static firtex::com::FX_IID const IUnknown__iid = {0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}};
//static firtex::com::FX_IID const ICreateErrorInfo__iid = {0x22F03340,0x547D,0x101B,{0x8E,0x65,0x08,0x00,0x2B,0x2B,0xD1,0x19}};
//static firtex::com::FX_IID const IErrorInfo__iid = {0x1CF2B120,0x547D,0x101B,{0x8E,0x65,0x08,0x00,0x2B,0x2B,0xD1,0x19}};
//static firtex::com::FX_IID const ISupportErrorInfo__iid = {0xDF0B3D60,0x548F,0x101B,{0x8E,0x65,0x08,0x00,0x2B,0x2B,0xD1,0x19}};

firtex::com::internal::ErrorInfo::ErrorInfo()
{
	ref = 0;
	memset( &guid, 0, sizeof(guid) );
	source = 0;
	description = 0;
	helpfile = 0;
	helpcontext = 0;
	pthread_mutex_init( &mutex, 0 );
}

firtex::com::internal::ErrorInfo::~ErrorInfo()
{
	assert( ref == 0 );
	pthread_mutex_destroy( &mutex );
}

FX_STDMETHODIMP firtex::com::internal::ErrorInfo::QueryInterface( firtex::com::FX_IID const& riid, void** ptr )
{
	if ( !ptr ) return FX_E_INVALIDARG;
	
	if(FX_IsEqualIID(riid, IErrorInfo::iid))
	{
	  *ptr = static_cast<IErrorInfo*>(this);
	}
	else if(FX_IsEqualIID(riid, ICreateErrorInfo::iid))
	{
	  *ptr = static_cast<ICreateErrorInfo*>(this);
	}
	else if(FX_IsEqualIID(riid, ISupportErrorInfo::iid))
	{
	  *ptr = static_cast<ISupportErrorInfo*>(this);
	}
	else if(FX_IsEqualIID(riid, IUnknown::iid))
	{
	  *ptr = static_cast<IUnknown*>(static_cast<IErrorInfo*>(this));
	}

	if (*ptr) {
		reinterpret_cast<IUnknown*>(ptr)->AddRef();
	  	return FX_S_OK;
	}
	return FX_E_NOINTERFACE;
}

uint32_t firtex::com::internal::ErrorInfo::AddRef()
{
	::pthread_mutex_lock( &mutex );
	uint32_t retval = ++ref;
 	::pthread_mutex_unlock( &mutex );
	return retval;
}

uint32_t firtex::com::internal::ErrorInfo::Release()
{
	::pthread_mutex_lock( &mutex );
	uint32_t tmp = --ref;
	::pthread_mutex_unlock( &mutex );
	if ( tmp == 0 ) {
		delete this;
	}
	return tmp;
}

firtex::com::FX_HRESULT firtex::com::internal::ErrorInfo::GetGUID( firtex::com::FX_GUID * pGUID )
{
	if ( !pGUID ) return FX_E_INVALIDARG;
	memcpy(pGUID, &guid, sizeof(firtex::com::FX_GUID));
	return FX_S_OK;
}

firtex::com::FX_HRESULT firtex::com::internal::ErrorInfo::GetSource(firtex::com::FX_BSTR *retVal)
{
	if (retVal == NULL) return FX_E_INVALIDARG;
	*retVal = FX_SysAllocString(source);
	return *retVal ? FX_S_OK : FX_E_INVALIDARG;
}

firtex::com::FX_HRESULT firtex::com::internal::ErrorInfo::GetDescription(firtex::com::FX_BSTR *retVal)
{
	if (retVal == NULL) return FX_E_INVALIDARG;
	*retVal = FX_SysAllocString(description);
	return *retVal ? FX_S_OK : FX_E_INVALIDARG;
}

firtex::com::FX_HRESULT firtex::com::internal::ErrorInfo::GetHelpFile(firtex::com::FX_BSTR *retVal)
{
	if (retVal == NULL) return FX_E_INVALIDARG;
	*retVal = FX_SysAllocString(helpfile);
	return *retVal ? FX_S_OK : FX_E_INVALIDARG;
}

firtex::com::FX_HRESULT firtex::com::internal::ErrorInfo::GetHelpContext(uint32_t *retVal)
{
	if (retVal == NULL) return FX_E_INVALIDARG;
	*retVal = helpcontext;
	return FX_S_OK;
}

firtex::com::FX_HRESULT firtex::com::internal::ErrorInfo::SetGUID(firtex::com::FX_REFGUID rguid)
{
	memcpy(&guid, &rguid, sizeof(firtex::com::FX_GUID));
	return FX_S_OK;
}

firtex::com::FX_HRESULT firtex::com::internal::ErrorInfo::SetSource(wchar_t const* newVal)
{
	firtex::com::FX_BSTR cpy = FX_SysAllocString( newVal );
	if ( !cpy ) return FX_E_INVALIDARG;
	if ( source ) FX_SysFreeString( source );
	source = cpy;
	return FX_S_OK;
}

firtex::com::FX_HRESULT firtex::com::internal::ErrorInfo::SetDescription(wchar_t const* newVal)
{
	firtex::com::FX_BSTR cpy = FX_SysAllocString( newVal );
	if ( !cpy ) return FX_E_INVALIDARG;
	if ( description ) FX_SysFreeString( description );
	description = cpy;
	return FX_S_OK;
}

firtex::com::FX_HRESULT firtex::com::internal::ErrorInfo::SetHelpFile(wchar_t const* newVal)
{
	firtex::com::FX_BSTR cpy = FX_SysAllocString( newVal );
	if ( !cpy ) return FX_E_INVALIDARG;
	if ( helpfile ) FX_SysFreeString( helpfile );
	helpfile = cpy;
	return FX_S_OK;
}

firtex::com::FX_HRESULT firtex::com::internal::ErrorInfo::SetHelpContext(uint32_t newVal)
{
	helpcontext = newVal;
	return FX_S_OK;
}

firtex::com::FX_HRESULT firtex::com::internal::ErrorInfo::InterfaceSupportsErrorInfo(firtex::com::FX_REFIID riid)
{
	return FX_IsEqualIID(riid, guid) ? FX_S_OK : FX_S_FALSE;
}

/***********************************************************************
 *		CreateErrorInfo (OLE32.@)
 */
firtex::com::FX_HRESULT FX_STDAPICALLTYPE firtex::com::FX_CreateErrorInfo(ICreateErrorInfo **pperrinfo)
{
	if ( !pperrinfo ) return FX_E_INVALIDARG;
	
	firtex::com::IErrorInfo* pei = 0;
	try {
		pei = new firtex::com::internal::ErrorInfo();
	}
	catch (...) {
		return FX_E_INVALIDARG;
	};	
	if ( !pei ) return FX_E_INVALIDARG;
	
	firtex::com::FX_HRESULT hr = pei->QueryInterface( firtex::com::ICreateErrorInfo::iid, (void**)pperrinfo);
	pei->Release();
	return hr;
}

/***********************************************************************
 *		GetErrorInfo (OLE32.@)
 */
firtex::com::FX_HRESULT FX_STDAPICALLTYPE firtex::com::FX_GetErrorInfo(uint32_t dwReserved, firtex::com::IErrorInfo **pperrinfo)
{
	if(!pperrinfo) return FX_E_INVALIDARG;
	
	firtex::com::internal::apartment_t* apt = firtex::com::internal::GetThreadSpecificData()->apartment;

	if ( !apt || !apt->errorinfo ) {
	   *pperrinfo = NULL;
	   return FX_S_FALSE;
	}

	firtex::com::internal::pthread_mutex_lock lock( &apt->mutex );
	*pperrinfo = apt->errorinfo;
	/* clear thread error state */
	apt->errorinfo = NULL;
	return FX_S_OK;
}

/***********************************************************************
 *		SetErrorInfo (OLE32.@)
 */
firtex::com::FX_HRESULT FX_STDAPICALLTYPE firtex::com::FX_SetErrorInfo(uint32_t dwReserved, firtex::com::IErrorInfo *perrinfo)
{
	firtex::com::internal::apartment_t* apt = firtex::com::internal::GetThreadSpecificData()->apartment;

	if (!apt) {
		apt = firtex::com::internal::CreateApartment(COINIT_UNINITIALIZED);
	}
	assert( apt );

	firtex::com::internal::pthread_mutex_lock lock( &apt->mutex );
	
	/* release old errorinfo */
	firtex::com::IErrorInfo* pei = apt->errorinfo;
	if (pei) pei->Release();

	/* set to new value */
	apt->errorinfo = perrinfo;
	if (apt->errorinfo) apt->errorinfo->AddRef();
	return FX_S_OK;
}
