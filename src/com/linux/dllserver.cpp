#include "com/Com.h"
#include "com/comerror.h"
#include <assert.h>
#include <pthread.h>

static int32_t volatile lock_count = 0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

FX_WINOLEAPI firtex::com::DllCanUnloadNowImpl()
{
	pthread_mutex_lock( &mutex );
	assert( lock_count >= 0 );
	int32_t tmp = lock_count;
	pthread_mutex_unlock( &mutex );

	return ( tmp > 0 ) ? FX_S_FALSE : FX_S_OK;
}

FX_WINOLEAPI firtex::com::DllLockServerImpl( int32_t fLock )
{
	pthread_mutex_lock( &mutex );
	assert( fLock || lock_count>0 );
	lock_count += fLock ? 1 : -1;
	assert( !fLock || lock_count>0 );
	pthread_mutex_unlock( &mutex );

	return FX_S_OK;
}
