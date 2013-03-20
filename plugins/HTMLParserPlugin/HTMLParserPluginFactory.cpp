#include "com/comerror.h"
#include "com/ComPtr.h"
#include "HTMLParserPlugin.h"

namespace firtex
{
	namespace plugin
	{
		FX_STDMETHODIMP_(uint32_t) CHTMLParserFactory::AddRef()
		{ 
			if ( !m_count ) 
				firtex::com::DllLockServerImpl(true); 
			return ++m_count; 
		};

		FX_STDMETHODIMP_(uint32_t) CHTMLParserFactory::Release()
		{ 
			unsigned long c = --m_count; 
			if ( !c ) 
			{
				firtex::com::DllLockServerImpl(false); 
				// only used as a static object
				// don't delete the object
				// not on heap

				/* delete this; */
			}
			return c; 
		};

		FX_STDMETHODIMP CHTMLParserFactory::QueryInterface( firtex::com::FX_IID const& riid, void** ptr )
		{
			if ( firtex::com::FX_IsEqualIID( riid, firtex::com::IUnknown::iid ) )
			{
				*ptr = static_cast<firtex::com::IUnknown*>( this );
				AddRef();
				return FX_S_OK;
			}
			if ( firtex::com::FX_IsEqualIID( riid, firtex::com::IClassFactory::iid ) )
			{
				*ptr = static_cast<firtex::com::IClassFactory*>( this );
				AddRef();
				return FX_S_OK;
			}

			return FX_E_NOINTERFACE;
		}

		FX_STDMETHODIMP CHTMLParserFactory::LockServer(int32_t fLock)
		{
			return firtex::com::DllLockServerImpl( fLock );
		}

		FX_STDMETHODIMP CHTMLParserFactory::CreateInstance( firtex::com::IUnknown* pUnkOuter, firtex::com::FX_REFIID riid, void** ppvObject )
		{
			if ( pUnkOuter ) 
				return FX_CLASS_E_NOAGGREGATION;

			CHTMLParserPlugin* ptr = 0;
			ptr = new CHTMLParserPlugin();
			if ( !ptr ) 
			{
				return FX_E_OUTOFMEMORY;
			}

			firtex::com::FX_HRESULT hr = ptr->QueryInterface( riid, ppvObject );
			if ( FX_FAILED(hr) ) 
			{
				delete ptr;
			}
			return hr;
		}

	}
}
