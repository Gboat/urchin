#include "search/Lexer.h"

#include "search/QueryToken.h"
#include "search/TokenList.h"
using namespace firtex::utility;

namespace firtex
{
	namespace search
	{
		CLexer::CLexer(const string& query) 
		{
			CStringReader *r = new CStringReader(query);					
			reader = new CFastCharStream(r);					
			bOwn = true;
		}
		CLexer::CLexer(CStringReader* source) 
		{			
			reader = new CFastCharStream(source);			
			bOwn  = false;
		}

		CLexer::~CLexer() 
		{		
			if (bOwn) 
			{
				delete(reader->input);
			}
			delete reader;
		}

		void CLexer::lex(CTokenList *tokenList) 
		{			
			CQueryToken* token = NULL;
			
			while((token = getNextToken()) != NULL) 
			{			
				tokenList->add(token);
			}
			
			token = new CQueryToken(EOF_);			
			
			tokenList->add(token);
		}


		CQueryToken* CLexer::getNextToken() 
		{
			while(!reader->isEos())
			{
				char ch = reader->getNext();
				//Ìø¹ý¿Õ¸ñ
				if( _istspace(ch)!=0 ) 
				{
					continue;
				}
				char buf[250] = {ch,'\0'}; //todo: why does this buffer need to be 250
				switch(ch) 
				{
				case '+':
					return new CQueryToken(buf ,PLUS);
				case '-':
					return new CQueryToken(buf, MINUS);
				case '(':
					return new CQueryToken(buf, LPAREN);
				case ')':
					return new CQueryToken(buf, RPAREN);
				case ':':
					return new CQueryToken(buf, COLON);
				case '!':
					return new CQueryToken(buf, NOT);
				case '^':
					return new CQueryToken(buf, CARAT);
#ifdef FUZZY_QUERY
				case '~':
					if( _istdigit( reader->peek() )!=0 ) 
					{
						string number = readIntegerNumber(ch);
						CQueryToken* ret = new CQueryToken(number, SLOP);						
						return ret;
					}
					else
					{
						return new CQueryToken(buf,FUZZY);
					}
#endif
				case '"':
					return readQuoted(ch);
#ifdef RANGE_QUERY
				case '[':
					return readInclusiveRange(ch);
				case '{':
					return readExclusiveRange(ch);
#endif
				case ']':
				case '}':
				case '*':
					throw CQueryParserException("Unrecognized char",reader->column(), reader->line());															
				default:
					return readTerm(ch);
				}
			}
			return NULL;
		}
		tstring CLexer::readIntegerNumber(const char ch)
		{			
			tstring number;
			number.push_back(ch);
			while (!reader->isEos() && _istdigit(reader->peek())!=0)
			{
				number.push_back(reader->getNext());
			}
			return number;
		}


#ifdef RANGE_QUERY
		CQueryToken* CLexer::readInclusiveRange(const tchar prev) 
		{		
			tchar ch = prev;
			tstring rang;
            rang.push_back(ch);
			while (!reader->isEos())
			{
				ch = reader->getNext();
				rang.push_back(ch);
				if(ch == _T(']'))
					return new CQueryToken(rang,RANGEIN);
			}			
			throw CQueryParserException(_T("Unterminated inclusive range! %d::%d."),reader->column(), reader->line());
			return NULL;			
		}
		CQueryToken* CLexer::readExclusiveRange(const tchar prev) 
		{			
			char ch = prev;
			string rang;
			rang.push_back(ch);
			while (!reader->isEos())
			{
				ch = reader->getNext();
				rang.push_back(ch);
				if(ch == '}')
					return new CQueryToken(rang,RANGEEX);
			}			
			throw CQueryParserException(_T("Unterminated exclusive range! %d::%d."),reader->column(), reader->line());
			return NULL;			
		}
#endif

		CQueryToken* CLexer::readQuoted(const char prev) 
		{			
			char ch = prev;
			string quoted;
			quoted.push_back(ch);
			while (!reader->isEos())
			{
				ch = reader->getNext();
				quoted.push_back(ch);
				if(ch == '"')
					return new CQueryToken(quoted,QUOTED);
			}			
			throw CQueryParserException("Unterminated string ",reader->column(), reader->line());
			return NULL;			
		}

		CQueryToken* CLexer::readTerm(const char prev) 
		{
			char ch = prev;
			bool completed = false;
			int asteriskCount = 0;
			bool hasQuestion = false;

			string val;
			while(true) 
			{
				switch(ch) 
				{
				case '\\':
					{
						string re = readEscape(ch);
						val.append( re );						
					}
					break;
				case '*':
					asteriskCount++;
					val.push_back(ch);
					break;
#ifdef WILDCARD_QUERY
				case '?':
					hasQuestion = true;
					val.push_back(ch);
					break;
#endif
				case '\n':
				case '\t':
				case ' ':
				case '+':
				case '-':
				case '!':
				case '(':
				case ')':
				case ':':
				case '^':
#ifdef RANGE_QUERY
				case '[':
				case ']':
				case '{':
				case '}':
#endif
#ifdef FUZZY_QUERY
				case '~':
#endif
				case '"':					
					reader->unget();
					completed = true;
					break;
				default:
					val.push_back(ch);
					break;
					
				}// end of switch

				if(completed || reader->isEos())
					break;
				else
					ch = reader->getNext();
			}
			
			if(false)
				return NULL;
#ifdef WILDCARD_QUERY
			else if(hasQuestion)
				return new CQueryToken(val, WILDTERM);
#endif
#ifdef PREFIX_QUERY
			else if(asteriskCount == 1 && val[val.size() - 1] == '*')
				return new CQueryToken(val, PREFIXTERM);
#endif
#ifdef WILDCARD_QUERY
			else if(asteriskCount > 0)
				return new CQueryToken(val, WILDTERM);
#endif
			else if( _tcsicmp(val.c_str(), _T("AND"))==0 || _tcscmp(val.c_str(), _T("&&"))==0 )
				return new CQueryToken(val, AND_);
			else if( _tcsicmp(val.c_str(), _T("OR"))==0 || _tcscmp(val.c_str(), _T("||"))==0)
				return new CQueryToken(val, OR);
			else if( _tcsicmp(val.c_str(), _T("NOT"))==0 )
				return new CQueryToken(val, NOT);
			else
			{
				bool isnum = true;
				int nlen = (int)val.size();
				for (int i=0;i<nlen;i++) 
				{
					char ch=val[i];
					//todo: should we also check for spaces,probably not???
					if ( ch < 0 || _istalpha(ch) ) 
					{
						isnum=false;
						break;
					}
				}

				if ( isnum )
					return new CQueryToken(val, NUMBER);
				else
					return new CQueryToken(val, TERM);
			}
		}

		string CLexer::readEscape(const char prev) 
		{
			char ch = prev;
			string val;
			val.push_back(ch);

			ch = reader->getNext();
			int idx = (int)_tcscspn( val.c_str(), _T("\\+-!():^[]{}\"~*") );
			if(idx == 0) 
			{
				val.push_back( ch );
				return val;
			}			
			throw CQueryParserException("Unrecognized escape sequence ",reader->column(), reader->line());						
			return "";
		}

	}
}
