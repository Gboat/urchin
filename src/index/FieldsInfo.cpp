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
// Created	: 2006/2/17
//
#include "index/FieldsInfo.h"
#include "utility/SmallFloat.h"
#include "utility/Config.h"
using namespace firtex::utility;

namespace firtex
{
	namespace index
	{
		CFieldsInfo::CFieldsInfo()
			:m_ppFieldsInfo(NULL)
			,m_nNumFieldInfo(0)
			,m_fdInfosIterator(0)
		{
		}
		CFieldsInfo::CFieldsInfo(const CFieldsInfo& src)
			:m_fdInfosIterator(0)
		{
			m_nNumFieldInfo = src.m_nNumFieldInfo;
			m_ppFieldsInfo = new CFieldInfo*[m_nNumFieldInfo];
			for (int32_t i = 0;i<m_nNumFieldInfo;i++)
			{
				m_ppFieldsInfo[i] = new CFieldInfo(*(src.m_ppFieldsInfo[i]));
				m_fdInfosByName.insert(make_pair(m_ppFieldsInfo[i]->m_name.c_str(),m_ppFieldsInfo[i]));
			}					
		}
	
		CFieldsInfo::~CFieldsInfo()
		{
			clear();
		}
	
		void CFieldsInfo::setSchema(CDocumentSchema* pSchema)
		{
			clear();
			CSchemaItem* pItem;			
			m_nNumFieldInfo = 0;
			pSchema->startIterator();
			while (pSchema->hasNext())
			{
				pItem = pSchema->next();
				if(pItem->getId() != -1)
				{					
					m_nNumFieldInfo++;
				}				
			}
			
			m_ppFieldsInfo = new CFieldInfo*[m_nNumFieldInfo];
			memset(m_ppFieldsInfo,0,m_nNumFieldInfo*sizeof(CFieldInfo*));

			int32_t n = 0;
			pSchema->startIterator();
			while (pSchema->hasNext())
			{
				pItem = pSchema->next();
				if(pItem->getId() != -1)
				{
					if(pItem->getAlias() && _tcscmp(pItem->getAlias(),_T("")))
						m_ppFieldsInfo[n] = new CFieldInfo(pItem->getId(),pItem->getAlias(),CField::bitsToFlag(pItem->getIndex(),pItem->getStore(),pItem->getForwardIndex()));
					else 
						m_ppFieldsInfo[n] = new CFieldInfo(pItem->getId(),pItem->getName(),CField::bitsToFlag(pItem->getIndex(),pItem->getStore(),pItem->getForwardIndex()));					
					m_ppFieldsInfo[n]->m_eIndexLevel = pItem->getIndexLevel();
					m_ppFieldsInfo[n]->m_fBoost = pItem->getBoost();

					m_fdInfosByName.insert(make_pair(m_ppFieldsInfo[n]->getName(),m_ppFieldsInfo[n]));
					n++;
				}				
			}
		}
		
		void CFieldsInfo::updateField(CField* pField)
		{
			FIRTEX_ASSERT((pField->getID()>=0 && pField->getID()<m_nNumFieldInfo),_T("CFieldsInfo::updateField():Field ID is out of range."));
			CFieldInfo* pFieldInfo = m_ppFieldsInfo[pField->getID()];

			if(pFieldInfo->m_totalTerms == 0)
				pFieldInfo->setFlag(pField->toFlag());
			
            if(pField->isAnalyzed())
				pFieldInfo->m_totalTerms += pField->tokensValue()->getNumTokens();
					
		}
		void CFieldsInfo::addField(CFieldInfo* pFieldInfo)
		{
			CFieldInfo* pInfo = getField(pFieldInfo->getName());
			if(pInfo)
			{
				pInfo->m_totalTerms += pFieldInfo->m_totalTerms;
			}
			else 
			{
				pInfo = new CFieldInfo(*pFieldInfo);
				CFieldInfo** ppFieldInfos = new CFieldInfo*[m_nNumFieldInfo+1];				
				for (int32_t i = 0;i<m_nNumFieldInfo;i++)
				{
					ppFieldInfos[i] = m_ppFieldsInfo[i];
				}
				ppFieldInfos[m_nNumFieldInfo] = pInfo;

				delete[] m_ppFieldsInfo;
				m_ppFieldsInfo = ppFieldInfos;
				m_nNumFieldInfo++;

				m_fdInfosByName.insert(pair<tstring,CFieldInfo*>(pInfo->getName(),pInfo));
			}			
		}
	
		void CFieldsInfo::read(CIndexInput* pIndexInput)
		{			
			try
			{				
				//Çå³ý¾ÉµÄ×Ö¶ÎÐÅÏ¢
				clear();
				
				int32_t count = pIndexInput->readInt(); ///<FieldsCount(Int32)>
				if(count <= 0)
				{
					FIRTEX_CLOG(level::warn) << _T("CFieldsInfo::read():field count <=0.") << FIRTEX_ENDL;
					return ;
				}
				m_nNumFieldInfo = count;
				m_ppFieldsInfo = new CFieldInfo*[count];
				memset(m_ppFieldsInfo,0,count*sizeof(CFieldInfo*));

				tstring str;
				CFieldInfo* pInfo = NULL;				
				for(int32_t i = 0;i<count;i++)
				{
					pInfo = new CFieldInfo();					
					pInfo->setID(i);
					pIndexInput->readString(str);	///<FieldName(String)>
					pInfo->setName(str.c_str());
					pInfo->setFlag((fieldflag_t)pIndexInput->readInt()); ///<FieldFlag(Int32)>

					if( (pInfo->getFlag() & BIT_INDEX_MASK) == BIT_INDEX_MASK )//Ë÷Òý×Ö¶Î
					{
						///< Boost(Float)>
						pInfo->setBoost(CSmallFloat::byte315ToFloat(pIndexInput->readByte()));

						pInfo->setNumTerms(pIndexInput->readLong());
						pInfo->setDistinctNumTerms(pIndexInput->readLong());
						pInfo->setIndexOffset(pIndexInput->readLong());
						pInfo->m_tdiLength = pIndexInput->readLong();
						pInfo->m_dfiLength = pIndexInput->readLong();
						pInfo->m_ptiLength = pIndexInput->readLong();						
					}				

					m_ppFieldsInfo[i] = pInfo;
					m_fdInfosByName.insert(pair<tstring,CFieldInfo*>(pInfo->getName(),pInfo));					
				}

			}
			catch(const CFileIOException& e)
			{
				FIRTEX_RETHROW(e);
			}
			catch (const bad_alloc& )
			{
				FIRTEX_THROW2(OUTOFMEM_ERROR,"CFieldsInfo.read():alloc memory failed.");
			}
			catch(const CFirteXException& fe)
			{
				FIRTEX_RETHROW(fe);
			}
			catch (...) 
			{
				FIRTEX_THROW2(UNKNOWN_ERROR,"CFieldsInfo::read failed.");
			}			
		}
		
		void CFieldsInfo::write(CIndexOutput* pIndexOutput)
		{
			try
			{								
				pIndexOutput->writeInt(m_nNumFieldInfo);///<FieldsCount(Int32)>
				CFieldInfo* pInfo;
				for (int32_t i = 0;i<m_nNumFieldInfo;i++)
				{				
					pInfo = m_ppFieldsInfo[i];
					pIndexOutput->writeString(pInfo->getName());	///<FieldName(String)>
					pIndexOutput->writeInt(pInfo->getFlag());		///<FieldFlag(Int32)>
					
					if( (pInfo->getFlag() & BIT_INDEX_MASK) == BIT_INDEX_MASK )//Ë÷Òý×Ö¶Î
					{						
						pIndexOutput->writeByte(CSmallFloat::floatToByte315(pInfo->getBoost()));///<Boost(Float)>

						pIndexOutput->writeLong(pInfo->numTerms());
						pIndexOutput->writeLong(pInfo->distinctNumTerms());
						pIndexOutput->writeLong(pInfo->getIndexOffset());
						pIndexOutput->writeLong(pInfo->m_tdiLength);
						pIndexOutput->writeLong(pInfo->m_dfiLength);
						pIndexOutput->writeLong(pInfo->m_ptiLength);						
					}			
					
				}
			}
			catch(const CFileIOException& e)
			{
				FIRTEX_RETHROW(e);
			}
			catch (const bad_alloc& )
			{
				FIRTEX_THROW2(OUTOFMEM_ERROR,"CFieldsInfo.write():alloc memory failed.");
			}
			catch(const CFirteXException& fe)
			{
				FIRTEX_RETHROW(fe);
			}
			catch (...) 
			{
				FIRTEX_THROW2(UNKNOWN_ERROR,"CFieldsInfo.write() failed.");
			}			
		}
		void CFieldsInfo::clear()
		{
			for (int32_t i = 0;i<m_nNumFieldInfo;i++)
			{
				delete m_ppFieldsInfo[i];
				m_ppFieldsInfo[i] = NULL;
			}
			if(m_ppFieldsInfo)
			{
				delete[] m_ppFieldsInfo;
				m_ppFieldsInfo = NULL;
			}
			m_nNumFieldInfo = 0;
			
			m_fdInfosByName.clear();
		}

		void CFieldsInfo::reset()
		{
			for (int32_t i = 0;i<m_nNumFieldInfo;i++)
			{
				m_ppFieldsInfo[i]->reset();				
			}			
		}

		void CFieldsInfo::setFieldOffset(fieldid_t fid,fileoffset_t offset)
		{
			FIRTEX_ASSERT((fid>=0 && fid<m_nNumFieldInfo),_T("Field ID is out of range."));
			m_ppFieldsInfo[fid]->setIndexOffset(offset);
		}
		fileoffset_t CFieldsInfo::getFieldOffset(fieldid_t fid)
		{
			FIRTEX_ASSERT((fid>=0 && fid<m_nNumFieldInfo),_T("Field ID is out of range."));
			return m_ppFieldsInfo[fid]->getIndexOffset();			
		}

		void CFieldsInfo::setDistinctNumTerms(fieldid_t fid,uint64_t distterms)
		{
			FIRTEX_ASSERT((fid>=0 && fid<m_nNumFieldInfo),_T("Field ID is out of range."));
			m_ppFieldsInfo[fid]->setDistinctNumTerms(distterms);			
		}
		
		uint64_t CFieldsInfo::distinctNumTerms(fieldid_t fid)
		{
			FIRTEX_ASSERT((fid>=0 && fid<m_nNumFieldInfo),_T("Field ID is out of range."));
			return m_ppFieldsInfo[fid]->distinctNumTerms();
		}
		uint64_t CFieldsInfo::numTerms(fieldid_t fid)
		{
			FIRTEX_ASSERT((fid>=0 && fid<m_nNumFieldInfo),_T("Field ID is out of range."));
			return m_ppFieldsInfo[fid]->numTerms();			
		}

		fieldid_t CFieldsInfo::getFieldID(const tchar* fname)
		{
			map<tstring,CFieldInfo*>::iterator iter = m_fdInfosByName.find(fname);
			if (iter != m_fdInfosByName.end())
			{
				return iter->second->getID();
			}
			return -1;
		}		
		CFieldInfo* CFieldsInfo::getField(const tchar* field)
		{
			tstring tmp(field);
			map<tstring,CFieldInfo*>::iterator iter = m_fdInfosByName.find(tmp);
			if (iter != m_fdInfosByName.end())
			{
				return iter->second;
			}
			return NULL;
		}		
	}
}
