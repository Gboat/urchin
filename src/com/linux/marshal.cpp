#include "com/Com.h"
#include "com/comerror.h"
#include <assert.h>
#include <unistd.h>
#include "com/linux/internal.h"

static firtex::com::FX_CLSID const CLSID_DfMarshal       = { 0x0000030b, 0, 0, {0xc0, 0, 0, 0, 0, 0, 0, 0x46} };

namespace firtex
{
	namespace com 
	{
		namespace internal 
		{

			struct marshal_data 
			{
				uint32_t dwDestContext;
				uint32_t mshlflags;
			};

			class StdMarshalImpl : public IMarshal 
			{
			public:
				FX_STDMETHOD(QueryInterface)(FX_REFIID riid, void** ptr );
				FX_STDMETHOD_(uint32_t,AddRef)();
				FX_STDMETHOD_(uint32_t,Release)();
				FX_STDMETHOD(GetUnmarshalClass)(FX_REFIID riid,void __RPC_FAR *pv,uint32_t dwDestContext,void __RPC_FAR *pvDestContext,uint32_t mshlflags,FX_CLSID __RPC_FAR *pCid);
				FX_STDMETHOD(GetMarshalSizeMax)(FX_REFIID riid,void __RPC_FAR *pv,uint32_t dwDestContext,void __RPC_FAR *pvDestContext,uint32_t mshlflags,uint32_t __RPC_FAR *pSize);
				FX_STDMETHOD(MarshalInterface)(IStream __RPC_FAR *pStm,FX_REFIID riid,void __RPC_FAR *pv,uint32_t dwDestContext,void __RPC_FAR *pvDestContext,uint32_t mshlflags);
				FX_STDMETHOD(UnmarshalInterface)(IStream __RPC_FAR *pStm,FX_REFIID riid,void __RPC_FAR *__RPC_FAR *ppv);
				FX_STDMETHOD(ReleaseMarshalData)(IStream __RPC_FAR *pStm);
				FX_STDMETHOD(DisconnectObject)(uint32_t dwReserved);

				uint32_t	ref;
				FX_IID		iid;
				uint32_t	dwDestContext;
				void*		pvDestContext;
				uint32_t	mshlflags;
			};

		};
	};
};

firtex::com::FX_HRESULT firtex::com::internal::StdMarshalImpl::QueryInterface(FX_REFIID riid,void** ppv) 
{
	if ( !ppv ) return FX_E_INVALIDARG;
	*ppv = 0;

	if ( FX_IsEqualIID(IUnknown::iid,riid) ) {
		*ppv = static_cast<IUnknown*>(this);
		AddRef();
		return FX_S_OK;
	}
	if ( FX_IsEqualIID(IMarshal::iid,riid) ) {
		*ppv = static_cast<IMarshal*>(this);
		AddRef();
		return FX_S_OK;
	}

	return FX_E_NOINTERFACE;
}

uint32_t firtex::com::internal::StdMarshalImpl::AddRef()
{
	// need thread protection
	++ref;
	return ref;
}

uint32_t firtex::com::internal::StdMarshalImpl::Release()
{
	uint32_t tmp = --ref;
	if ( !ref ) delete this;
	return tmp;
}

firtex::com::FX_HRESULT firtex::com::internal::StdMarshalImpl::GetUnmarshalClass(
	FX_REFIID riid, void* pv, uint32_t dwDestContext,
	void* pvDestContext, uint32_t mshlflags, FX_CLSID* pCid )
{
	if ( !pCid ) return FX_E_INVALIDARG;
	memcpy( pCid, &CLSID_DfMarshal, sizeof(FX_CLSID) );
	return FX_S_OK;
}

firtex::com::FX_HRESULT firtex::com::internal::StdMarshalImpl::GetMarshalSizeMax(
	FX_REFIID riid, void* pv, uint32_t dwDestContext,
	void* pvDestContext, uint32_t mshlflags, uint32_t* pSize)
{
	if ( !pSize ) return FX_E_INVALIDARG;
	*pSize = sizeof( internal::marshal_id) + sizeof( internal::marshal_data );
	return FX_S_OK;
}

firtex::com::FX_HRESULT firtex::com::internal::StdMarshalImpl::MarshalInterface(
	IStream *pStm,FX_REFIID riid, void* pv, uint32_t dwDestContext,
	void* pvDestContext, uint32_t mshlflags)
{
	internal::marshal_id	mid;
	internal::marshal_data 	md;
	IUnknown		*pUnk = 0;
	uint32_t			res;
	FX_HRESULT		hr;

	if ( !pStm ) return FX_E_INVALIDARG;
	if ( !pv ) return FX_E_INVALIDARG;
	hr = reinterpret_cast<IUnknown*>(pv)->QueryInterface( IUnknown::iid, (void**)&pUnk );
	assert( FX_SUCCEEDED(hr) );
	mid.processid = getpid();
	//mid.objectid = reinterpret_cast<uint32_t>(pUnk); /* FIXME */
	mid.objectid = (uint32_t)reinterpret_cast<uint64_t>(pUnk); /* FIXME */
	pUnk->Release();
	pUnk = 0;

	memcpy( &mid.iid, &riid, sizeof(mid.iid) );
	md.dwDestContext	= dwDestContext;
	md.mshlflags		= mshlflags;

	hr = pStm->Write( &mid, sizeof(mid), &res );
	if ( FX_FAILED(hr) ) return hr;
	hr = pStm->Write( &md, sizeof(md), &res );
	if ( FX_FAILED(hr) ) return hr;

	return FX_E_FAIL;  	

	/*
	IRpcStubBuffer	*stub;
	IPSFactoryBuffer	*psfacbuf;
	if (FX_SUCCEEDED(MARSHAL_Find_Stub_Buffer(&mid,&stub))) {
	// Find_Stub_Buffer gives us a ref but we want to keep it, as if we'd created a new one
	TRACE("Found RpcStubBuffer %p\n", stub);
	return FX_S_OK;
	}
	hres = get_facbuf_for_iid(riid,&psfacbuf);
	if (hres) return hres;

	hres = IPSFactoryBuffer_CreateStub(psfacbuf,riid,pv,&stub);
	IPSFactoryBuffer_Release(psfacbuf);
	if (hres) {
	FIXME("Failed to create a stub for %s\n",debugstr_guid(riid));
	return hres;
	}
	IUnknown_QueryInterface((LPUNKNOWN)pv,riid,(LPVOID*)&pUnk);
	MARSHAL_Register_Stub(&mid,pUnk,stub);
	IUnknown_Release(pUnk);
	return FX_S_OK;
	*/
}

firtex::com::FX_HRESULT firtex::com::internal::StdMarshalImpl::UnmarshalInterface(
	IStream *pStm, FX_REFIID riid, void **ppv) 
{
	internal::marshal_id       mid;
	internal::marshal_data     md;
	uint32_t			res;
	FX_HRESULT		hr;

	if ( !pStm ) return FX_E_INVALIDARG;

	hr = pStm->Read( &mid, sizeof(mid), &res );
	if ( FX_FAILED(hr) ) return hr;
	hr = pStm->Read( &md, sizeof(md), &res );
	if ( FX_FAILED(hr) ) return hr;

	return FX_E_FAIL;

	/*	
	IPSFactoryBuffer	*psfacbuf;
	IRpcProxyBuffer	*rpcproxy;
	IRpcChannelBuffer	*chanbuf;
	if (FX_SUCCEEDED(MARSHAL_Find_Stub(&mid,(LPUNKNOWN*)ppv))) {
	FIXME("Calling back to ourselves for %s!\n",debugstr_guid(riid));
	return FX_S_OK;
	}
	if (FX_IsEqualIID(riid, &IID_IUnknown) || FX_IsEqualIID(riid, &IID_NULL)) {
	// should return proxy manager IUnknown object 
	FIXME("Special treatment required for FX_IID of %s\n", debugstr_guid(riid));
	}
	hres = get_facbuf_for_iid(riid,&psfacbuf);
	if (hres) return hres;
	hres = IPSFactoryBuffer_CreateProxy(psfacbuf,NULL,riid,&rpcproxy,ppv);
	if (hres) {
	FIXME("Failed to create a proxy for %s\n",debugstr_guid(riid));
	return hres;
	}

	MARSHAL_Register_Proxy(&mid, (LPUNKNOWN) rpcproxy);

	hres = PIPE_GetNewPipeBuf(&mid,&chanbuf);
	IPSFactoryBuffer_Release(psfacbuf);
	if (hres) {
	ERR("Failed to get an rpc channel buffer for %s\n",debugstr_guid(riid));
	} else {
	/* Connect the channel buffer to the proxy and release the no longer
	* needed proxy.
	* NOTE: The proxy should have taken an extra reference because it also
	* aggregates the object, so we can safely release our reference to it.
	IRpcProxyBuffer_Connect(rpcproxy,chanbuf);
	IRpcProxyBuffer_Release(rpcproxy);
	// IRpcProxyBuffer takes a reference on the channel buffer and
	// we no longer need it, so release it
	IRpcChannelBuffer_Release(chanbuf);
	}
	return hres;
	*/
}

firtex::com::FX_HRESULT firtex::com::internal::StdMarshalImpl::ReleaseMarshalData(firtex::com::IStream *pStm) 
{
	internal::marshal_id       mid;
	uint32_t                 res;
	FX_HRESULT               hr;

	if ( !pStm ) return FX_E_INVALIDARG;

	hr = pStm->Read( &mid, sizeof(mid), &res );
	if ( FX_FAILED(hr) ) return hr;

	return FX_E_FAIL;
	/*
	IRpcStubBuffer       *stub = NULL;
	int                   i;
	for (i=0; i < nrofstubs; i++){
	if (!stubs[i].valid) continue;

	if (MARSHAL_Compare_Mids(&mid, &(stubs[i].mid)))
	{
	stub = stubs[i].stub;
	break;
	}
	}

	if (!stub)
	{
	FIXME("Could not map MID to stub??\n");
	return FX_E_FAIL;
	}

	res = IRpcStubBuffer_Release(stub);
	stubs[i].valid = FALSE;
	TRACE("stub refcount of %p is %ld\n", stub, res);
	*/
	return FX_S_OK;
}

firtex::com::FX_HRESULT firtex::com::internal::StdMarshalImpl::DisconnectObject(uint32_t dwReserved) 
{
	internal::log_printf( internal::ERROR, "(), stub!\n" );
	return FX_S_OK;
}


firtex::com::FX_HRESULT firtex::com::FX_CoGetStandardMarshal(
	FX_REFIID riid,IUnknown *pUnk,uint32_t dwDestContext,void* pvDestContext,
	uint32_t mshlflags, IMarshal** pMarshal)
{
	internal::StdMarshalImpl* dm = 0;

	if ( !pMarshal ) return FX_E_INVALIDARG;

	if (pUnk == NULL) {
		internal::log_printf( internal::ERROR, 
			"(%s,NULL,%lx,%p,%lx,%p), unimplemented yet.\n",
			/*debugstr_guid(riid)*/"refiid",dwDestContext,pvDestContext,mshlflags,pMarshal );
			return FX_E_FAIL;
	}

	try {
		dm = new internal::StdMarshalImpl();
	}
	catch (...) {
		*pMarshal = 0;
		return FX_E_OUTOFMEMORY;
	}

	*pMarshal = dm;
	assert( *pMarshal );

	memcpy( &dm->iid, &riid, sizeof(dm->iid) );
	dm->dwDestContext	= dwDestContext;
	dm->pvDestContext	= pvDestContext;
	dm->mshlflags		= mshlflags;
	return FX_S_OK;
}

/* Helper function for getting Marshaler */
static firtex::com::FX_HRESULT GetMarshaller(firtex::com::FX_REFIID riid, firtex::com::IUnknown *pUnk,uint32_t dwDestContext,void *pvDestContext, uint32_t mshlFlags, firtex::com::IMarshal** pMarshal ) 
{

	if (!pUnk) return FX_E_POINTER;

	firtex::com::FX_HRESULT hr = pUnk->QueryInterface( firtex::com::IMarshal::iid, (void**)pMarshal );
	if ( FX_SUCCEEDED(hr) ) return hr;

	return firtex::com::FX_CoGetStandardMarshal(riid,pUnk,dwDestContext,pvDestContext,mshlFlags,pMarshal);
}

firtex::com::FX_HRESULT firtex::com::FX_CoGetMarshalSizeMax(uint32_t* pulSize, FX_REFIID riid, IUnknown *pUnk,uint32_t dwDestContext, void *pvDestContext, uint32_t mshlFlags)
{
	IMarshal* pMarshal = 0;

	if ( !pulSize ) return FX_E_INVALIDARG;

	FX_HRESULT hr = GetMarshaller(riid,pUnk,dwDestContext,pvDestContext,mshlFlags,&pMarshal);
	if ( FX_FAILED(hr) ) return hr;
	assert( pMarshal );

	hr = pMarshal->GetMarshalSizeMax(riid,pUnk,dwDestContext,pvDestContext,mshlFlags,pulSize);
	*pulSize += sizeof(internal::marshal_id)+sizeof(internal::marshal_data)+sizeof(FX_CLSID);
	pMarshal->Release();
	return hr;
}


/***********************************************************************
*		FX_CoMarshalInterface	[OLE32.@]
*/
firtex::com::FX_HRESULT firtex::com::FX_CoMarshalInterface( IStream *pStm, FX_REFIID riid, IUnknown *pUnk,uint32_t dwDestContext, void *pvDestContext, uint32_t mshlflags)
{
	IMarshal*		pMarshal = 0;
	FX_CLSID			xclsid;
	uint32_t		writeres;
	internal::marshal_id	mid;
	internal::marshal_data 	md;
	uint32_t		res;
	IUnknown*		pUnknown = 0;

	if ( !pUnk ) return FX_E_INVALIDARG;
	if ( !pStm ) return FX_E_INVALIDARG;

	//? STUBMGR_Start(); /* Just to be sure we have one running. */
	mid.processid = getpid(); //GetCurrentProcessId();
	FX_HRESULT hr = pUnk->QueryInterface( IUnknown::iid, (void**)&pUnknown );
	assert( FX_SUCCEEDED(hr) );
	/*mid.objectid = reinterpret_cast<uint32_t>(pUnknown);*/
	mid.objectid = (uint32_t)reinterpret_cast<uint64_t>(pUnknown);
	pUnknown->Release();
	memcpy( &mid.iid, &riid, sizeof(mid.iid) );

	md.dwDestContext	= dwDestContext;
	md.mshlflags		= mshlflags;

	hr = pStm->Write( &mid, sizeof(mid), &res );
	if ( FX_FAILED(hr) ) return hr;
	hr = pStm->Write( &md, sizeof(md), &res );
	if ( FX_FAILED(hr) ) return hr;

	hr = GetMarshaller( riid,pUnk,dwDestContext,pvDestContext,mshlflags,&pMarshal);
	if ( FX_FAILED(hr) ) return hr;
	assert( pMarshal );

	hr = pMarshal->GetUnmarshalClass(riid,pUnk,dwDestContext,pvDestContext,mshlflags,&xclsid);
	if ( FX_FAILED(hr) ) {
		pMarshal->Release();
		return hr;
	}
	hr = pStm->Write( &xclsid, sizeof(xclsid), &writeres );
	if ( FX_FAILED(hr) ) {
		pMarshal->Release();
		return hr;
	}

	hr = pMarshal->MarshalInterface(pStm,riid,pUnk,dwDestContext,pvDestContext,mshlflags);
	if ( FX_FAILED(hr) ) {
		pMarshal->Release();
		return hr;
	}

	pMarshal->Release();
	return FX_S_OK;
}

firtex::com::FX_HRESULT firtex::com::FX_CoUnmarshalInterface(firtex::com::IStream *pStm, FX_REFIID riid, void** ppv) 
{
	internal::marshal_id	mid;
	internal::marshal_data	md;
	uint32_t		res;
	IMarshal*		pMarshal = 0;
	IUnknown*		pUnk = 0;
	FX_CLSID			xclsid;

	if ( !pStm ) return FX_E_INVALIDARG;

	FX_HRESULT hr = pStm->Read( &mid, sizeof(mid), &res );
	if ( FX_FAILED(hr) ) return hr;
	hr = pStm->Read( &md, sizeof(md), &res );
	if ( FX_FAILED(hr) ) return hr;
	hr = pStm->Read( &xclsid, sizeof(xclsid), &res );
	if ( FX_FAILED(hr) ) return hr;

	hr = FX_CoCreateInstance(xclsid,NULL,FX_CLSCTX_INPROC_SERVER | FX_CLSCTX_INPROC_HANDLER | FX_CLSCTX_LOCAL_SERVER,IMarshal::iid,(void**)&pUnk);
	if ( FX_FAILED(hr) ) return hr;
	assert( pUnk );

	hr = GetMarshaller(riid,pUnk,md.dwDestContext,NULL,md.mshlflags,&pMarshal);
	if ( FX_FAILED(hr) ) return hr;
	assert( pMarshal );

	hr = pMarshal->UnmarshalInterface( pStm, riid, ppv );
	pMarshal->Release();
	return hr;
}


firtex::com::FX_HRESULT firtex::com::FX_CoReleaseMarshalData(IStream *pStm) 
{
	internal::marshal_id	mid;
	internal::marshal_data	md;
	uint32_t		res;
	IMarshal*		pMarshal = 0;
	IUnknown*		pUnk = 0;
	FX_CLSID			xclsid;

	if ( !pStm ) return FX_E_INVALIDARG;

	FX_HRESULT hr = pStm->Read( &mid, sizeof(mid), &res);
	if ( FX_FAILED(hr) ) return hr;
	hr = pStm->Read( &md, sizeof(md), &res);
	if ( FX_FAILED(hr) ) return hr;
	hr = pStm->Read( &xclsid, sizeof(xclsid), &res );
	if ( FX_FAILED(hr) ) return hr;

	hr = FX_CoCreateInstance( xclsid, NULL, FX_CLSCTX_INPROC_SERVER | FX_CLSCTX_INPROC_HANDLER | FX_CLSCTX_LOCAL_SERVER, IMarshal::iid, (void**)&pUnk );
	if ( FX_FAILED(hr) ) return hr;
	assert( pUnk );

	hr = pUnk->QueryInterface( IMarshal::iid, (void**)&pMarshal );
	if ( FX_FAILED(hr) ) return hr;
	assert( pMarshal );

	hr = pMarshal->ReleaseMarshalData( pStm );
	if ( FX_FAILED(hr) ) return hr;

	pMarshal->Release();
	pUnk->Release();

	return FX_S_OK;
}

firtex::com::FX_HRESULT firtex::com::FX_CoMarshalInterThreadInterfaceInStream(FX_REFIID riid, IUnknown* pUnk, IStream** ppStm)
{
	if ( !ppStm ) return FX_E_INVALIDARG;

	FX_HRESULT hr = FX_E_FAIL;//FX_HRESULT hr = CreateStreamOnHGlobal(/*0, TRUE,*/ ppStm);
	if ( FX_FAILED(hr) ) return hr;
	hr = FX_CoMarshalInterface(*ppStm, riid, pUnk, MSHCTX_INPROC, 0, MSHLFLAGS_NORMAL);
	if ( FX_FAILED(hr) ) return hr;

	assert( *ppStm );
	uint64_t	xpos;
	hr = (*ppStm)->Seek( 0, IStream::STREAM_SEEK_SET, &xpos );
	return hr;
}

firtex::com::FX_HRESULT firtex::com::FX_CoGetInterfaceAndReleaseStream(IStream* pStm,FX_REFIID riid, void** ppv)
{
	FX_HRESULT hr = FX_CoUnmarshalInterface(pStm, riid, ppv);
	pStm->Release();
	return hr;
}


