//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: ¹ùÈð½Ü(GuoRuijie)
// Email	: ruijieguo@software.ict.ac.cn,ruijieguo@gmail.com
// Created	: 2006/5/15
//
#include "plugin/ComObject.h"
#include "com/comerror.h"
#include <assert.h>

namespace firtex
{
	namespace plugin
	{
		CComObject::CComObject(void)
			:m_refCount(0)
		{
		}

		CComObject::~CComObject(void)
		{
		}

		FX_STDMETHODIMP_(uint32_t) CComObject::AddRef()
		{ 
			if ( !m_refCount ) 
				firtex::com::DllLockServerImpl(true); 
			return ++m_refCount; 
		};

		FX_STDMETHODIMP_(uint32_t) CComObject::Release()
		{ 
			unsigned long c = --m_refCount; 
			if ( !c ) 
			{
				firtex::com::DllLockServerImpl(false); 
				delete this;
			}
			return c; 
		};	

		/////////////////////////////////////////////////////////////////////////////
		// FX_GUID tools

		tstring GUIDX::encode(const void * pInGUID)
		{
			const firtex::com::FX_GUID* pGUID = reinterpret_cast<const firtex::com::FX_GUID*>(pInGUID);
			tchar tmp[512];
			sprintf(tmp,_T("{%.8X-%.4X-%.4X-%.2X%.2X-%.2X%.2X%.2X%.2X%.2X%.2X}"),
				pGUID->Data1, pGUID->Data2, pGUID->Data3,
				pGUID->Data4[0], pGUID->Data4[1], pGUID->Data4[2], pGUID->Data4[3],
				pGUID->Data4[4], pGUID->Data4[5], pGUID->Data4[6], pGUID->Data4[7] );

			tstring str = tmp;
		
			return str;
		}

		bool GUIDX::decode(const tchar* pszIn, void* pOutGUID)
		{
			assert( pOutGUID != NULL );

			if ( pszIn == NULL ) return false;

			if ( _tcslen(pszIn) == 38 )
			{
				if ( pszIn[0] != '{' || pszIn[37] != '}' ) return false;
			}
			else if ( _tcslen(pszIn) == 36 )
			{
				pszIn --;
			}
			else return false;

			byte* pGUID = reinterpret_cast<byte*>(pOutGUID);

			if ( ! unhex( pszIn + 1, pGUID + 3 ) ) return false;
			if ( ! unhex( pszIn + 3, pGUID + 2 ) ) return false;
			if ( ! unhex( pszIn + 5, pGUID + 1 ) ) return false;
			if ( ! unhex( pszIn + 7, pGUID + 0 ) ) return false;
			if ( ! unhex( pszIn + 10, pGUID + 5 ) ) return false;
			if ( ! unhex( pszIn + 12, pGUID + 4 ) ) return false;
			if ( ! unhex( pszIn + 15, pGUID + 7 ) ) return false;
			if ( ! unhex( pszIn + 17, pGUID + 6 ) ) return false;
			if ( ! unhex( pszIn + 20, pGUID + 8 ) ) return false;
			if ( ! unhex( pszIn + 22, pGUID + 9 ) ) return false;
			if ( ! unhex( pszIn + 25, pGUID + 10 ) ) return false;
			if ( ! unhex( pszIn + 27, pGUID + 11 ) ) return false;
			if ( ! unhex( pszIn + 29, pGUID + 12 ) ) return false;
			if ( ! unhex( pszIn + 31, pGUID + 13 ) ) return false;
			if ( ! unhex( pszIn + 33, pGUID + 14 ) ) return false;
			if ( ! unhex( pszIn + 35, pGUID + 15 ) ) return false;

			return true;
		}

		bool GUIDX::unhex(const tchar* psz, byte* pOut)
		{
			register tchar c = *psz++;
			if ( c >= '0' && c <= '9' )
				*pOut = ( c - '0' ) << 4;
			else if ( c >= 'A' && c <= 'F' )
				*pOut = ( c - 'A' + 10 ) << 4;
			else if ( c >= 'a' && c <= 'f' )
				*pOut = ( c - 'a' + 10 ) << 4;
			else
				return false;
			c = *psz;
			if ( c >= '0' && c <= '9' )
				*pOut |= ( c - '0' );
			else if ( c >= 'A' && c <= 'F' )
				*pOut |= ( c - 'A' + 10 );
			else if ( c >= 'a' && c <= 'f' )
				*pOut |= ( c - 'a' + 10 );
			else
				return false;
			return true;
		}
	}
}
