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
// Created	: 2006/7/9
//
#include "plugin/ComReader.h"
#include "com/comerror.h"

namespace firtex
{
	namespace plugin
	{
		CComReader::CComReader(firtex::utility::CReader* pReader) : m_pReader(pReader)
		{
		}

		CComReader::~CComReader(void)
		{
			m_pReader = NULL;
		}
		IReader* CComReader::wrap(firtex::utility::CReader* pReader)
		{
			CComReader* pWrap = new CComReader(pReader);
			CLASS_TO_INTERFACE(CComReader,Reader,pWrap,pXThis);
			firtex::plugin::IReader* pCom = NULL;
			pXThis->QueryInterface(firtex::plugin::IReader::iid,(void**)&pCom);
			return pCom;
		}
		CReader* CComReader::unwrap(IReader* pInterface)
		{
			if ( pInterface == NULL ) return NULL;
			INTERFACE_TO_CLASS(CComReader, Reader, pInterface, pWrapper)
				return (CReader*)pWrapper->m_pReader;			
		}	
		//////////////////////////////////////////////////////////////////////////
		//CComReader IReader
		IMPLEMENT_UNKNOWN(CComReader,Reader)	

		FX_STDMETHODIMP CComReader::XReader::read(/* [in] [retval] */byte* data,/* [in] */int32_t length)
		{
			FX_METHOD_PROLOGUE( CComReader, Reader)
			pThis->m_pReader->read((char*)data,length);
			return FX_S_OK;
		}
		FX_STDMETHODIMP CComReader::XReader::readWithoutCopy(/* [out] [retval] */byte** data,/* [out] [retval] */int32_t* length)
		{
			FX_METHOD_PROLOGUE( CComReader, Reader)

			*data = (byte*)pThis->m_pReader->readWithoutCopy((size_t&)*length);
			return FX_S_OK;
		}
		FX_STDMETHODIMP CComReader::XReader::close()
		{
			FX_METHOD_PROLOGUE( CComReader, Reader)

			pThis->m_pReader->close();
			return FX_S_OK;
		}
		FX_STDMETHODIMP CComReader::XReader::getFilePointer(/* [out] [retval] */int64_t* position)
		{
			FX_METHOD_PROLOGUE( CComReader, Reader)
			*position = pThis->m_pReader->getFilePointer();
			return FX_S_OK;
		}
		FX_STDMETHODIMP CComReader::XReader::seek(/* [in] */int64_t position)
		{
			FX_METHOD_PROLOGUE( CComReader, Reader)
			pThis->m_pReader->seek(position);
			return FX_S_OK;
		}
		FX_STDMETHODIMP CComReader::XReader::isEof(/* [out] [retval] */firtex::com::bool_t* bEof)
		{
			FX_METHOD_PROLOGUE( CComReader, Reader)

			*bEof = pThis->m_pReader->isEof();
			return FX_S_OK;
		}
		FX_STDMETHODIMP CComReader::XReader::length(/* [out] [retval] */int64_t* len)
		{
			FX_METHOD_PROLOGUE( CComReader, Reader)

			*len = pThis->m_pReader->length();
			return FX_S_OK;
		}
	}
}
