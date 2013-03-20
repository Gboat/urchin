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
// Created	: 2005/11/20
//
#include "document/Field.h"
#include "utility/StringUtils.h"

namespace firtex
{
	namespace document
	{
		CField::CField(const tchar* name, const char* value, store_t store, index_t index, forwardindex_t forwardIndex,float fBoost)
			: m_fBoost(fBoost)
			, m_id(-1)
			, m_pTokens(NULL)
			, m_bDeleteTokens(false)
			, m_tokenType(CToken::UNKNOWN)
		{
			if (!name)
				throw CIllegalArgumentException(_T("name cannot be null."));
			if (!value)
				throw CIllegalArgumentException(_T("value cannot be null."));
			if (index == Index::NO && store == Store::NO)
				throw CIllegalArgumentException(_T("it doesn't make sense to have a field that is neither indexed nor stored."));
			if (index == Index::NO && forwardIndex != ForwardIndex::NO)
				throw CIllegalArgumentException(_T("cannot store term vector information for a field that is not indexed."));
	
			m_name = name;
			
			textValue((char*)value,firtex::string_utils::fx_strlen(value),false);

			m_store = store;
			m_index = index;
			m_forwardIndex = forwardIndex;
		}	
		CField::CField(const tchar* name, const wchar_t* value, store_t store, index_t index, forwardindex_t forwardIndex,float fBoost)
			: m_fBoost(fBoost)
			, m_id(-1)
			, m_pTokens(NULL)
			, m_bDeleteTokens(false)
			, m_tokenType(CToken::UNKNOWN)
		{
			if (!name)
				throw CIllegalArgumentException(_T("Name cannot be null."));
			if (!value)
				throw CIllegalArgumentException(_T("Value cannot be null."));
			if (index == Index::NO && store == Store::NO)
				throw CIllegalArgumentException(_T("It doesn't make sense to have a field that is neither indexed nor stored."));
			if (index == Index::NO && forwardIndex != ForwardIndex::NO)
				throw CIllegalArgumentException(_T("Cannot store term vector information for a field that is not indexed."));
			
			m_name = name;
			textValue((wchar_t*)value,wcslen(value),false);	

			m_store = store;
			m_index = index;
			m_forwardIndex = forwardIndex;
		}	

		CField::CField(const tchar* name, CReader* reader,bool bDelete,store_t store,index_t index,forwardindex_t forwardIndex,float fBoost)
			: m_fBoost(fBoost)
			, m_id(-1)
			, m_pTokens(NULL)
			, m_bDeleteTokens(false)
			, m_tokenType(CToken::UNKNOWN)
		{
			if (!name)
				throw CIllegalArgumentException(_T("Name cannot be null."));
			if (reader == NULL)
				throw CIllegalArgumentException(_T("Reader cannot be null."));

			m_name = name;
			
			readerValue(reader,bDelete);			
			m_store = store;
			m_index = index;
			m_forwardIndex = forwardIndex;
		}		
		
		CField::CField(const tchar* name,char* text,size_t len,bool bNoCopy,store_t store,index_t index,forwardindex_t forwardIndex,float fBoost)
			: m_fBoost(fBoost)
			, m_id(-1)
			, m_pTokens(NULL)
			, m_bDeleteTokens(false)
			, m_tokenType(CToken::UNKNOWN)
		{
			if (!name)
				throw CIllegalArgumentException(_T("Name cannot be null."));
			if (text == NULL)
				throw CIllegalArgumentException(_T("Reader cannot be null."));

			m_name = name;			
			textValue(text,len,bNoCopy);		
			m_store = store;
			m_index = index;
			m_forwardIndex = forwardIndex;
		}		
		CField::CField(const tchar* name,wchar_t* text,size_t len,bool bNoCopy,store_t store,index_t index,forwardindex_t forwardIndex,float fBoost)
			: m_fBoost(fBoost)
			, m_id(-1)
			, m_pTokens(NULL)
			, m_bDeleteTokens(false)
			, m_tokenType(CToken::UNKNOWN)
		{
			if (!name)
				throw CIllegalArgumentException(_T("Name cannot be null."));
			if (text == NULL || (*text == 0))
				throw CIllegalArgumentException(_T("Reader cannot be null."));

			m_name = name;
			
			textValue(text,len,bNoCopy);
			m_store = store;
			m_index = index;
			m_forwardIndex = forwardIndex;

		}
		CField::CField(const tchar* name, byte* value,size_t length,bool bNoCopy,store_t store,float fBoost)
			: m_fBoost(fBoost)
			, m_id(-1)
			, m_pTokens(NULL)
			, m_bDeleteTokens(false)
			, m_tokenType(CToken::UNKNOWN)
		{
			if (!name)
				throw CIllegalArgumentException(_T("Name cannot be null."));
			if (value == NULL)
				throw CIllegalArgumentException(_T("Value cannot be null."));

			m_name = name;
			
			binaryValue(value,length,bNoCopy);			
			
			m_store = store;
			m_index = Index::NO;
			m_forwardIndex = ForwardIndex::NO;			
		}

		CField::CField(const tchar* name, CTokens* value,bool bDelete,forwardindex_t forwardIndex,float fBoost)
			: m_fBoost(fBoost)
			, m_id(-1)
			, m_pTokens(NULL)
			, m_bDeleteTokens(false)
			, m_tokenType(CToken::UNKNOWN)
		{
			if (!name)
				throw CIllegalArgumentException(_T("Name cannot be null."));
			if (value == NULL)
				throw CIllegalArgumentException(_T("Value cannot be null."));

			m_name = name;
			tokensValue(value,bDelete);

			m_store = Store::NO;
			m_index = Index::ANALYZE;
			m_forwardIndex = forwardIndex;			

		}

		CField::CField(const tchar* name, const numbervar& nvar,store_t store,index_t index,float fBoost)
			: m_fBoost(fBoost)
			, m_id(-1)
			, m_pTokens(NULL)
			, m_bDeleteTokens(false)
			, m_tokenType(CToken::UNKNOWN)
		{
			if (!name)
				throw CIllegalArgumentException(_T("name cannot be null."));
			m_name = name;				
			
			numberValue(nvar);
			
			m_store = store;
			m_index = index;
			m_forwardIndex = ForwardIndex::NO;			

		}

		CField::CField(const CField& clone)
		{	
			m_id = clone.m_id;
			m_name = clone.m_name;			
			m_index = clone.m_index;
			m_store = clone.m_store;
			m_forwardIndex = clone.m_forwardIndex;

			m_data.assign(clone.m_data,true);
			m_fBoost = clone.m_fBoost;			
			
			m_pTokens = clone.m_pTokens;
			m_bDeleteTokens = false;
			m_tokenType = clone.m_tokenType;
		}		

		CField::CField(fieldid_t id,const tchar* name,store_t store,index_t index,forwardindex_t tv,float fBoost)
			: m_id(id)
			, m_name(name)
			, m_fBoost(fBoost)
			, m_pTokens(NULL)
			, m_bDeleteTokens(false)
			, m_tokenType(CToken::UNKNOWN)
		{			
			m_store = store;
			m_index = index;
			m_forwardIndex = tv;						
		}

		CField::~CField()
		{			
			if(m_bDeleteTokens && m_pTokens)
			{
				delete m_pTokens;
				m_pTokens = NULL;
			}
		}			
		
		void CField::fromFlag(fieldflag_t flag)
		{
			field_t  vtype= FIELDTYPE(flag);
			m_data.vt = vtype;

			m_tokenType = TOKENTYPE(flag);

			if( (vtype & CField::NUMBER) == CField::NUMBER)
			{				
				m_data.data.number.nt = m_tokenType;
			}
			
			fieldbits_t bits = FIELDBITS(flag);

			if(bits & BIT_INDEX_MASK)
				m_index = Index::UN_ANALYZE;
			else m_index = Index::NO;
			if(bits & BIT_ANALYZE_MASK)
				m_index = Index::ANALYZE;			

			if(bits & BIT_STORE_MASK)
				m_store = Store::YES;
			else m_store = Store::NO;
			if(bits & BIT_COMPRESS_MASK)
				m_store = Store::COMPRESS;
			

			if(bits & BIT_FORWARDINDEX_SEQUENCE_MASK)
				m_forwardIndex = ForwardIndex::SEQUENCE;
			else if(bits & BIT_FORWARDINDEX_FREQ_MASK)
				m_forwardIndex = ForwardIndex::FREQ;
			else if(bits & BIT_FORWARDINDEX_POSITION_MASK)
				m_forwardIndex = ForwardIndex::POSITION;
			else m_forwardIndex = ForwardIndex::NO;
		}

		fieldflag_t CField::toFlag()
		{			
			fieldbits_t bits = 0;
			if (m_index == Index::ANALYZE) 
			{
				bits |= (BIT_ANALYZE_MASK|BIT_INDEX_MASK);								
			}
			else if (m_index == Index::UN_ANALYZE)
			{
				bits |= BIT_INDEX_MASK;								
			}				

			if (m_store == Store::YES)
			{
				bits |= BIT_STORE_MASK;
			}
			else if (m_store == Store::COMPRESS)
			{
				bits |= BIT_STORE_MASK|BIT_COMPRESS_MASK;	
			}			

			
			if(m_forwardIndex == ForwardIndex::SEQUENCE)
			{
				bits |= (BIT_FORWARDINDEX_SEQUENCE_MASK);
			}
			else if(m_forwardIndex == ForwardIndex::FREQ)
			{
				bits |= (BIT_FORWARDINDEX_FREQ_MASK);
			}	
			else if(m_forwardIndex == ForwardIndex::POSITION)
			{
				bits |= (BIT_FORWARDINDEX_POSITION_MASK);
			}			

			field_t datatype = m_data.vt;
			if(datatype == CField::NUMBER)
				m_tokenType = m_data.data.number.nt;
			return MAKEFIELDFLAG(datatype,m_tokenType,bits);
		}	
		fieldflag_t	CField::bitsToFlag(index_t index,store_t store,forwardindex_t forwardIndex)
		{
			fieldbits_t bits = 0;
			if (index == Index::ANALYZE) 
			{
				bits |= (BIT_ANALYZE_MASK|BIT_INDEX_MASK);								
			}
			else if (index == Index::UN_ANALYZE)
			{
				bits |= BIT_INDEX_MASK;								
			}				

			if (store == Store::YES)
			{
				bits |= BIT_STORE_MASK;
			}
			else if (store == Store::COMPRESS)
			{
				bits |= BIT_STORE_MASK|BIT_COMPRESS_MASK;	
			}			


			if(forwardIndex == ForwardIndex::SEQUENCE)
			{
				bits |= (BIT_FORWARDINDEX_SEQUENCE_MASK);
			}
			else if(forwardIndex == ForwardIndex::FREQ)
			{
				bits |= (BIT_FORWARDINDEX_FREQ_MASK);
			}	
			else if(forwardIndex == ForwardIndex::POSITION)
			{
				bits |= (BIT_FORWARDINDEX_POSITION_MASK);
			}		
			return MAKEFIELDFLAG(0,0,bits);
		}
		void CField::flagToBits(fieldflag_t flag,index_t& index,store_t& store,forwardindex_t& forwardIndex)
		{			
			fieldbits_t bits = FIELDBITS(flag);

			if(bits & BIT_INDEX_MASK)
				index = Index::UN_ANALYZE;
			else index = Index::NO;
			if(bits & BIT_ANALYZE_MASK)
				index = Index::ANALYZE;
			

			if(bits & BIT_STORE_MASK)
				store = Store::YES;
			else store = Store::NO;
			if(bits & BIT_COMPRESS_MASK)
				store = Store::COMPRESS;			

			if(bits & BIT_FORWARDINDEX_SEQUENCE_MASK)
				forwardIndex = ForwardIndex::SEQUENCE;
			else if(bits & BIT_FORWARDINDEX_FREQ_MASK)
				forwardIndex = ForwardIndex::FREQ;
			else if(bits & BIT_FORWARDINDEX_POSITION_MASK)
				forwardIndex = ForwardIndex::POSITION;
			else forwardIndex = ForwardIndex::NO;
		}
	}
}
