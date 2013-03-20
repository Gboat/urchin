//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: �����(GuoRuijie)
// Email	: ruijieguo@software.ict.ac.cn,ruijieguo@gmail.com
// Created	: 2006/5/15
//
#ifndef __COMOBJECT_H
#define __COMOBJECT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../utility/StdHeader.h"
#include <stddef.h>
#include "../com/Com.h"

namespace firtex
{
	namespace plugin
	{
		class CComObject
		{
		public:
			CComObject(void);
			virtual ~CComObject(void);
		public:
			//FX_STDMETHOD(QueryInterface)( firtex::com::FX_IID const& riid, void** ptr );
			FX_STDMETHOD_(uint32_t,AddRef)();
			FX_STDMETHOD_(uint32_t,Release)();

		private:
			uint32_t	m_refCount;
		};

	#define FX_METHOD_PROLOGUE(theClass, localClass) \
		theClass* pThis = \
		((theClass*)((byte*)this - offsetof(theClass, m_x##localClass))); \
		pThis; // avoid warning from compiler \

	#define INTERFACE_TO_CLASS(icClass, icInterface, icIn, icOut)	\
		icClass * icOut = (icClass *)( (byte*) icIn - offsetof( icClass, m_x##icInterface ) );
	
	#define CLASS_TO_INTERFACE(icClass, icInterface, icIn, icOut)	\
		X##icInterface* icOut = (X##icInterface*)( (byte*) icIn + offsetof( icClass, m_x##icInterface ) );

	#define BEGIN_INTERFACE_PART(localClass, baseClass) \
		class X##localClass : public baseClass \
		{ \
		public: \
			FX_STDMETHOD(QueryInterface)(firtex::com::FX_REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject); \
			FX_STDMETHOD_(uint32_t, AddRef)(); \
			FX_STDMETHOD_(uint32_t, Release)(); \
			
	#define END_INTERFACE_PART(localClass) \
		} m_x##localClass; \
		friend class X##localClass;\

	#define IMPLEMENT_UNKNOWN(theClass, localClass)					\
		FX_STDMETHODIMP_(uint32_t) theClass::X##localClass::AddRef()	\
		{															\
			FX_METHOD_PROLOGUE(theClass, localClass)					\
			return pThis->AddRef();									\
		}															\
		FX_STDMETHODIMP_(uint32_t) theClass::X##localClass::Release()	\
		{															\
			FX_METHOD_PROLOGUE(theClass, localClass)					\
			return pThis->Release();								\
		}															\
		FX_STDMETHODIMP theClass::X##localClass::QueryInterface(firtex::com::FX_REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject)	\
		{															\
			FX_METHOD_PROLOGUE(theClass, localClass)					\
			if ( firtex::com::FX_IsEqualIID( riid, firtex::com::IUnknown::iid ) )\
			{														\
				*ppvObject = static_cast<firtex::com::IUnknown*>(this);	\
				pThis->AddRef();									\
				return FX_S_OK;										\
			}														\
			if ( firtex::com::FX_IsEqualIID( riid, X##localClass::iid ) ) \
			{														\
				*ppvObject = static_cast<X##localClass*>( this );		\
				pThis->AddRef();									\
				return FX_S_OK;										\
			}														\
			return FX_E_NOINTERFACE;									\
		}					

		class GUIDX
		{
		public:
			static tstring encode(const void * pInGUID);
			static bool decode(const tchar* pszIn, void* pOutGUID);
			inline static bool unhex(const tchar* psz, byte* pOut);
		};
	}
}

#endif
