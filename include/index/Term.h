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
// Created	: 2005/12/20
//
#ifndef _TERM_H
#define _TERM_H

#include "../utility/StdHeader.h"
#include "../utility/FXString.h"
using namespace firtex::utility;

namespace firtex
{
	namespace index
	{
		class CTerm
		{
		public:
			CTerm(const tchar* field);
			CTerm(const tstring& field);
			CTerm(const tchar* field,const tchar* value);
			CTerm(const tstring& field,const tstring& value);
			CTerm(const CTerm& term);
			CTerm();
			virtual ~CTerm(void);
		public:						
			const tchar*		getField()const{return m_field.c_str();}
			void				setField(const tchar* field){m_field = field;}

			inline const tchar*	stringValue()const;
			inline void			stringValue(const tchar* value);
			
			virtual CTerm*		asTermX()const;
			virtual int32_t		compare(const CTerm* pOther);
			virtual CTerm*		clone();

			virtual void		copy(const CTerm* pSrc);
			virtual void		copyValue(const CTerm* pSrc);
		protected:
			CFXString	m_field; //ÓòÃû			
			CFXString*	m_strValue;			
		};

		template<typename TermT>
		class CTermX : public CTerm
		{
		public:
			CTermX(){}			
			CTermX(const tchar* field,const TermT t ) : CTerm(field),m_value(t)
			{
			}
			virtual ~CTermX(){}
			CTermX(const CTermX<TermT>& clone)
				: CTerm(clone)
				, m_value(clone.m_value)
			{

			}
		public:
			TermT		getValue()const{return m_value;}
			TermT		getValue(){return m_value;}
			void		setValue(const TermT t){m_value = t;}

			void		copy(const CTerm* pSrc)
			{
				m_field = pSrc->getField();
				if(pSrc->stringValue())
					m_strValue = new CFXString(pSrc->stringValue());
				else m_strValue = NULL;	
				m_value = ((CTermX<TermT>*)pSrc->asTermX())->m_value;
			}
			void		copyValue(const CTerm* pSrc)
			{
				if(pSrc->stringValue())
					m_strValue = new CFXString(pSrc->stringValue());
				else m_strValue = NULL;		
				m_value = ((CTermX<TermT>*)pSrc->asTermX())->m_value;
			}

			CTerm*		asTermX()const{return (CTerm*)this;}
			CTerm*		clone(){return new CTermX<TermT>(*this);}
			int32_t		compare(const CTerm* pOther)
			{
				int32_t ret = CTerm::compare(pOther);
				if(ret != 0)
					return ret;
				CTermX<TermT>* pX = (CTermX<TermT>*)pOther->asTermX();
				return (int32_t)(m_value - pX->m_value);
			}
		private:
			TermT	m_value;
		};

		class CTermArray
		{
		public:
			CTermArray(bool bDeleteTerms)
				: m_nMaxTerms(5)
				, m_nNumTerms(0)
				, m_bDeleteTerms(bDeleteTerms)
			{
				m_ppTerms = new CTerm*[m_nMaxTerms];
				memset(m_ppTerms,0,m_nMaxTerms*sizeof(CTerm*));
			}
			CTermArray(size_t nMaxTerms,bool bDeleteTerms)
				: m_nMaxTerms(nMaxTerms)
				, m_nNumTerms(0)
				, m_bDeleteTerms(bDeleteTerms)
			{
				m_ppTerms = new CTerm*[m_nMaxTerms];
				memset(m_ppTerms,0,m_nMaxTerms*sizeof(CTerm*));
			}
			~CTermArray()
			{
				if(m_bDeleteTerms)
				{
					for (size_t i = 0;i<m_nNumTerms;i++)
					{
						delete m_ppTerms[i];
					}
				}
				delete[] m_ppTerms;
				m_ppTerms = NULL;
			}
		public:
			void	add(CTerm* pWeightedTerm)
			{
				if(m_nNumTerms >= m_nMaxTerms)
					grow();
				m_ppTerms[m_nNumTerms++] = pWeightedTerm;
			}

			void	merge(CTermArray* pArray2) 
			{
				for (size_t i = 0;i<pArray2->getNumTerms();i++)
				{
					add(pArray2->getTerm(i));
				}
			}
			CTerm*	getTerm(size_t _off)
			{
				FIRTEX_ASSERT2(_off < m_nNumTerms);
				return m_ppTerms[_off];
			}

			size_t	getNumTerms(){return m_nNumTerms;}

			bool	isDeleteTerms(){return m_bDeleteTerms;}
		protected:
			void	grow()
			{
				m_nMaxTerms += 5;
				CTerm** pWT = new CTerm*[m_nMaxTerms];
				memset(pWT,0,m_nMaxTerms*sizeof(CTerm*));
				memcpy(pWT,m_ppTerms,m_nNumTerms);
			}
		protected:
			CTerm**		m_ppTerms;
			size_t		m_nNumTerms;
			size_t		m_nMaxTerms;
			bool		m_bDeleteTerms;
		};
		//////////////////////////////////////////////////////////////////////////
		///
		inline const tchar*	CTerm::stringValue()const
		{
			return ( (m_strValue==NULL)?NULL:m_strValue->c_str());
		}
		inline void CTerm::stringValue(const tchar* value)
		{
			if(m_strValue == NULL)
				m_strValue = new CFXString(value);
			else *m_strValue = value;
		}
	}
}



#endif
