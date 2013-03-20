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
// Email	: ruijieguo@software.ict.ac.cn
// Created	: 2006/1/4
//
#include "index/NormsReader.h"
#include "utility/StringUtils.h"
#include "index/NormsWriter.h"

namespace firtex
{
	namespace index
	{
		CNormsReader::CNormsReader(CDirectory* pDirectory)
			:m_pDirectory(pDirectory)			
		{
		}

		CNormsReader::~CNormsReader(void)
		{
			close();
			m_pDirectory = NULL;
		}
		void CNormsReader::open(const tchar* barrelName,CFieldsInfo* pFieldsInfo)
		{
			close();
			int32_t nNumFields = pFieldsInfo->numFields();
			CFieldInfo* pFieldInfo;
			CNorms* pNorm;
			for (int32_t i = 0;i<nNumFields;i++)
			{			
				pFieldInfo = (*pFieldsInfo)[i];				
				if(pFieldInfo->isIndexed())
				{
					pNorm = new CNorms();					
					tstring str = barrelName;
					str += _T(".n");
					str = firtex::string_utils::append(str,i);
					CIndexInput* pNormStream = m_pDirectory->openInput(str);
					if(pNorm->read(pNormStream))
					{
						m_normsByID.insert(pair<fieldid_t,CNorms*>(i,pNorm));
						m_normsByName.insert(pair<tstring,CNorms*>(pFieldsInfo->getFieldName(i),pNorm));
					}
					else 
					{
						delete pNorm;
					}
					delete pNormStream;
				}                
			}
		}		
		void CNormsReader::open(CCachedNormBytes** ppCachedNormBytes,int32_t numFields,CFieldsInfo* pFieldsInfo)
		{
			close();

			CNorms* pNorm;
			for(int32_t i = 0;i <numFields;i++)
			{
				if((ppCachedNormBytes[i]) && (ppCachedNormBytes[i]->getLength() > 0))
				{
					pNorm = new CNorms();
					pNorm->m_pNormsBytes = new CNormsBytes();
					pNorm->m_pNormsBytes->setNormBytes(0,(CNormBytes*)ppCachedNormBytes[i]);
					pNorm->m_bOwnNorms = false;
					m_normsByID.insert(make_pair(i,pNorm));
					m_normsByName.insert(pair<tstring,CNorms*>(pFieldsInfo->getFieldName(i),pNorm));
				}
			}
		}

		void CNormsReader::close()
		{
			map<fieldid_t,CNorms*>::iterator iter = m_normsByID.begin();
			while (iter != m_normsByID.end())
			{
				delete iter->second;
				iter++;
			}
			m_normsByID.clear();
			m_normsByName.clear();
		}
		CNormsBytes* CNormsReader::getNorms(const tchar* fieldname)
		{
			map<tstring,CNorms*>::iterator iter = m_normsByName.find(fieldname);
			if(iter !=m_normsByName.end())
			{
				return (iter->second->m_pNormsBytes);
			}
			return NULL;
		}
		CNormsBytes* CNormsReader::getNorms(fieldid_t fid)
		{
			map<fieldid_t,CNorms*>::iterator iter = m_normsByID.find(fid);
			if(iter !=m_normsByID.end())
			{
				return (iter->second->m_pNormsBytes);
			}
			return NULL;
		}
	}
}
