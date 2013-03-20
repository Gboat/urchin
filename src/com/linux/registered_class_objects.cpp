#include "com/Com.h"
#include "com/comerror.h"
#include <assert.h>
#include <pthread.h>
#include <stdlib.h>
#include "com/linux/internal.h"

/*
 * This linked list contains the list of registered class objects. These
 * are mostly used to register the factories for out-of-proc servers of OLE
 * objects.
 *
 * TODO: Make this data structure aware of inter-process communication. This
 *       means that parts of this will be exported to the Wine Server.
 */
struct RegisteredClass
{
  firtex::com::FX_CLSID     classIdentifier;
  firtex::com::IUnknown* classObject;
  uint32_t  runContext;
  uint32_t  connectFlags;
  uint32_t  dwCookie;
  void*     hThread; /* only for localserver */
  RegisteredClass* nextClass;
};

static RegisteredClass* first_class = NULL;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/***
 * COM_GetRegisteredClassObject
 *
 * This internal method is used to scan the registered class list to
 * find a class object.
 *
 * Params:
 *   rclsid        Class ID of the class to find.
 *   dwClsContext  Class context to match.
 *   ppv           [out] returns a pointer to the class object. Complying
 *                 to normal COM usage, this method will increase the
 *                 reference count on this object.
 */
firtex::com::FX_HRESULT firtex::com::internal::GetRegisteredClassObject(
	FX_REFCLSID    rclsid,
	uint32_t       dwClsContext,
	IUnknown**  ppUnk)
{
  pthread_mutex_lock lock( &mutex );

  /*
   * Sanity check
   */
  assert( ppUnk );

  /*
   * Iterate through the whole list and try to match the class ID.
   */
  RegisteredClass* curClass = first_class;

  while (curClass != 0)
  {
    /*
     * Check if we have a match on the class ID.
     */
    if (FX_IsEqualGUID( curClass->classIdentifier, rclsid))
    {
      /*
       * Since we don't do out-of process or DCOM just right away, let's ignore the
       * class context.
       */

      /*
       * We have a match, return the pointer to the class object.
       */
      *ppUnk = curClass->classObject;
      (*ppUnk)->AddRef();
      return FX_S_OK;
    }

    /*
     * Step to the next class in the list.
     */
    curClass = curClass->nextClass;
  }

  /*
   * If we get to here, we haven't found our class.
   */
  return FX_S_FALSE;
}
