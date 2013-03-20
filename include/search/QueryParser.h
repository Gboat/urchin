#ifndef __QUERYPARSER_H
#define __QUERYPARSER_H
#include <string>
#include <vector>
#include "../analyzer/Analyzer.h"
#include "QueryParserConstants.h"
#include "../utility/StringReader.h"
#include "Query.h"


using namespace std;

namespace firtex
{
	namespace index
	{
		class CIndexReader;
	}
}


namespace firtex
{
	namespace search
	{

#define CONJ_NONE   0
#define CONJ_AND    1
#define CONJ_OR     2

#define MOD_NONE    0
#define MOD_NOT     10
#define MOD_REQ     11
		
		class CTokenList;
		class CBooleanClause;
		class CQueryToken;
		class CQueryParser
		{
		public:
			typedef map<tstring,CAnalyzer*> analyzer_map;
			typedef vector<pair<tstring,CAnalyzer*> > analyzer_list;
		public:
			/**
			 * constructor
			 * @param field field name, also as default field for searching
			 * @param pAnalyzer the analyzer for field
			 * @param bDeleteAnalyzer delete pAnlyzer in destructor or not 
			 */
			CQueryParser(const tchar* field,CAnalyzer* pAnalyzer,bool bDeleteAnalyzer);

			/**
			 * constructor
			 * @param fields field names, the first field is the default field for searching
			 * @param pAnalyzers analyzers for fields
			 * @param nNumFields number of fields
			 */
			CQueryParser(const tchar** fields,CAnalyzer** pAnalyzers,int32_t nNumFields);

			/**
			 * constructor
			 * @param fields field names, the first field is the default field for searching
			 * @param pAnalyzers analyzers for fields
			 * @param nNumFields number of fields
			 */
			CQueryParser(analyzer_list& analyzers);

			virtual ~CQueryParser(void);
		public:
			/**
			 * parse a query expression and create a query object represent this query expression
			 * @param query query expression
			 * @param field field name, as a default field for searching
			 * @param pAnalyzer the analyzer for field
			 * @param bDeleteAnalyzer delete pAnlyzer in destructor or not 
			 * @return a query object represent this query expression, must be deleted by caller.
			 */
			static CQuery*			parse(const tchar* query,const tchar* field,CAnalyzer* pAnalyzer,bool bDeleteAnalyzer);

			/**
			 * create a query parser
			 * @param pIndexReader index reader
			 * @return a query parser for this index reader, MUST be deleted by the caller.
			 */
			static CQueryParser*	createQueryParser(firtex::index::CIndexReader* pIndexReader);
		public:
			/**
			 * parse query expression
			 * @param query query string
			 * @return a query object represent the query expression, not an internal object, must destroy by caller			 
			 */
			CQuery*				parse(const tchar* query);

			/**
			 * parse query expression
			 * @param query query string
			 * @return a query object represent the query expression, not an internal object, must destroy by caller			 
			 */
			CQuery*				parse(const tchar* query,const tchar* field);

			/**
			 * attach an analyzer for field
			 * @param field field name
			 * @param pAnalyzer analyzer for field			 
			 */
			void				attachAnalyzer(const tchar* field,CAnalyzer* pAnalyzer);

			/**
			 * detach an analyzer for field
			 * @param field field name
			 * @return an analyzer has been attached for this field			 
			 */
			CAnalyzer*			detachAnalyzer(const tchar* field);

			/**
			 * get analyzer of field
			 * @param field field name
			 * @return an analyzer has been attached for this field, just return the reference 
			 */
			CAnalyzer*			getAnalyzer(const tchar* field);
		protected:
			CQuery*				parse(CStringReader* reader);			

			CAnalyzer*			getAnalyzer(const tstring& field);
		private:
			/**
			 * match CONJUNCTION
			 * CONJUNCTION ::= <AND> | <OR>
			 */
			int32_t				matchConjunction(CTokenList* pTokenList);

			/**
			 * match MODIFIER
			 * MODIFIER ::= <PLUS> | <MINUS> | <NOT>
			 */
			int32_t				matchModifier(CTokenList* pTokenList);


			/** 			 
			 * match query expression
			 * QUERY ::= [MODIFIER] CLAUSE (<CONJUNCTION> [MODIFIER] CLAUSE)
			 * @param field field name
			 * @return query object represent the query expression
			 */
			CQuery*				matchQuery(const tstring& field,CTokenList* pTokenList);

			/**
			 * match CLAUSE
			 * CLAUSE ::= [TERM <COLON>] ( TERM | (<LPAREN> QUERY <RPAREN>))
			 */
			CQuery*				matchClause(const tstring& field,CTokenList* pTokenList);

			/**
			 * match TERM
			 * TERM ::= TERM | PREFIXTERM | WILDTERM | NUMBER
			 *			[ <FUZZY> ] [ <CARAT> <NUMBER> [<FUZZY>]]			 
			 *			| (<RANGEIN> | <RANGEEX>) [<CARAT> <NUMBER>]
			 *			| <QUOTED> [SLOP] [<CARAT> <NUMBER>]
			 */
			CQuery*				matchTerm(const tstring& field,CTokenList* pTokenList);

			/** 
			 * match a given QueryToken£¬an exception will be threw if mismatch
			 * @param expectedType token type expected
			 */
			CQueryToken*		matchQueryToken(QueryTokenTypes expectedType,CTokenList* pTokenList);

			/** 
			 * extract the first token and delete it
			 */
			void extractAndDeleteToken(CTokenList* pTokenList);

			/**
			 * add a clause
			 */
			void addClause(vector<CBooleanClause*>* clauses, int32_t conj, int32_t mods, CQuery* q);

			string	discardEscapeChar(const tstring& token);
			CQuery* getFieldQuery(const tstring& field, const tstring& queryText);
			CQuery* getRangeQuery(const tstring& field, const tstring& queryText, bool inclusive);

		private:
			analyzer_map	m_analyzers;			

			CFXString		m_sDefaultField;	///default field for searching
			CAnalyzer*		m_pDefaultAnalyzer;		

			bool			m_lowercaseWildcardTerms;
		};
	}
}

#endif
