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
// Created	: 2006/5/9
//
#include "plugin/FirteXCOM.h"
//#ifdef _DEBUG
//	#pragma comment(lib, "comD")  
//#else
//	#pragma comment(lib, "com")  
//#endif

namespace firtex
{
	namespace plugin
	{
		// {D6CC19E5-3FEA-4c96-B50C-C176080E89C5}
		firtex::com::FX_IID const IParserPlugin::iid = { 0xd6cc19e5, 0x3fea, 0x4c96, { 0xb5, 0xc, 0xc1, 0x76, 0x8, 0xe, 0x89, 0xc5 } };
		// {31F417A2-5111-4e49-8D14-EA1D63F6CD5D}
		firtex::com::FX_IID const IDocumentInput::iid = { 0x31f417a2, 0x5111, 0x4e49, { 0x8d, 0x14, 0xea, 0x1d, 0x63, 0xf6, 0xcd, 0x5d } };	
		// {09B42270-8ACD-4e8a-B125-A79B46003F5D}		
		firtex::com::FX_IID const IDocumentSchema::iid = { 0x9b42270, 0x8acd, 0x4e8a, { 0xb1, 0x25, 0xa7, 0x9b, 0x46, 0x0, 0x3f, 0x5d } };
		// {C92FA9F4-86FD-48c1-AB00-B16C78D2983F}
		firtex::com::FX_IID const IDocument::iid = { 0xc92fa9f4, 0x86fd, 0x48c1, { 0xab, 0x0, 0xb1, 0x6c, 0x78, 0xd2, 0x98, 0x3f } };
		// {750C8BE7-1858-4c61-A97C-C864CF552EFD}
		firtex::com::FX_IID const IReader::iid = { 0x750c8be7, 0x1858, 0x4c61, { 0xa9, 0x7c, 0xc8, 0x64, 0xcf, 0x55, 0x2e, 0xfd } };
		// {E6F0F10C-DF0B-411a-9A3E-EA2BCD4DF256}
		firtex::com::FX_IID const ITokens::iid = { 0xe6f0f10c, 0xdf0b, 0x411a, { 0x9a, 0x3e, 0xea, 0x2b, 0xcd, 0x4d, 0xf2, 0x56 } };
		// {7D7D876B-CCE4-4fe0-8389-A20106A828A3}
		firtex::com::FX_IID const IAnalyzerPlugin::iid = { 0x7d7d876b, 0xcce4, 0x4fe0, { 0x83, 0x89, 0xa2, 0x1, 0x6, 0xa8, 0x28, 0xa3 } };


	}
}