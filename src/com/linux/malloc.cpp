#include "com/comerror.h"
#include "com/Com.h"
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
			class Malloc : public IMalloc 
			{
			public:
				Malloc();
				~Malloc();

				FX_STDMETHOD(QueryInterface)( firtex::com::FX_IID const& riid, void** ptr );
				FX_STDMETHOD_(uint32_t,AddRef)();
				FX_STDMETHOD_(uint32_t,Release)();
				FX_STDMETHOD_(void*,Alloc)(uint32_t);
				FX_STDMETHOD_(void*,Realloc)(void*,uint32_t);
				FX_STDMETHOD_(void,Free)(void*);
				FX_STDMETHOD_(uint32_t,GetSize)(void*);
				FX_STDMETHOD_(int16_t,DidAlloc)(void*);
				FX_STDMETHOD_(void,HeapMinimize)();

			public:
				IMallocSpy* pSpy;          /* the spy when active */
				uint32_t SpyedAllocationsLeft; /* number of spyed allocations left */
				bool SpyReleasePending;     /* FX_CoRevokeMallocSpy called with spyed allocations left*/
				void** SpyedBlocks;       /* root of the table */
				int SpyedBlockTableLength;  /* size of the table*/
			};
		}
	}
}

static firtex::com::internal::Malloc malloc32;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	
/* resize the old table */
static firtex::com:: FX_HRESULT SetSpyedBlockTableLength ( int NewLength )
{
	void** NewSpyedBlocks = 0;

	if (! malloc32.SpyedBlocks ) {
		NewSpyedBlocks = (void**)malloc(NewLength*sizeof(void*));
		memset( NewSpyedBlocks, 0, NewLength*sizeof(void*) );
	}
	else {
		NewSpyedBlocks = (void**)realloc( malloc32.SpyedBlocks, NewLength*sizeof(void*));
		if ( NewLength > malloc32.SpyedBlockTableLength )
			memset( NewSpyedBlocks + malloc32.SpyedBlockTableLength, 0, (NewLength - malloc32.SpyedBlockTableLength)*sizeof(void*) );
	}

	if (NewSpyedBlocks) {
		malloc32.SpyedBlocks = NewSpyedBlocks;
		malloc32.SpyedBlockTableLength = NewLength;
		return FX_S_OK;
	}

	return FX_E_OUTOFMEMORY;
}

/* add a location to the table */
static firtex::com::FX_HRESULT AddMemoryLocation(void* pMem)
{
	/* allocate the table if not already allocated */
	if ( !malloc32.SpyedBlockTableLength ) {
		firtex::com::FX_HRESULT hr = SetSpyedBlockTableLength( 0x1000 );
		if ( FX_FAILED(hr) ) return hr;
	}

	/* find a free location */
	void** Current = malloc32.SpyedBlocks;
	while (*Current) {
		++Current;

	    if ( Current >= malloc32.SpyedBlocks + malloc32.SpyedBlockTableLength ) {
	    	/* no more space in table, grow it */
			firtex::com::FX_HRESULT hr = SetSpyedBlockTableLength( malloc32.SpyedBlockTableLength + 0x1000 );
			if ( FX_FAILED(hr) ) return hr;
	    }
	};

	/* put the location in our table */
	*Current = pMem;
	++malloc32.SpyedAllocationsLeft;
	return FX_S_OK;
}

static firtex::com::FX_HRESULT RemoveMemoryLocation(void* pMem)
{
	/* allocate the table if not already allocated */
	if ( !malloc32.SpyedBlockTableLength ) {
		firtex::com::FX_HRESULT hr = SetSpyedBlockTableLength( 0x1000 );
		if ( FX_FAILED(hr) ) return hr;
	}

	void** Current = malloc32.SpyedBlocks;

	/* find the location */
	while (*Current != pMem) {
		++Current;
	    if (Current >= malloc32.SpyedBlocks + malloc32.SpyedBlockTableLength)  
			return FX_E_FAIL;      /* not found  */
	}

	/* location found */
    --malloc32.SpyedAllocationsLeft;
	/*TRACE("%lu\n",Malloc32.SpyedAllocationsLeft);*/
	*Current = NULL;
	return FX_S_OK;
}

firtex::com::internal::Malloc::Malloc()
{
	pSpy = 0;          
	SpyedAllocationsLeft = 0; 
	SpyReleasePending = false;
	
	SpyedBlocks = 0;
	SpyedBlockTableLength = 0;
}

firtex::com::internal::Malloc::~Malloc()
{
	free( SpyedBlocks );
}


/******************************************************************************
 *	IMalloc32_QueryInterface	[VTABLE]
 */
firtex::com::FX_HRESULT firtex::com::internal::Malloc::QueryInterface(FX_REFIID refiid,void** obj) 
{
	if ( !obj ) return FX_E_INVALIDARG;

	if ( FX_IsEqualIID(IUnknown::iid,refiid) ) {
		*obj = static_cast<IUnknown*>(this);
		return FX_S_OK;
	}
	else if ( FX_IsEqualIID(IMalloc::iid,refiid) ) {
		*obj = static_cast<IMalloc*>(this);
		return FX_S_OK;
	}
	return FX_E_NOINTERFACE;
}

/******************************************************************************
 *	IMalloc32_AddRefRelease		[VTABLE]
 */
uint32_t firtex::com::internal::Malloc::AddRef() {
	return 1;
}

uint32_t firtex::com::internal::Malloc::Release() {
	return 1;
}

/******************************************************************************
 *	IMalloc32_Alloc 		[VTABLE]
 */
void* firtex::com::internal::Malloc::Alloc(uint32_t cb) 
{
	if ( malloc32.pSpy ) {
	    uint32_t preAllocResult;

		::pthread_mutex_lock( &mutex );
	    
	    preAllocResult = malloc32.pSpy->PreAlloc(cb);
	    if ((cb != 0) && (preAllocResult == 0)) {
			/* PreAlloc can force Alloc to fail, but not if cb == 0 */
			::pthread_mutex_unlock( &mutex );
			return NULL;
	    }
	}
 	
	void* addr = malloc(cb);

	if ( malloc32.pSpy ) {
	    addr = malloc32.pSpy->PostAlloc(addr);
	    if (addr) AddMemoryLocation(addr);
		::pthread_mutex_unlock( &mutex );
	}

	return addr;
}

/******************************************************************************
 * IMalloc32_Realloc [VTABLE]
 */
void* firtex::com::internal::Malloc::Realloc(void* pv,uint32_t cb) 
{
	if ( malloc32.pSpy ) {
	    void* pRealMemory;
	    bool fSpyed;

		::pthread_mutex_lock( &mutex );

		FX_HRESULT hr = RemoveMemoryLocation(pv);
		fSpyed = FX_SUCCEEDED(hr);
		cb = malloc32.pSpy->PreRealloc( pv, cb, &pRealMemory, fSpyed);

	    /* check if can release the spy */
	    if( malloc32.SpyReleasePending && !malloc32.SpyedAllocationsLeft ) {
			malloc32.SpyReleasePending = false;
			malloc32.pSpy->Release();
			malloc32.pSpy = 0;
	    }

	    if (0==cb) {
	        /* PreRealloc can force Realloc to fail */
			::pthread_mutex_unlock( &mutex );
			return NULL;
	    }
	    pv = pRealMemory;
	}

	void* pNewMemory = 0;
    if (!pv) 
		pNewMemory = malloc(cb);
	else if (cb) 
		pNewMemory = realloc(pv,cb);
	else {
		free( pv );
	    pNewMemory = NULL;
	}

	if( malloc32.pSpy ) {
	    pNewMemory = malloc32.pSpy->PostRealloc(pNewMemory, -1);
	    if (pNewMemory) 
			AddMemoryLocation(pNewMemory);
		::pthread_mutex_unlock( &mutex );
	}

	return pNewMemory;
}

/******************************************************************************
 * IMalloc32_Free [VTABLE]
 */
void firtex::com::internal::Malloc::Free(void* pv) 
{
	bool fSpyed = false;

	if ( malloc32.pSpy ) {
		::pthread_mutex_lock( &mutex );
  		FX_HRESULT hr = RemoveMemoryLocation(pv);
		fSpyed = FX_SUCCEEDED(hr);

	    pv = malloc32.pSpy->PreFree(pv, fSpyed);
	}

	free( pv );

	if( malloc32.pSpy ) {
		malloc32.pSpy->PostFree( fSpyed );

	    /* check if can release the spy */
	    if(malloc32.SpyReleasePending && !malloc32.SpyedAllocationsLeft) {
			malloc32.SpyReleasePending = false;
			malloc32.pSpy->Release();
			malloc32.pSpy = NULL;
	    }

		::pthread_mutex_unlock( &mutex );
	}
}

/******************************************************************************
 * IMalloc32_GetSize [VTABLE]
 *
 * NOTES
 *  FIXME returns:
 *      win95:  size allocated (4 byte boundarys)
 *      win2k:  size originally requested !!! (allocated on 8 byte boundarys)
 */
uint32_t firtex::com::internal::Malloc::GetSize(void* pv) 
{
	bool fSpyed = false;

	if(malloc32.pSpy) {
		::pthread_mutex_lock( &mutex );
	    pv = malloc32.pSpy->PreGetSize(pv, fSpyed);
	}

	// oh, oh, what do we do here
	//cb = HeapSize(GetProcessHeap(),0,pv);
	//?uint32_t cb = msize( pv );
	uint32_t cb = 0;

	if( malloc32.pSpy ) {
	    cb = malloc32.pSpy->PostGetSize(cb, fSpyed);
		::pthread_mutex_unlock( &mutex );
	}

	return cb;
}

/******************************************************************************
 * IMalloc32_DidAlloc [VTABLE]
 */
int16_t firtex::com::internal::Malloc::DidAlloc(void* pv) 
{
	bool fSpyed = false;

	if (malloc32.pSpy) {
		::pthread_mutex_lock( &mutex );
	    pv = malloc32.pSpy->PreDidAlloc(pv, fSpyed);
	}

	int16_t didAlloc = -1;

	if( malloc32.pSpy ) {
	    didAlloc = malloc32.pSpy->PostDidAlloc(pv, fSpyed, didAlloc);
		::pthread_mutex_unlock( &mutex );
	}
	return didAlloc;
}

/******************************************************************************
 * IMalloc32_HeapMinimize [VTABLE]
 */
void firtex::com::internal::Malloc::HeapMinimize() 
{

	if(malloc32.pSpy ) {
		::pthread_mutex_lock( &mutex );
	    malloc32.pSpy->PreHeapMinimize();
	}

	if(malloc32.pSpy) {
	    malloc32.pSpy->PostHeapMinimize();
		::pthread_mutex_unlock( &mutex );
	}
}

/******************************************************************************
 *	IMallocSpy implementation
 *****************************************************************************/
namespace firtex
{
	namespace com 
	{
		namespace internal 
		{
			class MallocSpy : public IMallocSpy 
			{
			public:
				MallocSpy();
				~MallocSpy();

				FX_STDMETHOD(QueryInterface)(FX_REFIID riid, void** ptr );
				FX_STDMETHOD_(uint32_t,AddRef)();
				FX_STDMETHOD_(uint32_t,Release)();
				FX_STDMETHOD_(uint32_t,PreAlloc)(uint32_t cbRequest);
				FX_STDMETHOD_(void*,PostAlloc)(void __RPC_FAR *pActual);
				FX_STDMETHOD_(void*,PreFree)(void __RPC_FAR *pRequest,bool_t fSpyed);
				FX_STDMETHOD_(void,PostFree)(bool_t fSpyed);
				FX_STDMETHOD_(uint32_t,PreRealloc)(void __RPC_FAR *pRequest,
					uint32_t cbRequest,void __RPC_FAR *__RPC_FAR *ppNewRequest,bool_t fSpyed);
				FX_STDMETHOD_(void*,PostRealloc)(void __RPC_FAR *pActual,bool_t fSpyed);
				FX_STDMETHOD_(void*,PreGetSize)(void __RPC_FAR *pRequest,bool_t fSpyed);
				FX_STDMETHOD_(uint32_t,PostGetSize)(uint32_t cbActual,bool_t fSpyed);
				FX_STDMETHOD_(void*,PreDidAlloc)(void __RPC_FAR *pRequest,bool_t fSpyed);
				FX_STDMETHOD_(int16_t,PostDidAlloc)(void __RPC_FAR *pRequest,bool_t fSpyed,int16_t fActual);
				FX_STDMETHOD_(void,PreHeapMinimize)( void);
				FX_STDMETHOD_(void,PostHeapMinimize)( void);

			private:
				uint32_t ref;	
			};
		}
	}
}



static firtex::com::internal::MallocSpy mallocspy32;

firtex::com::internal::MallocSpy::MallocSpy()
{
	ref = 0;
}

firtex::com::internal::MallocSpy::~MallocSpy()
{
	if ( ref != 0 )
		internal::log_printf( internal::WARN, "(%ld) - Reference count for internal MallocSpy not zero.\n", ref );
}


/******************************************************************************
 *	IMalloc32_QueryInterface	[VTABLE]
 */
firtex::com::FX_HRESULT firtex::com::internal::MallocSpy::QueryInterface(FX_REFIID refiid,void** obj)
{
	if ( FX_IsEqualIID(IUnknown::iid,refiid) ) {
		*obj = static_cast<IUnknown*>(this);
		AddRef();
		return FX_S_OK;
	}
	if ( FX_IsEqualIID(IMallocSpy::iid,refiid) ) {
		*obj = static_cast<IMallocSpy*>(this);
		AddRef();
		return FX_S_OK;
	}
	return FX_E_NOINTERFACE;
}

/******************************************************************************
 *	IMalloc32_AddRef		[VTABLE]
 */
uint32_t firtex::com::internal::MallocSpy::AddRef()
{
	pthread_mutex_lock lock(&mutex);
	return ++ref;
}

/******************************************************************************
 *	IMalloc32_AddRelease		[VTABLE]
 *
 * NOTES
 *   Our MallocSpy is static. If the count reaches 0 we dump the leaks
 */
uint32_t firtex::com::internal::MallocSpy::Release()
{
	pthread_mutex_lock lock(&mutex);
	return --ref;
}

uint32_t firtex::com::internal::MallocSpy::PreAlloc(uint32_t cbRequest)
{
    return cbRequest;
}

void* firtex::com::internal::MallocSpy::PostAlloc(void* pActual)
{
    return pActual;
}

void* firtex::com::internal::MallocSpy::PreFree(void* pRequest, firtex::com::bool_t fSpyed)
{
    return pRequest;
}
void firtex::com::internal::MallocSpy::PostFree(firtex::com::bool_t fSpyed)
{
	// do nothing
}

uint32_t firtex::com::internal::MallocSpy::PreRealloc(void* pRequest, uint32_t cbRequest, void** ppNewRequest, firtex::com::bool_t fSpyed)
{
    *ppNewRequest = pRequest;
    return cbRequest;
}

void* firtex::com::internal::MallocSpy::PostRealloc(void* pActual, firtex::com::bool_t fSpyed)
{
    return pActual;
}

void* firtex::com::internal::MallocSpy::PreGetSize(void* pRequest, firtex::com::bool_t fSpyed)
{
    return pRequest;
}

uint32_t firtex::com::internal::MallocSpy::PostGetSize(uint32_t cbActual, firtex::com::bool_t fSpyed)
{
    return cbActual;
}

void* firtex::com::internal::MallocSpy::PreDidAlloc(void* pRequest, firtex::com::bool_t fSpyed)
{
    return pRequest;
}

int16_t firtex::com::internal::MallocSpy::PostDidAlloc(void* pRequest, firtex::com::bool_t fSpyed, int16_t fActual)
{
    return fActual;
}

void firtex::com::internal::MallocSpy::PreHeapMinimize()
{
	// do nothing
}

void firtex::com::internal::MallocSpy::PostHeapMinimize()
{
	// do nothing
}

static void MallocSpyDumpLeaks() {
	firtex::com::internal::log_printf( firtex::com::internal::ERROR, "leaks: %lu\n", malloc32.SpyedAllocationsLeft);
}

/******************************************************************************
 *		FX_CoGetMalloc	[OLE32.@]
 *
 * RETURNS
 *	The win32 IMalloc
 */
firtex::com::FX_HRESULT firtex::com::FX_CoGetMalloc(uint32_t dwMemContext, firtex::com::IMalloc** lpMalloc)
{
	if ( !lpMalloc ) return FX_E_INVALIDARG;
        *lpMalloc = &malloc32;
        //? malloc32->AddRef();
        return FX_S_OK;
}

/***********************************************************************
 *           FX_CoTaskMemAlloc     [OLE32.@]
 * RETURNS
 * 	pointer to newly allocated block
 */
void* firtex::com::FX_CoTaskMemAlloc(uint32_t size)
{
	return malloc32.Alloc( size );
}
/***********************************************************************
 *           FX_CoTaskMemFree      [OLE32.@]
 */
void firtex::com::FX_CoTaskMemFree(void* ptr)
{
	malloc32.Free( ptr );
}

/***********************************************************************
 *           FX_CoTaskMemRealloc   [OLE32.@]
 * RETURNS
 * 	pointer to newly allocated block
 */
void* firtex::com::FX_CoTaskMemRealloc(void* pvOld, uint32_t size)
{
	return malloc32.Realloc( pvOld, size );
}

/***********************************************************************
 *           FX_CoRegisterMallocSpy        [OLE32.@]
 *
 * NOTES
 *  if a mallocspy is already registered, we can't do it again since
 *  only the spy knows, how to free a memory block
 */
firtex::com::FX_HRESULT firtex::com::FX_CoRegisterMallocSpy(firtex::com::IMallocSpy* pMallocSpy)
{

	/* HACK TO ACTIVATE OUT SPY */
	if ( pMallocSpy == reinterpret_cast<void*>(-1) ) {
		pMallocSpy = &mallocspy32;
	}

	if ( malloc32.pSpy) {
		return FX_CO_E_OBJISREG;
	}
	
	internal::pthread_mutex_lock lock(&mutex);

	IMallocSpy* pSpy = 0;
	FX_HRESULT hr = pMallocSpy->QueryInterface( IMallocSpy::iid, (void**)pSpy ); 
	if ( FX_FAILED(hr) ) return FX_E_INVALIDARG;
	assert( pSpy );
	
	malloc32.pSpy = pSpy;
	return FX_S_OK;
}

/***********************************************************************
 *           FX_CoRevokeMallocSpy  [OLE32.@]
 *
 * NOTES
 *  we can't revoke a malloc spy as long as memory blocks allocated with
 *  the spy are active since only the spy knows how to free them
 */
firtex::com::FX_HRESULT firtex::com::FX_CoRevokeMallocSpy(void)
{
 	internal::pthread_mutex_lock lock(&mutex);
 	
	/* if it's our spy it's time to dump the leaks */
	if ( malloc32.pSpy == &mallocspy32 ) {
	    MallocSpyDumpLeaks();
	}

	if ( malloc32.SpyedAllocationsLeft ) {
	    internal::log_printf( internal::INFO, "SpyReleasePending with %lu allocations left\n", malloc32.SpyedAllocationsLeft);
	    malloc32.SpyReleasePending = true;
	    return FX_E_ACCESSDENIED;

	}
	else if ( malloc32.pSpy ) {
		malloc32.pSpy->Release();
    		malloc32.pSpy = NULL;
        }

	return FX_S_OK;
}

/******************************************************************************
 *		IsValidInterface	[OLE32.@]
 *
 * RETURNS
 *  True, if the passed pointer is a valid interface
 */
/*BOOL WINAPI IsValidInterface(
	LPUNKNOWN punk	// [in] interface to be tested
) {
	return !(
		IsBadReadPtr(punk,4)					||
		IsBadReadPtr(punk->lpVtbl,4)				||
		IsBadReadPtr(punk->lpVtbl->QueryInterface,9)	||
		IsBadCodePtr((FARPROC)punk->lpVtbl->QueryInterface)
	);
}
*/
