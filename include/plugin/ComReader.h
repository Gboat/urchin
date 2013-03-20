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
#ifndef __COMREADER_H
#define __COMREADER_H

#include "FirteXCOM.h"
#include "ComObject.h"
#include "../utility/Reader.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace firtex
{
	namespace plugin
	{
		class CComReader :	public CComObject
		{
		public:
			CComReader(firtex::utility::CReader* pReader);
			virtual ~CComReader(void);
		public:
			static IReader*		wrap(firtex::utility::CReader* pReader);
			static CReader*		unwrap(IReader* pInterface);
		public:
			BEGIN_INTERFACE_PART(Reader, IReader)				
				FX_STDMETHOD(read)(/* [in] [retval] */byte* data,/* [in] */int32_t length);
				FX_STDMETHOD(readWithoutCopy)(/* [out] [retval] */byte** data,/* [out] [retval] */int32_t* length);
				FX_STDMETHOD(close)();
				FX_STDMETHOD(getFilePointer)(/* [out] [retval] */int64_t* position);
				FX_STDMETHOD(seek)(/* [in] */int64_t position);
				FX_STDMETHOD(isEof)(/* [out] [retval] */firtex::com::bool_t* bEof);
				FX_STDMETHOD(length)(/* [out] [retval] */int64_t* len);
			END_INTERFACE_PART(Reader)

		private:
			firtex::utility::CReader*	m_pReader;
		};
	}
}

#endif

