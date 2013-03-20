#ifndef _LEXER_H
#define _LEXER_H

#include "../utility/StdHeader.h"
#include "QueryParserConstants.h"
#include "../utility/FastCharStream.h"
#include "../utility/StringReader.h"

using namespace firtex::utility;
using namespace std;


namespace firtex
{
	namespace search
	{
		class CTokenList;
		class CQueryToken;
		class CLexer
		{
		public:			
			CLexer(const tstring& query);			
			CLexer(CStringReader* source);						
			~CLexer();
		public:
			void lex(CTokenList *tokens);
		private:
			CQueryToken* getNextToken();
			
			/**
			 * ¶ÁÕûÊý
			 */
			string readIntegerNumber(const tchar ch);

#ifdef RANGE_QUERY
			/**
			 * Reads an inclusive range like [some words]
			 */			
			CQueryToken* readInclusiveRange(const tchar prev);


			/**
			 * Reads an exclusive range like {some words}
			 */ 
			CQueryToken* readExclusiveRange(const tchar prev);
#endif

			/**
			 * Reads quoted string like "something else"
			 */			
			CQueryToken* readQuoted(const tchar prev);

			/**
			 * Reads a term
			 */
			CQueryToken* readTerm(const tchar prev);

			/**
			 * Reads Escape char
			 */
			string readEscape(const tchar prev);
		private:
			CFastCharStream* reader;			
			bool	bOwn;
		};
	}
}

#endif
