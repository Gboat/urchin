#include <assert.h>
#include "search/TokenList.h"

namespace firtex
{
	namespace search
	{
		CTokenList::CTokenList()
		{		
		}

		CTokenList::~CTokenList()
		{		
			m_tokens.clear();
		}

		void CTokenList::add(CQueryToken* token)
		{
			assert(token!=NULL);
			m_tokens.insert(m_tokens.begin(),token);
		}

		void CTokenList::push(CQueryToken* token)
		{		
			assert(token!=NULL);
			m_tokens.push_back(token);
		}

		CQueryToken* CTokenList::peek() 
		{
			size_t nTokens = m_tokens.size();
			if (nTokens == 0) 
			{
				push(new CQueryToken(EOF_));
				nTokens++;
			}
			return m_tokens[nTokens-1];
		}

		CQueryToken* CTokenList::extract()
		{
			CQueryToken* token = peek();			
			m_tokens.pop_back();
			return token;
		}

		int CTokenList::count()
		{
			return (int)m_tokens.size();
		}
	}
}

