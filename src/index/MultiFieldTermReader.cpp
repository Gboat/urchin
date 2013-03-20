#include "index/MultiFieldTermReader.h"
#include "index/FieldFactory.h"
#include "index/FieldsInfo.h"
#include "document/Field.h"
using namespace firtex::document;

namespace firtex
{
	namespace index
	{
		CMultiFieldTermReader::CMultiFieldTermReader(CDirectory* pDirectory,const tchar* barrelname,CFieldsInfo* pFieldsInfo,CBitVector* pDocFilter,CFactoryFinder* pFactories)
			: CTermReader(pDocFilter)
			, m_pFieldFactoryFinder(pFactories)
			, m_pCurReader(NULL)			
		{			
			open(pDirectory,barrelname,pFieldsInfo);
		}
		CMultiFieldTermReader::CMultiFieldTermReader(CBitVector* pDocFilter)
			: CTermReader(pDocFilter)
			, m_pFieldFactoryFinder(NULL)
			, m_pCurReader(NULL)		
		{
		}

		CMultiFieldTermReader::~CMultiFieldTermReader(void)
		{
			close();			
		}

		void CMultiFieldTermReader::open(CDirectory* pDirectory,const tchar* barrelname,CFieldInfo* pFieldsInfo,CBitVector* pDocFilter)
		{			
			throw CUnsupportedOperationException("CMultiFieldTermReader::open(): unsupported operation.");
		}

		CTermReader* CMultiFieldTermReader::termReader(const tchar* field)
		{
			reader_map::iterator iter = m_fieldsTermReaders.find(field);
			if(iter != m_fieldsTermReaders.end())
			{
				return iter->second->clone();
			}
			return NULL;
		}

		CTermIterator* CMultiFieldTermReader::termIterator(CTerm* pLowerTerm,CTerm* pUpperTerm)
		{
			if(!pLowerTerm && !pUpperTerm)
				FIRTEX_THROW3(ILLEGALARGUMENT_ERROR,_T("At least one term must be non-null."));
			if(pLowerTerm && pUpperTerm && _tcscmp(pLowerTerm->getField(),pUpperTerm->getField()))
				FIRTEX_THROW3(ILLEGALARGUMENT_ERROR,_T("Both terms must be for the same field"));
			map<tstring,CTermReader*>::iterator iter = m_fieldsTermReaders.find(pLowerTerm?pLowerTerm->getField():pUpperTerm->getField());
			if(iter != m_fieldsTermReaders.end())
				return iter->second->termIterator(pLowerTerm,pUpperTerm);
			return NULL;
		}
		
		CTermIterator* CMultiFieldTermReader::termIterator(const tchar* field)
		{
			map<tstring,CTermReader*>::iterator iter = m_fieldsTermReaders.find(field);
			if(iter != m_fieldsTermReaders.end())
				return iter->second->termIterator(field);
			return NULL;
		}
		
		bool CMultiFieldTermReader::seek( CTerm* term)
		{
			tstring field = term->getField();
			map<tstring,CTermReader*>::iterator iter = m_fieldsTermReaders.find(field);
			if(iter != m_fieldsTermReaders.end())
			{
				m_pCurReader = iter->second;
				return m_pCurReader->seek(term);
			}
			return false;
		}
		
		CTermDocFreqs* CMultiFieldTermReader::termDocFreqs()
		{
			return m_pCurReader->termDocFreqs();
		}
		
		CTermPositions* CMultiFieldTermReader::termPositions()
		{
			return m_pCurReader->termPositions();
		}

		
		freq_t CMultiFieldTermReader::docFreq(CTerm* term)
		{
			return m_pCurReader->docFreq(term);
		}

		CTermInfo* CMultiFieldTermReader::termInfo(CTerm* term)
		{
			return m_pCurReader->termInfo(term);
		}

		void CMultiFieldTermReader::close()
		{
			m_pCurReader = NULL;;

			reader_map::iterator iter = m_fieldsTermReaders.begin();
			while (iter != m_fieldsTermReaders.end())
			{
				delete iter->second;
				iter++;
			}
			m_fieldsTermReaders.clear();	
		}

		CTermReader* CMultiFieldTermReader::clone()
		{
			CMultiFieldTermReader* pReader = new CMultiFieldTermReader(getFilter());
			pReader->m_pFieldFactoryFinder = this->m_pFieldFactoryFinder;
			
			reader_map::iterator iter = m_fieldsTermReaders.begin();
			while (iter != m_fieldsTermReaders.end())
			{
				pReader->m_fieldsTermReaders.insert(make_pair(iter->first,iter->second->clone()));
				iter++;
			}
			return pReader;
		}

		void CMultiFieldTermReader::open(CDirectory* pDirectory,const tchar* barrelname,CFieldsInfo* pFieldsInfo)
		{		
			CFieldInfo* pInfo = NULL;
			CTermReader* pTermReader = NULL;
			pFieldsInfo->startIterator();
			while(pFieldsInfo->hasNext())
			{
				pInfo = pFieldsInfo->next();

				if( pInfo->isIndexed() )//Ë÷ÒýÓò
				{
					pTermReader = m_pFieldFactoryFinder->getFactory(pInfo->getID())->createTermReader();
					if(pTermReader)
					{
						pTermReader->open(pDirectory,barrelname,pInfo,getFilter());
						m_fieldsTermReaders.insert(pair<string,CTermReader*>(pFieldsInfo->getFieldName(pInfo->getID()),pTermReader));
					}					
				}				
			}		
		}

		void CMultiFieldTermReader::addTermReader(const tchar* field,CTermReader* pTermReader)
		{
			tstring sf = field;
			m_fieldsTermReaders.insert(make_pair(sf,pTermReader));
		}

	}
}

