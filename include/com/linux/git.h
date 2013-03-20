#ifndef _CPCOM_UNIX_GIT_H_
#define _CPCOM_UNIX_GIT_H_

#include "com/Com.h"

namespace firtex
{
	namespace com 
	{
		namespace git 
		{
			extern FX_CLSID clsid;

			FX_HRESULT Initialize();
			void    Uninitialize();
			FX_HRESULT QueryInterface( FX_REFIID iid, void** ppv );
		} // namespace git
	} // namespace com
}


#endif
