#include "com/Com.h"
#include "com/comerror.h"
#include <assert.h>
#include <pthread.h>
#include <stdlib.h>
#include "com/linux/git.h"
#include "com/linux/internal.h"

struct StdGITEntry
{
	uint32_t		cookie;
	firtex::com::FX_IID		iid;	// FX_IID of the interface
	firtex::com::IStream*	stream;	// Holds the marshalled interface */

	struct StdGITEntry* next;
	struct StdGITEntry* prev;  
};

namespace firtex
{
	namespace com 
	{
		namespace git 
		{
			FX_CLSID clsid = { 0x00000323, 0, 0, {0xc0, 0, 0, 0, 0, 0, 0, 0x46} };

			class StdGlobalInterfaceTableImpl : public IGlobalInterfaceTable
			{
			public:
				StdGlobalInterfaceTableImpl()
				{ 
					ref = 1; 
					firstEntry=0; 
					lastEntry=0; 
					nextCookie=0xf100; // that's where windows starts, so that's where we start
				};

				FX_STDMETHOD(QueryInterface)(FX_REFIID riid, void ** ppvObject );
				uint32_t   AddRef();
				uint32_t   Release();
				FX_STDMETHOD(RegisterInterfaceInGlobal)( 
					/* [in] */ IUnknown __RPC_FAR *pUnk,
					/* [in] */ FX_REFIID riid,
					/* [out] */ uint32_t __RPC_FAR *pdwCookie);
					FX_STDMETHOD(RevokeInterfaceFromGlobal)(uint32_t dwCookie);
				FX_STDMETHOD(GetInterfaceFromGlobal)(
					/* [in] */ uint32_t dwCookie,
					/* [in] */ FX_REFIID riid,
					/* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppv);

					StdGITEntry* FindEntry( uint32_t cookie );

			private:
				uint32_t		ref;
				StdGITEntry*	firstEntry;
				StdGITEntry*	lastEntry;
				uint32_t		nextCookie; 
			};

			class ClassFactory : public IClassFactory
			{
			public:
				FX_STDMETHOD(QueryInterface)( FX_IID const& riid, void** ptr );
				uint32_t   AddRef();
				uint32_t   Release();
				FX_STDMETHOD(LockServer)(int32_t fLock);
				FX_STDMETHOD(CreateInstance)( IUnknown* pUnkOuter, FX_REFIID riid, void** ppvObject );
			};

		};
	};
};

static firtex::com::git::StdGlobalInterfaceTableImpl* instance = 0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


StdGITEntry* firtex::com::git::StdGlobalInterfaceTableImpl::FindEntry(uint32_t cookie) 
{
	StdGITEntry* e;

	internal::pthread_mutex_lock lock(&mutex);

	e = firstEntry;
	while ( e ) {
		if (e->cookie == cookie) return e;
		e = e->next;
	}
  
  
	assert( !e );
	return 0;
}

firtex::com::FX_HRESULT firtex::com::git::StdGlobalInterfaceTableImpl::QueryInterface(FX_REFIID riid, void** ppvObject) 
{
	/* Make sure silly coders can't crash us */
	if (ppvObject == 0) return FX_E_INVALIDARG;

	*ppvObject = 0; /* assume we don't have the interface */

	/* Do we implement that interface? */
	if (FX_IsEqualIID( IUnknown::iid, riid) ) {
		*ppvObject = static_cast<firtex::com::IUnknown*>(this);
	} 
	else if ( FX_IsEqualIID( IGlobalInterfaceTable::iid, riid)) {
		*ppvObject = static_cast<firtex::com::IGlobalInterfaceTable*>(this);
	} 
	else return FX_E_NOINTERFACE;

	/* Now inc the refcount */
	AddRef();
	return FX_S_OK;
}

uint32_t firtex::com::git::StdGlobalInterfaceTableImpl::AddRef() 
{
  /* InterlockedIncrement(&self->ref); */
  return ref;
}

uint32_t  firtex::com::git::StdGlobalInterfaceTableImpl::Release() 
{
  /* InterlockedDecrement(&self->ref); */
  if (ref == 0) {
    /* Hey ho, it's time to go, so long again 'till next weeks show! */
    delete this;
    return 0;
  }

  return ref;
}

firtex::com::FX_HRESULT firtex::com::git::StdGlobalInterfaceTableImpl::RegisterInterfaceInGlobal(
	IUnknown* pUnk, FX_REFIID riid, uint32_t * pdwCookie) 
{

	if (pUnk == NULL) return FX_E_INVALIDARG;

	
	// marshal the interface
	IStream* stream = NULL;
	FX_HRESULT hr = FX_CoMarshalInterThreadInterfaceInStream(riid, pUnk, &stream);
	if ( FX_FAILED(hr) ) return hr;
	assert( stream );

	StdGITEntry* entry = reinterpret_cast<StdGITEntry*>( malloc( sizeof(StdGITEntry) ) );
	if ( !entry ) return FX_E_OUTOFMEMORY;

	internal::pthread_mutex_lock lock( &mutex );
  
	entry->iid = riid;
	entry->stream = stream;
	entry->cookie = nextCookie;
	++nextCookie; // inc the cookie count

	// insert the new entry at the end of the list
	entry->next = NULL;
	entry->prev = lastEntry;
	if (entry->prev) 
		entry->prev->next = entry;
	else 
		firstEntry = entry;
	lastEntry = entry;

	// and return the cookie
	*pdwCookie = entry->cookie;
  
	return FX_S_OK;
}

firtex::com::FX_HRESULT firtex::com::git::StdGlobalInterfaceTableImpl::RevokeInterfaceFromGlobal( uint32_t dwCookie) 
{
	StdGITEntry* entry = FindEntry(dwCookie);
	if ( !entry ) return FX_E_INVALIDARG; /* not found */
  
	// Free the stream
	entry->stream->Release();
		    
	/* chop entry out of the list, and free the memory */
	internal::pthread_mutex_lock lock( &mutex );
	if (entry->prev) 
	  entry->prev->next = entry->next;
	else 
	  firstEntry = entry->next;
	if (entry->next) 
	  entry->next->prev = entry->prev;
	else 
		lastEntry = entry->prev;


	free( entry );
	return FX_S_OK;
}

firtex::com::FX_HRESULT firtex::com::git::StdGlobalInterfaceTableImpl::GetInterfaceFromGlobal(uint32_t dwCookie, FX_REFIID riid, void **ppv) 
{
	StdGITEntry* entry = FindEntry( dwCookie );
 	if ( !entry ) return FX_E_INVALIDARG;
  
	if ( !FX_IsEqualIID( entry->iid, riid) ) {
 		//WARN("entry->iid (%s) != riid\n", debugstr_guid(&entry->iid));
		return FX_E_INVALIDARG;
	}
  
	// unmarshal the interface
	FX_HRESULT hr = FX_CoUnmarshalInterface( entry->stream, riid, ppv );
	if ( FX_FAILED(hr) ) {
		//WARN("Failed to unmarshal stream\n");
		return hr;
	}
  
	// rewind stream, in case it's used again
	entry->stream->Seek( 0, IStream::STREAM_SEEK_SET, NULL );

	// addref it
	IUnknown* lpUnk = reinterpret_cast<IUnknown*>( *ppv );
	lpUnk->AddRef();
	return FX_S_OK;
}

/* Classfactory definition - despite what MSDN says, some programs need this */

firtex::com::FX_HRESULT firtex::com::git::ClassFactory::QueryInterface(FX_REFIID riid, void** ppv) 
{
	if ( !ppv ) return FX_E_INVALIDARG;
	*ppv = NULL;
	if ( FX_IsEqualIID(riid,IUnknown::iid) )
		*ppv = (IUnknown*)this;
	else if ( FX_IsEqualIID(riid,IGlobalInterfaceTable::iid) ) 
		*ppv = (IGlobalInterfaceTable*)this;
	else
		return FX_E_NOINTERFACE;

    return FX_S_OK;
}

uint32_t firtex::com::git::ClassFactory::AddRef() { return 2; }

uint32_t firtex::com::git::ClassFactory::Release() { return 1; }

firtex::com::FX_HRESULT firtex::com::git::ClassFactory::CreateInstance(firtex::com::IUnknown* pUnk, firtex::com::FX_REFIID riid, void** ppv) 
{
	if ( FX_IsEqualIID( riid,IGlobalInterfaceTable::iid ) ) {
		assert( instance );
		return instance->QueryInterface( riid, ppv );
	}

	return FX_E_NOINTERFACE;
}

firtex::com::FX_HRESULT firtex::com::git::ClassFactory::LockServer(int32_t fLock) 
{
	internal::log_printf( internal::ERROR, "(%d) - FIXME firtex::com::git::ClassFactory::LockServer\n", fLock);
	return FX_S_OK;
}

firtex::com::FX_HRESULT GetFactory(void** ppv) 
{
	static firtex::com::git::ClassFactory factory;

	*ppv = &factory;
	return FX_S_OK;
}

firtex::com::FX_HRESULT firtex::com::git::Initialize()
{
	assert( !instance );
	
	try {
		instance = new firtex::com::git::StdGlobalInterfaceTableImpl();
	}
	catch (...) {
		return FX_E_OUTOFMEMORY;
	}

	return FX_S_OK;
}

void firtex::com::git::Uninitialize()
{
	delete instance;
	instance = 0;
}

firtex::com::FX_HRESULT firtex::com::git::QueryInterface( FX_REFIID iid, void** ppv )
{
	assert( instance );
	return instance->QueryInterface( iid, ppv );
}


