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
#ifndef ____EXCEPTION_H__
#define ____EXCEPTION_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <iostream>
#include <string>
#include <sstream>
using namespace std;


namespace firtex
{
	namespace utility
	{		

		typedef unsigned short exception_type;
		class CFirteXException
		{
		public:
			CFirteXException(exception_type code,const string& msg = "") :m_code(code),m_what(s_errorStrings[code] + ":" + msg)
			{
			}
			CFirteXException(exception_type code,const string& msg,const string& file,int line) : m_code(code)
			{
				stringstream ss;
				ss << s_errorStrings[code] << ":" << msg << " at file: " << file << ",line:" << line;
				m_what = ss.str();
			}
			CFirteXException(const CFirteXException& clone):m_code(clone.m_code),m_what(clone.m_what)
			{
			}
			CFirteXException&operator=(const CFirteXException& right)
			{
				m_code = right.m_code;
				m_what = right.m_what;
			}
			virtual~CFirteXException(){}

			enum
			{
				UNKNOWN_ERROR,
				GENERIC_ERROR,
				MISSING_PARAMETER_ERROR,
				BAD_PARAMETER_ERROR,
				FILEIO_ERROR,
				RUNTIME_ERROR,
				OUTOFMEM_ERROR,
				ILLEGALARGUMENT_ERROR,
				UNSUPPORTED_ERROR,
				CAST_ERROR,
				OUTOFORDER_ERROR,
				OUTOFRANGE_ERROR,
				PARSE_ERROR,
				ANALYZE_ERROR,
				INDEX_COLLAPSE_ERROR,
				QUERY_PARSE_ERROR,
				VERSION_ERROR,
				ASSERT_ERROR,
				NUMBER_FORMAT_ERROR,
				NUM_ERRORS
			};
		public:
			virtual const char*		what(){return m_what.c_str();}
			virtual exception_type	code(){return m_code;}
		protected:
			static string s_errorStrings[NUM_ERRORS];
			string			m_what;
			exception_type	m_code;
		};		

		class CIllegalArgumentException : public CFirteXException
		{
		public:
			CIllegalArgumentException(const string& msg):CFirteXException(ILLEGALARGUMENT_ERROR,msg)
			{
			}
			~CIllegalArgumentException(){}		
		};		

		class CUnsupportedOperationException : public CFirteXException
		{
		public:
			CUnsupportedOperationException(const string& msg):CFirteXException(UNSUPPORTED_ERROR,msg)
			{
			}
			~CUnsupportedOperationException(){}		
		};		

		class CFileIOException : public CFirteXException
		{
		public:
			CFileIOException(const string& msg):CFirteXException(FILEIO_ERROR,msg)
			{
			}
			~CFileIOException(){}		
		};		

		#define FIRTEX_THROW1(code) throw firtex::utility::CFirteXException(firtex::utility::CFirteXException::code)
		#define FIRTEX_THROW2(code,msg) throw firtex::utility::CFirteXException(firtex::utility::CFirteXException::code,msg)		
		#define FIRTEX_THROW3(code,msg) throw firtex::utility::CFirteXException(firtex::utility::CFirteXException::code,msg,__FILE__, __LINE__)		
		#define FIRTEX_THROW_LINE(code,msg,file,line) throw firtex::utility::CFirteXException(firtex::utility::CFirteXException::code,msg,file,line)
		#define FIRTEX_RETHROW(e) throw firtex::utility::CFirteXException(e)

	#ifdef _DEBUG
		#define FIRTEX_ASSERT(exp,msg) if(!(exp)){throw firtex::utility::CFirteXException(firtex::utility::CFirteXException::ASSERT_ERROR,msg,__FILE__, __LINE__);}
		#define FIRTEX_ASSERT2(exp) if(!(exp)){throw firtex::utility::CFirteXException(firtex::utility::CFirteXException::ASSERT_ERROR,"", __FILE__, __LINE__);}
	#else 
		#define FIRTEX_ASSERT(exp,msg)
		#define FIRTEX_ASSERT2(exp)
	#endif
	}
}
#endif

