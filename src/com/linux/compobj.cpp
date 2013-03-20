#include "com/Com.h"
#include "com/comerror.h"
#include <assert.h>
#include <pthread.h>
#include <stdlib.h>
#include "com/linux/git.h"
#include "com/linux/database.h"
#include "com/linux/internal.h"

static int32_t volatile lock_count = 0;
static pthread_mutex_t lock_count_mutex = PTHREAD_MUTEX_INITIALIZER;
static firtex::com::internal::apartment_t mta;
static firtex::com::internal::apartment_t* apartments = 0;
static pthread_mutex_t apartments_mutex = PTHREAD_MUTEX_INITIALIZER;

/* The multi-threaded apartment (MTA) contains zero or more threads interacting
with free threaded (ie thread safe) COM objects. There is only ever one MTA
in a process - you can enter it by calling FX_CoInitializeEx(COINIT_MULTITHREADED)
*/
static void COM_InitMTA(void)
{
	/* OXIDs are object exporter IDs. Each apartment has an OXID, which is unique
	within a network. That is, two different MTAs on different machines will have
	different OXIDs.

	This method of generating an OXID is therefore wrong as it doesn't work across
	a network, but for local RPC only it's OK. We can distinguish between MTAs and
	STAs because STAs use the thread ID as well, and no thread can have an ID of zero.
	*/
	//mta.oxid = ((OXID)GetCurrentProcessId() << 32);
	mta.next = 0;
	memset( &mta.tid, 0, sizeof(mta.tid) );
	pthread_mutex_init( &mta.mutex, NULL);
}

static void COM_UninitMTA(void)
{
	pthread_mutex_destroy( &mta.mutex );
	// MTA.oxid = 0;
}

/* creates an apartment structure which stores OLE thread-local
 * information. Call with COINIT_UNINITIALIZED to create an apartment
 * that will be initialized with a model later. Note: do not call
 * with COINIT_UNINITIALIZED if the apartment has already been initialized
 * with a different COINIT value 
 */
firtex::com::internal::apartment_t* firtex::com::internal::CreateApartment(uint32_t model)
{
	apartment_t* apt = 0;
	bool create = (GetThreadSpecificData()->apartment == NULL);

	if (create) 
	{
		apt = (apartment_t*)malloc( sizeof(apartment_t) );
		if ( !apt ) return 0;
		memset( apt, 0, sizeof(*apt) );
		apt->tid = pthread_self();
		pthread_mutex_init( &apt->mutex, 0 );
		// TODO:  do we need to check the above return?
	}
	else
	{
		apt = GetThreadSpecificData()->apartment;
	}

	apt->model = model;
	if ( model & firtex::com::COINIT_APARTMENTTHREADED ) 
	{
		/* FIXME: how does windoze create OXIDs? */
		//apt->oxid = MTA.oxid | GetCurrentThreadId();

		//apt->win = CreateWindowA(aptWinClass, NULL, 0,
		//	       0, 0, 0, 0,
		//	       0, 0, OLE32_hInstance, NULL);

		pthread_mutex_init( &apt->mutex, NULL);
	}
	else if (!(model & COINIT_UNINITIALIZED)) 
	{
		apt->parent = &mta;
		//apt->oxid = MTA.oxid;
	}

	if (create)
	{
		internal::pthread_mutex_lock lock( &apartments_mutex );
		if (apartments) apartments->prev = apt;
		apt->next = apartments;
		apartments = apt;
	}
	GetThreadSpecificData()->apartment = apt;
	return apt;
}

void firtex::com::internal::DestroyApartment(firtex::com::internal::apartment_t* apt)
{
	// pull apartment from the list of linked apartments
	::pthread_mutex_lock( &apartments_mutex );
	if (apt->prev) apt->prev->next = apt->next;
	if (apt->next) apt->next->prev = apt->prev;
	if (apartments == apt) apartments = apt->next;
	apt->prev = NULL; apt->next = NULL;
	::pthread_mutex_unlock( &apartments_mutex );

	// clean up the apartment's resources
	if (apt->model & COINIT_APARTMENTTHREADED) 
	{
		//if (apt->win) DestroyWindow(apt->win);

		pthread_mutex_destroy( & apt->mutex );
	}

	// free the memory
	if ( apt->errorinfo ) apt->errorinfo->Release();
	free( apt );
}


/******************************************************************************
*		FX_CoInitialize	[OLE32.@]
*
* Initializes the COM libraries.
*
* See FX_CoInitializeEx
*/
firtex::com::FX_HRESULT FX_STDAPICALLTYPE firtex::com::FX_CoInitialize(void* lpReserved	/* [in] pointer to win32 malloc interface(obsolete, should be NULL) */)
{
	/*
	* Just delegate to the newer method.
	*/
	return FX_CoInitializeEx( lpReserved, COINIT_APARTMENTTHREADED );
}

/******************************************************************************
*		FX_CoInitializeEx	[OLE32.@]
*
* Initializes the COM libraries. The behavior used to set the win32 IMalloc
* used for memory management is obsolete.
*
* RETURNS
*  FX_S_OK               if successful,
*  FX_S_FALSE            if this function was called already.
*  FX_RPC_E_CHANGED_MODE if a previous call to FX_CoInitializeEx specified another
*                     threading model.
*/
firtex::com::FX_HRESULT FX_STDAPICALLTYPE firtex::com::FX_CoInitializeEx(void* lpReserved,	/* [in] pointer to win32 malloc interface(obsolete, should be NULL) */
															  uint32_t dwCoInit	/* [in] A value from COINIT specifies the threading model */)
{
	FX_HRESULT hr = FX_S_OK;

	// check the parameters
	if ( lpReserved!=NULL )
	{
		internal::log_printf( internal::ERROR, "(%p, %x) - Bad parameter passed-in %p, must be an old Windows Application\n", lpReserved, (int)dwCoInit, lpReserved);
	}

	/*
	* Check the lock count. If this is the first time going through the initialize
	* process, we have to initialize the libraries.
	*
	* And crank-up that lock count.
	*/
	pthread_mutex_lock( &lock_count_mutex );
	if ( lock_count == 0 ) 
	{
		/*
		 * Initialize the various COM libraries and data structures.
		 */
		internal::log_printf( internal::INFO, "() - Initializing the COM libraries\n");

		FX_HRESULT hr2 = firtex::com::database::Initialize();
		if ( FX_FAILED(hr2) )  hr = hr2;
		hr2 = git::Initialize();
		if ( FX_FAILED(hr2) ) hr = hr2;

		COM_InitMTA();
		// RunningObjectTableImpl_Initialize();
	}
	++lock_count;
	pthread_mutex_unlock( &lock_count_mutex );

	// Note, this could be safer with a number of win32 interlocked primitives,
	// but they are not protable.

	// First call to CoInitailize should be with a single thread
	// New threads should not be called until after first thread is
	// initialized, so we take some shortcuts

	internal::apartment_t *apt = internal::GetThreadSpecificData()->apartment;
	if ( apt && !( apt->model == COINIT_UNINITIALIZED) )
	{
		if (dwCoInit != apt->model)
		{
			/* Changing the threading model after it's been set is illegal. If this warning is triggered by Wine
			code then we are probably using the wrong threading model to implement that API. */
			internal::log_printf( internal::WARN, "Attempt to change threading model of this apartment from 0x%lx to 0x%lx\n", apt->model, dwCoInit);
			return FX_RPC_E_CHANGED_MODE;
		}
		hr = FX_S_FALSE;
	}
	else
		hr = FX_S_OK;

	if ( !apt || apt->model == COINIT_UNINITIALIZED ) 
	{
		apt = internal::CreateApartment(dwCoInit);
	}

	// InterlockedIncrement( &apt->inits );
	pthread_mutex_lock( &apt->mutex );
	++apt->lock_count;
	pthread_mutex_unlock( &apt->mutex );

	if (hr == FX_S_OK) 
	{
		internal::GetThreadSpecificData()->apartment = apt;
		assert( internal::GetThreadSpecificData()->apartment );
	}

	return hr;
}

/***********************************************************************
*           CoUninitialize   [OLE32.@]
*
* This method will release the COM libraries.
*
* See the windows documentation for more details.
*/
void FX_STDAPICALLTYPE firtex::com::FX_CoUninitialize(void)
{
	internal::apartment_t* apt = 0;

	apt = internal::GetThreadSpecificData()->apartment;
	if (!apt) return;

	pthread_mutex_lock( &apt->mutex );
	--apt->lock_count;
	if ( apt->lock_count==0 ) 
	{
		internal::GetThreadSpecificData()->apartment = NULL;
		internal::DestroyApartment(apt);
		apt = NULL;
	}
	pthread_mutex_unlock( &apt->mutex );

	/*
	* Decrease the reference count.
	* If we are back to 0 locks on the COM library, make sure we free
	* all the associated data structures.
	*/
	pthread_mutex_lock( &lock_count_mutex );
	--lock_count;

	if (lock_count==0)
	{
		internal::log_printf( internal::INFO, "() - Releasing the COM libraries\n");

		//RunningObjectTableImpl_UnInitialize();

		/* disconnect proxies to release the corresponding stubs.
		* FIXME: native version might not do this and we might just be working
		* around bugs elsewhere. */
		//MARSHAL_Disconnect_Proxies();

		/* Release the references to the registered class objects */
		//COM_RevokeAllClasses();

		/* This will free the loaded COM Dlls  */
		FX_CoFreeAllLibraries();

		/* This will free list of external references to COM objects */
		//COM_ExternalLockFreeList();

		/* This ensures we deal with any pending RPCs */
		//COM_FlushMessageQueue();

		git::Uninitialize();		

		COM_UninitMTA();
	}

	// sanity check on lock count
	if (lock_count<0) {
		internal::log_printf( internal::ERROR, "CoUninitialize() - not CoInitialized.\n" );
	}

	pthread_mutex_unlock( &lock_count_mutex );
}
