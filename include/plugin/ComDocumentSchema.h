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
// Created	: 2006/7/2
//
#ifndef __COMDOCUMENTSCHEMA_H
#define __COMDOCUMENTSCHEMA_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FirteXCOM.h"
#include "ComObject.h"
#include "../document/DocumentSchema.h"

using namespace firtex::document;

namespace firtex
{
	namespace plugin
	{
		class CComDocumentSchema : public CComObject
		{
		public:
			CComDocumentSchema(CDocumentSchema* pDocSchema);
			virtual ~CComDocumentSchema(void);
		public:
			static IDocumentSchema*	wrap(CDocumentSchema* pDocSchema);
			static CDocumentSchema*	unwrap(IDocumentSchema* pInterface);
		public:
			BEGIN_INTERFACE_PART(DocumentSchema, IDocumentSchema)
				FX_STDMETHOD(addKeywordItem)(/* [in] */firtex::com::FX_BSTR name,
										  /* [out] */firtex::plugin::fieldid_t* pid);

				FX_STDMETHOD(addUnIndexedItem)(/* [in] */firtex::com::FX_BSTR name,
											/* [out] */firtex::plugin::fieldid_t* pid);

				FX_STDMETHOD(addTextItem)(/* [in] */firtex::com::FX_BSTR name,
									   /* [in] */firtex::plugin::store_t store,
									   /* [in] */firtex::plugin::forwardindex_t termVector,
									   /* [out] */firtex::plugin::fieldid_t* pid);

				FX_STDMETHOD(addUnStoredItem)(/* [in] */firtex::com::FX_BSTR name,
										   /* [in] */firtex::plugin::forwardindex_t termVector,
										   /* [out] */firtex::plugin::fieldid_t* pid);

				FX_STDMETHOD(addBinaryItem)(/* [in] */firtex::com::FX_BSTR name,
										 /* [in] */firtex::com::bool_t bCompress,
										 /* [out] */firtex::plugin::fieldid_t* pid);				

			END_INTERFACE_PART(DocumentSchema)
		private:
			CDocumentSchema*	m_pDocSchema;
		};
	}
}

#endif
