
#include <memory>
#include "search/QueryParser.h"
#include "search/TokenList.h"
#include "search/Lexer.h"
#include "search/QueryToken.h"
#include "search/BooleanClause.h"
#include "search/PhraseQuery.h"
#include "search/BooleanQuery.h"
#include "search/RangeQuery.h"
#include "search/TermQuery.h"
#include "utility/StringUtils.h"
#include "index/IndexReader.h"
#include "document/DocumentSchema.h"
#include "index/Index.h"
#include "analyzer/AnalyzerFactory.h"

namespace firtex
{
	namespace search
	{		
		CQueryParser::CQueryParser(const tchar* field,CAnalyzer* pAnalyzer,bool bDeleteAnalyzer)
			: m_sDefaultField(field)
			, m_pDefaultAnalyzer(pAnalyzer)
		{
			if(bDeleteAnalyzer)
				m_analyzers.insert(make_pair(tstring(field),pAnalyzer));			
			m_lowercaseWildcardTerms = true;
		}
		CQueryParser::CQueryParser(const tchar** fields,CAnalyzer** pAnalyzers,int32_t nNumFields)
		{
			if(nNumFields <= 0)
				FIRTEX_THROW3(ILLEGALARGUMENT_ERROR,_T("out of range."));
			m_sDefaultField = fields[0];
			m_pDefaultAnalyzer = pAnalyzers[0];
			for(int32_t i = 0;i<nNumFields;i++)
			{
				m_analyzers.insert(make_pair(tstring(fields[i]),pAnalyzers[i]));
			}
		}

		CQueryParser::CQueryParser(analyzer_list& analyzers)
		{			
			if(analyzers.size() <= 0)
				FIRTEX_THROW3(ILLEGALARGUMENT_ERROR,_T("No analyzer."));
			analyzer_list::iterator iter = analyzers.begin();
			while (iter != analyzers.end())
			{
				m_analyzers.insert(*iter);
				iter++;
			}
			m_sDefaultField = analyzers[0].first;
			m_pDefaultAnalyzer = analyzers[0].second;
		}

		CQueryParser::~CQueryParser(void)
		{
			analyzer_map::iterator iter = m_analyzers.begin();
			while(iter!= m_analyzers.end())
			{
				delete iter->second;
				iter++;
			}
			m_analyzers.clear();

			m_pDefaultAnalyzer = NULL;			
		}

		CQueryParser* CQueryParser::createQueryParser(firtex::index::CIndexReader* pIndexReader)
		{
			CQueryParser::analyzer_list analyzers;
			firtex::document::CDocumentSchema* pSchema = pIndexReader->getIndex()->getSchema();

			pSchema->startIterator();
			CSchemaItem* pItem = NULL;
			tstring sField;
			firtex::analyzer::CAnalyzer* pAnalyzer = NULL;
			while (pSchema->hasNext())
			{
				pItem = pSchema->next();
				if(pItem->getIndex() == Index::ANALYZE)
				{
					if(pItem->getAlias() && _tcsicmp(pItem->getAlias(),_T("")) )
						sField = pItem->getAlias();
					else 
						sField = pItem->getName();

					const tchar* _identifier = pItem->getComponent(_T("analyzer"));
					const tchar* _param = pItem->getComponent(_T("analyzer_parameter"));
					if(_identifier && _tcsicmp(_identifier,_T("")))
					{						
						pAnalyzer = firtex::analyzer::CAnalyzerFactory::createAnalyzer(_identifier,_param);
					}
					else 
						pAnalyzer = firtex::analyzer::CAnalyzerFactory::standardAnalyzer(_param);

					analyzers.push_back(pair<tstring,CAnalyzer*>(sField,pAnalyzer));
				}
			}

			return new CQueryParser(analyzers);
		}	

		CQuery*	CQueryParser::parse(const tchar* query,const tchar* field,CAnalyzer* pAnalyzer,bool bDeleteAnalyzer)
		{
			CQueryParser parser(field,pAnalyzer,bDeleteAnalyzer);
			return parser.parse(query);			
		}

		CQuery*	CQueryParser::parse(const tchar* query)
		{			
			CStringReader* pReader = new CStringReader(query);
			std::auto_ptr<CStringReader> reader_ptr(pReader);
			CQuery* ret = NULL;
			try
			{			
				ret = parse(pReader);
			}
			catch(CFirteXException& fe)
			{				
				FIRTEX_RETHROW(fe);
			}
			catch(...)
			{				
				return NULL;
			}			
			return ret;			
		}
		CQuery* CQueryParser::parse(const tchar* query,const tchar* field)
		{
			if(field && *field)
			{
				tstring sQuery = field;
				sQuery += _T(":");
				sQuery += query;
				return parse(sQuery.c_str());
			}
			return parse(query);
		}

		CQuery* CQueryParser::parse(CStringReader* reader)
		{
			CTokenList* pTokenList = new CTokenList();
			std::auto_ptr<CTokenList> token_list(pTokenList);
			
			CLexer lexer(reader);
			lexer.lex(pTokenList);
	
			if (pTokenList->peek()->m_type == EOF_)
			{				
				return NULL;
			}
			CQuery* ret = matchQuery(m_sDefaultField,pTokenList);

			return ret;
		}

		int32_t CQueryParser::matchConjunction(CTokenList* pTokenList)
		{
			switch(pTokenList->peek()->m_type)
			{
			case AND_ :
				{				
					extractAndDeleteToken(pTokenList);
					return CONJ_AND;
				}
			case OR:
				{	
                    extractAndDeleteToken(pTokenList);
					return CONJ_OR;
				}
			default : 				
				return CONJ_NONE;					  
			}
		}

		int32_t CQueryParser::matchModifier(CTokenList* pTokenList)
		{
			switch(pTokenList->peek()->m_type)
			{
			case PLUS:
				{
                    extractAndDeleteToken(pTokenList);
					return MOD_REQ;
				}

			case MINUS :
			case NOT :
				{
                    extractAndDeleteToken(pTokenList);
					return MOD_NOT;
				}
			default :				
				return MOD_NONE;					 
			}
		}

		CQuery* CQueryParser::matchQuery(const tstring& field,CTokenList* pTokenList)
		{
			if(pTokenList == NULL)
				return NULL;			

			vector<CBooleanClause*> clauses;
			CQuery* q = NULL;			

			int32_t mods = MOD_NONE;
			int32_t conj = CONJ_NONE;
			
			mods = matchModifier(pTokenList);
			
			q = matchClause(field,pTokenList);
			if(q!=NULL)
			{
				addClause(&clauses, CONJ_NONE, mods, q);				
			}
			
			while(true)
			{
				CQueryToken* p = pTokenList->peek();
                if(p->m_type == EOF_)
				{
					CQueryToken* qt = matchQueryToken(EOF_,pTokenList);
					delete qt;
					break;
				}
				if(p->m_type == RPAREN)
				{
					//MatchQueryToken(RPAREN);
					break;
				}
				///match AND OR NOT
				conj = matchConjunction(pTokenList);
				///match modifier
				mods = matchModifier(pTokenList);

				q = matchClause(field,pTokenList);
				if ( q != NULL )
				{
					addClause(&clauses, (conj == CONJ_NONE)?CONJ_AND:conj, mods, q);
				}
			}
			
			if(clauses.size() <=0)
				return NULL;
			if(clauses.size() == 1)
			{ 
				CBooleanClause* c = clauses[0];
				CQuery* q = c->getQuery()->clone();										
				clauses.clear();
				delete c;
				return q;
			}
			else
			{
				CBooleanQuery* query = new CBooleanQuery();
				vector<CBooleanClause*>::iterator iter = clauses.begin();
				while(iter != clauses.end())
				{					
					query->add(*iter);
					iter++;
				}
				return query;
			}
		}		

		CQuery* CQueryParser::matchClause(const tstring& field,CTokenList* pTokenList)
		{
			CQuery* q = NULL;
			tstring sfield = field;
			CQueryToken *DelToken = NULL;

			//match for [TERM <COLON>]
			CQueryToken* term = pTokenList->extract();
			if(term->m_type == TERM && pTokenList->peek()->m_type == COLON)
			{
				DelToken = matchQueryToken(COLON,pTokenList);				
				delete(DelToken);
				sfield = discardEscapeChar(term->m_value);
				delete(term);
			}
			else
			{
				pTokenList->push(term);
			}

			// match for
			// TERM | (<LPAREN> QUERY <RPAREN>)
			if(pTokenList->peek()->m_type == LPAREN)
			{
				DelToken = matchQueryToken(LPAREN,pTokenList);
				delete(DelToken);

				q = matchQuery(sfield,pTokenList);
				
				try
				{
					DelToken = matchQueryToken(RPAREN,pTokenList);	
					delete(DelToken);

				}
				catch(...) 
				{
					delete(q);
					throw;
				}
			}
			else
			{
				q = matchTerm(sfield,pTokenList);
			}
			return q;
		}


		CQuery* CQueryParser::matchTerm(const tstring& field,CTokenList* pTokenList)
		{
			// TERM ::= TERM | PREFIXTERM | WILDTERM | NUMBER
			//          [ <FUZZY> ] [ <CARAT> <NUMBER> [<FUZZY>]]
			//  	    | (<RANGEIN> | <RANGEEX>) [<CARAT> <NUMBER>]
			//		    | <QUOTED> [SLOP] [<CARAT> <NUMBER>]
		
			CQueryToken* term = NULL;
			CQueryToken* slop = NULL;
			CQueryToken* boost = NULL;

			bool prefix = false;
			bool wildcard = false;
			bool fuzzy = false;
			bool rangein = false;
			CQuery* q = NULL;

			term = pTokenList->extract();
			CQueryToken* DelToken = NULL; //Token that is about to be deleted

			switch(term->m_type)
			{
			case TERM:
			case NUMBER:
#ifdef PREFIX_QUERY
			case PREFIXTERM:
#endif
#ifdef WILDCARD_QUERY
			case WILDTERM:
#endif
				{ //start case
#ifdef PREFIX_QUERY
					//Check if type of CQueryToken term is a prefix term
					if(term->m_type == PREFIXTERM)
					{
						prefix = true;
					}
#endif
#ifdef WILDCARD_QUERY
					//Check if type of CQueryToken term is a wildcard term
					if(term->m_type == WILDTERM)
					{
						wildcard = true;
					}
#endif

#ifdef FUZZY_QUERY
					//Peek to see if the type of the next token is fuzzy term
					if(pTokenList->peek()->m_type == FUZZY)
					{
						DelToken = matchQueryToken(FUZZY,pTokenList);
						delete(DelToken);
						fuzzy = true;
					}
#endif
					if(pTokenList->peek()->m_type == CARAT)
					{
						DelToken = matchQueryToken(CARAT,pTokenList);
						delete(DelToken);
						boost = matchQueryToken(NUMBER,pTokenList);
#ifdef FUZZY_QUERY
						if(pTokenList->peek()->m_type == FUZZY)
						{
							DelToken = matchQueryToken(FUZZY,pTokenList);						
							delete(DelToken);
							fuzzy = true;
						}
#endif
					} //end if type==CARAT

					if (false)
					{
					}
#ifdef WILDCARD_QUERY
					else if(wildcard)
					{
						CTerm* t = NULL;
						tstring sTmp = discardEscapeChar(term->m_value);
						if ( m_lowercaseWildcardTerms )
						{
							_tcslwr((char*)sTmp.c_str());
							t = new CTerm(field, sTmp);
						}
						else
						{
							t = new CTerm(field, sTmp);
						}
						q = new CWildcardQuery(t);
						delete t;						
						break;
					}
#endif
#ifdef PREFIX_QUERY
					else if(prefix)
					{
						//Create a PrefixQuery
						q = getPrefixQuery(field,term->m_value,m_lowercaseExpandedTerms);
						break;
					}
#endif
#ifdef FUZZY_QUERY
					else if(fuzzy)
					{
						//Create a FuzzyQuery
						q = getFuzzyQuery(field,term->m_value,m_lowercaseExpandedTerms);
						break;
					}
#endif
					else
					{
						q = getFieldQuery(field, term->m_value);
						break;
					}
				}//case TERM:
				//case NUMBER:
				//case PREFIXTERM:
				//case WILDTERM:

#ifdef RANGE_QUERY
			case RANGEIN:
			case RANGEEX:
				{                    
					if(term->m_type == RANGEIN)				
					{
						rangein = true;
					}
					if(pTokenList->peek()->m_type == CARAT)
					{
						DelToken = matchQueryToken(CARAT,pTokenList);
						delete(DelToken);
						boost = matchQueryToken(NUMBER,pTokenList);
					}
					
					q = getRangeQuery(field,term->m_value, rangein);			
										
				} //case RANGEIN:
				break;											 
#endif
			case QUOTED:
				{
					if(pTokenList->peek()->m_type == SLOP)
					{
						slop = matchQueryToken(SLOP,pTokenList);
					}
                    
					if(pTokenList->peek()->m_type == CARAT)
					{
						DelToken = matchQueryToken(CARAT,pTokenList);
						delete(DelToken);
						boost = matchQueryToken(NUMBER,pTokenList);
					}					

					tstring quotedValue;
					quotedValue.assign(term->m_value.begin() + 1,term->m_value.end() - 1);
					q = getFieldQuery(field, quotedValue);					
					if(slop != NULL && q != NULL && q->instanceOf(CPhraseQuery::getClassName()) )
					{
						try 
						{
							char* end;
							int32_t s = (int32_t)_tcstoi64(slop->m_value.c_str()+1, &end, 10);
							((CPhraseQuery*)q)->setSlop( s );
						}
						catch(...)
						{
							//ignored
						}
					}
					delete(slop);//bvk: moved delting of slop to after
					//the CPhraseQuery check. Otherwise use of slop
					//with non-phrasequeries will leak memory
				} //case QUOTED:
			} // end of switch

			delete(term);

			if( q!=NULL && boost != NULL )
			{
				float f = 1.0F;
				try 
				{
					//TODO: check this
					/*char* tmp;
					f = _tcstod(boost->m_value.c_str(), &tmp);*/					
					f = (float)atof(boost->m_value.c_str());
					//f = Single.Parse(boost->m_value, NumberFormatInfo.InvariantInfo);
				}
				catch(...)
				{ 
					//ignored
				}
				delete(boost);

				q->setBoost( f);
			}

			return q;
		}

		CQueryToken* CQueryParser::matchQueryToken(QueryTokenTypes expectedType,CTokenList* pTokenList)
		{
			if(pTokenList->count() == 0)
			{
				throw CQueryParserException(_T("Error: Unexpected end of program"),0,0);
			}

			//Extract a token form the TokenList pTokenList
			CQueryToken* t = pTokenList->extract();
			//Check if the type of the token t matches the expectedType
			if (expectedType != t->m_type)
			{
				char buf[200];
				_sntprintf(buf,200,_T("Error: Unexpected CQueryToken: %d, expected: %d"),t->m_type,expectedType);
				delete(t);
				throw CQueryParserException(buf,0,0);
			}
			return t;
		}

		void CQueryParser::extractAndDeleteToken(CTokenList* pTokenList)
		{
			CQueryToken* t = pTokenList->extract();
			delete(t);
		}

		void CQueryParser::addClause(vector<CBooleanClause*>* clauses, int32_t conj, int32_t mods, CQuery* q)
		{		
			bool required;
			bool prohibited;			
			if (conj == CONJ_AND) 
			{
				// 2004.11.01: SF 1051984:
				// If attempting to parse a query such as "a AND b", where the first
				// clause is a stopword (and thus, is filtered out), the following code
				// assumed when it reached the second clause that the previous clause
				// had not been filtered out, so it undershot clauses and crashed.
				size_t nPreviousClauses = clauses->size();
				if (nPreviousClauses > 0) 
				{
					CBooleanClause* c = (*clauses)[nPreviousClauses-1];
					if (!c->isProhibited())
						c->setRequired();
				}
			}

			// We might have been passed a NULL query; the term might have been
			// filtered away by the analyzer.
			if (q == NULL)
				return;

			// We set REQUIRED if we're introduced by AND or +; PROHIBITED if
			// introduced by NOT or -; make sure not to set both.
			prohibited = (mods == MOD_NOT);
			required = (mods == MOD_REQ);
			if (conj == CONJ_AND && !prohibited)
				required = true;

			clauses->push_back(new CBooleanClause(q,required, prohibited));
			delete q;
		}

		tstring CQueryParser::discardEscapeChar(const tstring& source)
		{
			int32_t len = (int32_t)source.size();
			char* dest = new char[len+1];

			int32_t j = 0;
			for (int32_t i = 0; i < len; i++) 
			{
				if ((source[i] != '\\') || (i > 0 && source[i-1] == '\\')) {
					dest[j++]=source[i];
				}
			}
			dest[j]=0;
			tstring ret = dest;
			delete[] dest;

			return ret;
		}
		CQuery* CQueryParser::getFieldQuery(const tstring& field, const tstring& queryText)
		{			
			CAnalyzer* pAnalyzer = getAnalyzer(field);
			if(!pAnalyzer)
			{
				pAnalyzer = m_pDefaultAnalyzer;
			}
			CStringReader reader(queryText);

			//pAnalyzer->setMaxTokensToAnalyze(100);
			CTokens* pTokens = NULL;
			pTokens = pAnalyzer->nextTokens(&reader);	

			if (pTokens->getNumTokens() <= 0)
			{			
				return NULL;
			}
			else
			{
				if (pTokens->getNumTokens() == 1)
				{
					pTokens->startIterator();
					CTerm* t = pTokens->next()->toTerm(field.c_str());
					CQuery* ret = new CTermQuery(t);
					delete t;					
					return ret;
				}
				else
				{
					CPhraseQuery* q = new CPhraseQuery;
					q->setSlop(0);

					CToken* pToken;			
					pTokens->startIterator();
					while (pTokens->hasNext())
					{
						pToken = pTokens->next();
						CTerm* t = pToken->toTerm(field.c_str());
						q->addTerm(t);			
						delete t;						
					}					
					return q;
				}
			}
			return NULL;			
		}
		CQuery* CQueryParser::getRangeQuery(const tstring& field,const tstring& queryText, bool inclusive)
		{			
			CAnalyzer* pAnalyzer = getAnalyzer(field);
			CTerm* t1 = NULL,*t2 = NULL;
			if(!pAnalyzer)
			{
				pAnalyzer = m_pDefaultAnalyzer;
			}
			else 
			{
				CStringReader reader;

				pAnalyzer->setMaxTokensToAnalyze(2);
				CTokens* pTokens = NULL;

				tstring tmp(queryText.begin() + 1,queryText.end() - 1);
				std::vector<tstring> terms = firtex::string_utils::split(tmp,_T("TO"));
				if(terms.size() > 2 || terms.size() <= 0)
					FIRTEX_THROW3(QUERY_PARSE_ERROR,_T("range query syntax error."));
				if(terms.size() == 2)
				{
					tstring str = discardEscapeChar(terms[0]);
					reader.open(str.c_str(),str.length());
					pTokens = pAnalyzer->nextTokens(&reader);
					pTokens->startIterator();
					t1 = pTokens->next()->toTerm(field.c_str());
					
					str = discardEscapeChar(terms[1]);
					reader.open(str.c_str(),str.length());
					pTokens = pAnalyzer->nextTokens(&reader);
					pTokens->startIterator();
					t2 = pTokens->next()->toTerm(field.c_str());
				}
				else// if(terms.size() == 2)//[TO upper] or [lower TO] or [lower upper]
				{
					if(!_tcsicmp(tmp.substr(0,2).c_str(),_T("TO")))
					{
						tstring str = discardEscapeChar(terms[0]);
						reader.open(str.c_str(),str.length());
						pTokens = pAnalyzer->nextTokens(&reader);
						pTokens->startIterator();
						t2 = pTokens->next()->toTerm(field.c_str());
					}
					else
					{
						tstring str = discardEscapeChar(terms[0]);
						reader.open(str.c_str(),str.length());
						pTokens = pAnalyzer->nextTokens(&reader);
						pTokens->startIterator();
						t1 = pTokens->next()->toTerm(field.c_str());
					}
				}				
			}

			CQuery* q = NULL;
			if(t1 || t2)
				q = new CRangeQuery(t1,t2,inclusive);
			if(t1)
				delete t1;
			if(t2)
				delete t2;			
			return q;			
		}

		CAnalyzer* CQueryParser::getAnalyzer(const tstring& field)
		{
			analyzer_map::iterator iter = m_analyzers.find(field);
			if(iter!=m_analyzers.end())
			{
				return iter->second;
			}
			return NULL;
		}
		void CQueryParser::attachAnalyzer(const tchar* field,CAnalyzer* pAnalyzer)
		{			
			analyzer_map::iterator iter = m_analyzers.find(tstring(field));
			if(iter != m_analyzers.end())
			{
				delete iter->second;
				m_analyzers.erase(iter);
			}
			m_analyzers.insert(make_pair(tstring(field),pAnalyzer));			
		}		
		CAnalyzer* CQueryParser::detachAnalyzer(const tchar* field)
		{
			CAnalyzer* pAnalyzer = NULL;
			analyzer_map::iterator iter = m_analyzers.find(tstring(field));
			if(iter == m_analyzers.end())
			{
				return NULL;
			}
			else 
			{
				pAnalyzer = iter->second;
				m_analyzers.erase(iter);
			}
			return pAnalyzer;
		}
		CAnalyzer* CQueryParser::getAnalyzer(const tchar* field)
		{
			CAnalyzer* pAnalyzer = NULL;
			analyzer_map::iterator iter = m_analyzers.find(tstring(field));
			if(iter == m_analyzers.end())
			{
				return NULL;
			}
			else 
			{
				pAnalyzer = iter->second;				
			}
			return pAnalyzer;
		}
	}
}

