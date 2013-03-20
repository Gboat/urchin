#include "com/Com.h"
#include "com/comerror.h"
#include <assert.h>
#include <pthread.h>
#include <stdlib.h>
#include "com/linux/internal.h"
#include "com/linux/git.h"
#include "com/linux/database.h"


/***********************************************************************
*           FX_CoCreateInstance [COMPOBJ.13]
*           FX_CoCreateInstance [OLE32.@]
*/
firtex::com::FX_HRESULT FX_STDAPICALLTYPE firtex::com::FX_CoCreateInstance(
	firtex::com::FX_REFCLSID rclsid,
	firtex::com::IUnknown* pUnkOuter,
	uint32_t dwClsContext,
	firtex::com::FX_REFIID iid,
	void** ppv)
{
	IClassFactory* lpclf = 0;

	// check parameters
	if (ppv==0) return FX_E_POINTER;

	// Initialize the "out" parameter
	*ppv = 0;

	/*
	* The Standard Global Interface Table (GIT) object is a process-wide singleton.
	* Rather than create a class factory, we can just check for it here
	*/
	if (FX_IsEqualCLSID(rclsid, git::clsid)) 
	{
		return git::QueryInterface( iid, ppv );
	}

	// Get a class factory to construct the object we want.
	FX_HRESULT hr = FX_CoGetClassObject(rclsid, dwClsContext, NULL,
		IClassFactory::iid, (void**)&lpclf);
	if ( FX_FAILED(hr) ) {
		//FIXME("no classfactory created for FX_CLSID %s, hres is 0x%08lx\n",
		//debugstr_guid(rclsid),hres);
		return hr;
	}

	/*
	* Create the object and don't forget to release the factory
	*/
	hr = lpclf->CreateInstance( pUnkOuter, iid, ppv );
	lpclf->Release();
	return hr;
}

/***********************************************************************
*           FX_CoCreateInstanceEx [OLE32.@]
*/
firtex::com::FX_HRESULT FX_STDAPICALLTYPE firtex::com::FX_CoCreateInstanceEx(
	FX_REFCLSID      rclsid,
	firtex::com::IUnknown*     pUnkOuter,
	uint32_t dwClsContext,
	firtex::com::COSERVERINFO* pServerInfo,
	uint32_t         cmq,
	MULTI_QI*     pResults)
{
	IUnknown* pUnk = NULL;
	FX_HRESULT		hr;
	uint32_t		index;
	uint32_t     successCount = 0;

	// check arguments
	if ( cmq==0 ) return FX_E_INVALIDARG;
	if ( pResults==NULL ) return FX_E_INVALIDARG;

	if (pServerInfo!=NULL) {
		internal::log_printf( internal::WARN, "() non-NULL pServerInfo not supported!\n" );
	}

	// Initialize all the "out" parameters.
	for ( index = 0; index < cmq; index++) {
		pResults[index].pItf = NULL;
		pResults[index].hr   = FX_E_NOINTERFACE;
	}

	// Get the object and get its IUnknown pointer.
	hr = FX_CoCreateInstance(rclsid, pUnkOuter, dwClsContext,
		IUnknown::iid, (void**)&pUnk);
	if (hr) return hr;

	/*
	* Then, query for all the interfaces requested.
	*/
	for (index = 0; index < cmq; index++)
	{
		pResults[index].hr = pUnk->QueryInterface(
			*pResults[index].pIID,
			(void**)&(pResults[index].pItf));

		if (pResults[index].hr == FX_S_OK)
			successCount++;
	}

	/*
	* Release our temporary unknown pointer.
	*/
	pUnk->Release();

	if (successCount == 0)
		return FX_E_NOINTERFACE;

	if (successCount!=cmq)
		return FX_CO_S_NOTALLINTERFACES;

	return FX_S_OK;
}

/***********************************************************************
*           FX_CoGetClassObject [COMPOBJ.7]
*           FX_CoGetClassObject [OLE32.@]
*
* FIXME.  If request allows of several options and there is a failure
*         with one (other than not being registered) do we try the
*         others or return failure?  (E.g. inprocess is registered but
*         the DLL is not found but the server version works)
*/
firtex::com::FX_HRESULT FX_STDAPICALLTYPE firtex::com::FX_CoGetClassObject(
	firtex::com::FX_REFCLSID rclsid, uint32_t dwClsContext, 
	firtex::com::COSERVERINFO *pServerInfo,
	firtex::com::FX_REFIID iid, void** ppv
	)
{
	IUnknown* regClassObject = 0;

	//WINE_StringFromCLSID((LPCLSID)rclsid,xclsid);
	//TRACE("\n\tCLSID:\t%s,\n\tIID:\t%s\n", debugstr_guid(rclsid), debugstr_guid(iid));

	if (pServerInfo) {
		internal::log_printf( internal::WARN, "() non-NULL pServerInfo not supported!\n" );
		//FIXME("\tpServerInfo: name=%s\n",debugstr_w(pServerInfo->pwszName));
		//FIXME("\t\tpAuthInfo=%p\n",pServerInfo->pAuthInfo);
	}

	/*
	* First, try and see if we can't match the class ID with one of the
	* registered classes.
	*/
	if (FX_S_OK == internal::GetRegisteredClassObject(rclsid, dwClsContext, &regClassObject))
	{
		// Get the required interface from the retrieved pointer.
		FX_HRESULT hr = regClassObject->QueryInterface( iid, ppv );

		/*
		* Since QI got another reference on the pointer, we want to release the
		* one we already have. If QI was unsuccessful, this will release the object. This
		* is good since we are not returning it in the "out" parameter.
		*/
		regClassObject->Release();

		// and we are done
		return hr;
	}

	// first try: in-process 
	if ((FX_CLSCTX_INPROC_SERVER | FX_CLSCTX_INPROC_HANDLER) & dwClsContext) {
		FX_HRESULT hr = internal::GetInprocClassObject( rclsid, &regClassObject);
		if ( FX_FAILED(hr) ) return hr;

		// Get the required interface from the retrieved pointer.
		hr = regClassObject->QueryInterface( iid, ppv );

		/*
		* Since QI got another reference on the pointer, we want to release the
		* one we already have. If QI was unsuccessful, this will release the object. This
		* is good since we are not returning it in the "out" parameter.
		*/
		regClassObject->Release();

		// and we are done
		return hr;
	}

	// Next try out of process 
	internal::log_printf( internal::NONE, "\tChecking out local server\n" );
	if (FX_CLSCTX_LOCAL_SERVER & dwClsContext)
	{
		// return create_marshalled_proxy(rclsid,iid,ppv);
		internal::log_printf( internal::ERROR, "FX_CLSCTX_LOCAL_SERVER not supported." );
		return FX_E_UNEXPECTED;
	}

	/* finally try remote: this requires networked DCOM (a lot of work) */
	internal::log_printf( internal::NONE, "\tChecking out remote server\n" );
	if (FX_CLSCTX_REMOTE_SERVER & dwClsContext)
	{
		internal::log_printf( internal::ERROR, "FX_CLSCTX_REMOTE_SERVER not supported." );
		return FX_E_NOINTERFACE;
	}

	return FX_E_UNEXPECTED;
}

