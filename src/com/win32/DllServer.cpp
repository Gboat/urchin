#include "com/Com.h"
#include "com/comerror.h"
#include <assert.h>

//static uint_least32_t lock_count = 0;
static uint32_t lock_count = 0;


FX_WINOLEAPI firtex::com::DllCanUnloadNowImpl()
{
	return ( lock_count > 0 ) ? FX_S_FALSE : FX_S_OK;
}

FX_WINOLEAPI firtex::com::DllLockServerImpl( int32_t fLock )
{
	// need to add thread safety

	if ( fLock ) {
		++lock_count;
		assert( lock_count != 0 );
	}
	else {
		assert( lock_count != 0 );
		--lock_count;
	}

	return FX_S_OK;
}
