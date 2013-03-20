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
// Created	: 2005/12/16
//
#include "utility/Exception.h"

namespace firtex
{
	namespace utility
	{
		string CFirteXException::s_errorStrings[NUM_ERRORS] = 
		{
			"Unknown error",				//UNKNOWN_ERROR
			"Generic error",				//GENERIC_ERROR
			"Missing parameter",			//MISSING_PARAMETER_ERROR
			"Bad parameter",				//BAD_PARAMETER_ERROR
			"File I/O error",				//FILEIO_ERROR
			"Rumtime error",				//RUNTIME_ERROR
			"Out of memory",				//OUTOFMEM_ERROR
			"Illegal argument",				//ILLEGALARGUMENT_ERROR
			"Unsopported operation",		//UNSUPPORTED_ERROR
			"Type cast error",				//CAST_ERROR
			"Out of order",					//OUTOFORDER_ERROR
			"Out of range",					//OUTOFRANG_ERROR
			"Parse error",					//PARSE_ERROR
			"Analyze error",				//ANALYZE_ERROR
			"Index file collpased",			//INDEX_COLLAPSE_ERROR
			"Query parse error",			//QUERY_PARSE_ERROR
			"Version error",				//VERSION_ERROR
			"Assert error",					//ASSERT_ERROR
			"Number format error",			//NUMBER_FORMAT_ERROR,
		};
	}
}
