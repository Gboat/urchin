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
// Created	: 2005/10/20
//
#ifndef __TYPEDEFS_H
#define __TYPEDEFS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "platform.h"
#include <string>

namespace firtex
{
	///document's field id 
	typedef int32_t		fieldid_t;

	///document's schema id
	typedef fieldid_t	schemaid_t;

	typedef int32_t		termid_t;
	typedef int32_t		wordid_t;
	/// term position
	typedef termid_t	loc_t;
	/// internal document id encoding.
	typedef termid_t	docid_t;
	/// internal term id encoding.
	typedef termid_t	count_t;
	/// integral frequencies
	typedef count_t		freq_t;

	/// floating point frequencies and/or scores.
	typedef float		score_t;
	typedef float		weight_t;
	/// external term encoding
	typedef std::string term_t;	

	typedef	int32_t		norm_t;	///norm byte

	typedef struct _tag_data_record
	{
		byte*	data;
		size_t	length;
	}datarecord;

	template<typename CharT>
		struct _str 
		{
			CharT*	str;
			size_t	length;
		};
	
	typedef _str<char>		str_t;
	typedef _str<wchar_t>	wstr_t;
}
#endif
