#ifndef _QUERYPARSERCONSTANTS_H
#define _QUERYPARSERCONSTANTS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "../utility/StdHeader.h"
#include <sstream>
#include <string>

using namespace std;

namespace firtex
{
	namespace search
	{

#define RANGE_QUERY

		enum QueryTokenTypes
		{
			AND_,
			OR,
			NOT,
			PLUS,
			MINUS,
			LPAREN,
			RPAREN,
			COLON,
			CARAT,
			QUOTED,
			TERM,
			SLOP,
#ifndef NO_FUZZY_QUERY
			FUZZY,
#endif
#ifndef NO_PREFIX_QUERY
			PREFIXTERM,
#endif
#ifndef NO_WILDCARD_QUERY
			WILDTERM,
#endif
#ifdef RANGE_QUERY
			RANGEIN,
			RANGEEX,
#endif
			NUMBER,
			EOF_
		};

		class CQueryParserException : public CFirteXException
		{
		public:
			CQueryParserException(const string& msg,int line,int column):CFirteXException(QUERY_PARSE_ERROR)
			{
				stringstream ss;
				ss << s_errorStrings[QUERY_PARSE_ERROR] << ":" << msg << " at line:" << line << " column:"<<column;

				m_what = ss.str();
			}
			~CQueryParserException(){}		
		};	
	}
}

#endif

