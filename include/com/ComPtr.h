#ifndef _COMPTR_H_
#define _COMPTR_H_

#include "Com.h"
#include "comerror.h"
#include <assert.h>
#include <stdexcept>

namespace firtex
{
	namespace com 
	{

		/////////////////////////////////////////////////////////////////////////////
		// CComPtr

		template <class T>
		class CComPtr
		{
		public:
			CComPtr()
			{
				ptr = 0; 
			};

			CComPtr( T* rhs )
			{
				ptr = rhs; if (ptr) ptr->AddRef(); 
			};
			CComPtr( CComPtr<T> const& rhs )
			{
				ptr = rhs.ptr; if (ptr) ptr->AddRef(); 
			};
			~CComPtr()
			{
				if ( ptr )
					ptr->Release(); 
			};

			operator T*() const 
			{
				return ptr; 
			};
			T** operator&() 
			{
				return &ptr; 
			};
			T* operator->()
			{
				return ptr; 
			};
			CComPtr<T>&	operator=( T* rhs )
			{
				if ( ptr ) 
					ptr->Release();
				ptr=rhs; 
				if (ptr)
					ptr->AddRef();
				return *this; 
			};
			CComPtr<T>&	operator=( CComPtr<T> const& rhs )
			{ 
				if ( ptr )
					ptr->Release(); 
				ptr=rhs.ptr;
				if (ptr) 
					ptr->AddRef();
				return *this; 
			};

			void attach( T* rhs )
			{
				if ( ptr ) 
					ptr->Relese();
				ptr = rhs; 
			}; 
			T*	detach() 
			{ 
				T* ret = ptr; 
				ptr = 0;
				return ret; 
			};
			T*	get()
			{
				return ptr; 
			};

		private:
			T*	ptr;
		};

		/////////////////////////////////////////////////////////////////////////////
		// CComStr

		class CComStr
		{
		public:
			CComStr()
			{
				m_str = NULL;
			}
			/*explicit*/ CComStr( int nSize )
			{
				m_str = FX_SysAllocStringLen(NULL, nSize);
			}
			/*explicit*/ CComStr(int nSize, wchar_t const* sz)
			{
				m_str = FX_SysAllocStringLen(sz, nSize);
			}
			/*explicit*/ CComStr(wchar_t const* pSrc)
			{
				m_str = FX_SysAllocString(pSrc);
			}
			/*explicit*/ CComStr(const CComStr& src)
			{
				m_str = src.copy();
			}
			/*explicit*/ CComStr( FX_REFGUID src )
			{
				wchar_t guid[64];
				FX_StringFromGUID2( src, guid, sizeof(guid)/sizeof(*guid) );
				m_str = FX_SysAllocString(guid);
			}

			CComStr& operator=(const CComStr& src)
			{
				if (m_str != src.m_str) {
					FX_BSTR tmp = src.copy();
					if ( !tmp ) throw std::bad_alloc();
					FX_SysFreeString(m_str);
					m_str = tmp;
				}
				return *this;
			}
			CComStr& operator=(wchar_t const* pSrc)
			{
				FX_BSTR tmp = FX_SysAllocString(pSrc);
				if ( !tmp ) throw std::bad_alloc();
				FX_SysFreeString(m_str);
				m_str = tmp;
				return *this;
			}
			~CComStr()
			{
				FX_SysFreeString(m_str);
			}

			uint16_t getLength() const
			{
				return (m_str == NULL)? 0 : FX_SysStringLen(m_str);
			}
			operator FX_BSTR() const
			{
				return m_str;
			}
			FX_BSTR* operator&()
			{
				return &m_str;
			}
			FX_BSTR copy() const
			{
				return FX_SysAllocStringLen(m_str, FX_SysStringLen(m_str));
			}
			FX_HRESULT copyTo(FX_BSTR* pbstr)
			{
				assert(pbstr != NULL);
				if (pbstr == NULL) return FX_E_POINTER;
				*pbstr = FX_SysAllocStringLen(m_str, FX_SysStringLen(m_str));
				if (*pbstr == NULL) return FX_E_OUTOFMEMORY;
				return FX_S_OK;
			}
			void attach(FX_BSTR src)
			{
				assert(m_str == NULL);
				m_str = src;
			}
			FX_BSTR detach()
			{
				FX_BSTR s = m_str;
				m_str = NULL;
				return s;
			}
			FX_BSTR get()
			{
				return m_str;
			}
			void empty()
			{
				FX_SysFreeString(m_str);
				m_str = NULL;
			}
			bool operator!() const
			{
				return (m_str == NULL);
			}
			FX_HRESULT append(const CComStr& bstrSrc)
			{
				return append(bstrSrc.m_str, FX_SysStringLen(bstrSrc.m_str));
			}
			FX_HRESULT append(wchar_t const* lpsz)
			{
				return append(lpsz, (int)wcslen(lpsz));
			}
			// a FX_BSTR is just a LPCOLESTR so we need a special version to signify
			// that we are appending a FX_BSTR
			FX_HRESULT appendBSTR(FX_BSTR p)
			{
				return append(p, FX_SysStringLen(p));
			}
			FX_HRESULT append(wchar_t const* lpsz, int nLen)
			{
				int n1 = getLength();
				FX_BSTR b;
				b = FX_SysAllocStringLen(NULL, n1+nLen);
				if (b == NULL)
					return FX_E_OUTOFMEMORY;
				memcpy(const_cast<wchar_t*>(b), m_str, n1*sizeof(wchar_t));
				memcpy(const_cast<wchar_t*>(b+n1), lpsz, nLen*sizeof(wchar_t));
				assert( b[n1+nLen] == L'\0' );
				FX_SysFreeString(m_str);
				m_str = b;
				return FX_S_OK;
			}

			CComStr& operator+=(const CComStr& bstrSrc)
			{
				appendBSTR(bstrSrc.m_str);
				return *this;
			}
			bool operator<(FX_BSTR bstrSrc) const
			{
				if (bstrSrc == NULL && m_str == NULL)
					return false;
				if (bstrSrc != NULL && m_str != NULL)
					return wcscmp(m_str, bstrSrc) < 0;
				return m_str == NULL;
			}
			bool operator==(FX_BSTR bstrSrc) const
			{
				if (bstrSrc == NULL && m_str == NULL)
					return true;
				if (bstrSrc != NULL && m_str != NULL)
					return wcscmp(m_str, bstrSrc) == 0;
				return false;
			}
			bool operator<(char const* pszSrc) const
			{
				if (pszSrc == NULL && m_str == NULL)
					return false;
				CComStr tmp( pszSrc );
				if (tmp.m_str != NULL && m_str != NULL)
					return wcscmp(m_str, tmp.m_str) < 0;
				return m_str == NULL;
			}
			bool operator==(char const* pszSrc) const
			{
				if (pszSrc == NULL && m_str == NULL)
					return true;
				CComStr tmp( pszSrc );
				if ( !tmp ) throw std::bad_alloc();
				if (tmp.m_str != NULL && m_str != NULL)
					return wcscmp(m_str, tmp.m_str) == 0;
				return false;
			}

			/* explicit */ CComStr(char const* pSrc)
			{
				m_str = FX_SysAllocStringA( pSrc );
			}

			FX_HRESULT append( char const* pSrc)
			{
				CComStr tmp( pSrc );
				if ( !tmp ) return FX_E_OUTOFMEMORY;
				return append( tmp );
			}

			CComStr& operator=(char const* pSrc)
			{
				CComStr tmp( pSrc );
				if ( !tmp ) throw std::bad_alloc();
				FX_SysFreeString(m_str);
				m_str = tmp.detach();
				return *this;
			}
		private:
			FX_BSTR m_str;
		}; // class CComStr
	}// namespace com
}; // namespace firtex
#endif // _COMPTR_H_
