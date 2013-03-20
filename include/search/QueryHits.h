//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: �����(GuoRuijie)
// Email	: ruijieguo@software.ict.ac.cn,ruijieguo@gmail.com
// Created	: 2005/12/27
//
#ifndef _HITS_H
#define _HITS_H

#include "../utility/typedefs.h"
#include "../document/Document.h"
#include "HitCollector.h"

#include <vector>
using namespace std;

using namespace firtex;

namespace firtex
{
	namespace search
	{
		class CSearcher;
		class CQuery;

		class CHitDoc;		
		class CQueryHits
		{
		public:
			class CHitDoc
			{
			public:				
				CHitDoc(docid_t i,score_t s,CQueryHits* pQH)
					:m_pQH(pQH)
					,m_score(s)
					,m_docID(i)
					,m_doc(NULL)
				{			
				}
			public:
				/**
				* ������{@link hasNext()}����true����ã���ȡ�����ӵĵ�ǰ�ĵ����
				*/
				docid_t		docid(){return m_docID;}

				/**
				* ������{@link hasNext()}����true����ã���ȡ�����ӵĵ�ǰ�ĵ���ֽ��
				*/
				score_t		score(){return m_score;}
				document::CDocument* doc()
				{
					if(!m_doc)
						m_doc = m_pQH->getDoc(this);
					return m_doc;
				}
			protected:
				score_t					m_score;
				docid_t					m_docID;
				document::CDocument*	m_doc;

				CQueryHits*				m_pQH;

				friend class CQueryHits;
			};

		public:
			CQueryHits(void);
			CQueryHits(CHitQueue* hq,CQuery* pQuery,CSearcher* pSearcher);
			virtual ~CQueryHits(void);			
		public:
			/**
			* ��ȡHits�е�n���ĵ�
			* @param n Hits�е�n���ĵ�(0<=n<length())
			*/
			document::CDocument*	doc(int32_t n);

			/**
			* ��ȡHits�е�n���ĵ��Ĵ�ֽ��
			* @param n Hits�е�n���ĵ�(0<=n<length())
			*/
			score_t score(int32_t n){return hitDoc(n)->m_score;}			

			/**
			* ��ȡHits�е�n���ĵ����ڲ��ĵ����
			* @param n Hits�е�n���ĵ�(0<=n<length())
			*/
			docid_t	docid(int32_t n){return hitDoc(n)->m_docID;}				

			/**
			* ��ȡ��ѯ��������			 
			*/
			count_t	getHitNum(){return m_totalHits;}		

			/**
			* ��ȡQueryHits�а������ĵ�����			 
			*/
			int32_t length()
			{
				return (int32_t)m_nNumHitdDocs;
			}			
		public:
			/**
			* ��ʼ���е���ʹ��ʾ�����£�
			* startIterator();
			* while(hasNext())
			* {
			*		//nextHitDoc();	
			* }
			*/
			void	startIterator(){m_nIterator = 0;}

			/**
			* �ж��Ƿ����
			*/
			bool	hasNext(){return (m_nIterator < m_nNumHitdDocs);}

			/**
			* ������{@link hasNext()}����true����ã���ȡ�����ӵĵ�ǰ�����ĵ�
			* @param �����ĵ����󣬱������ⲿɾ��
			*/
			CHitDoc*	nextHitDoc()
			{
				return m_ppHitDocs[m_nIterator++];
			};			
		protected:
			void		getTopDocs(int32_t topk);
			void		clear();
			CQueryHits::CHitDoc*	hitDoc(int32_t n);		
			document::CDocument*	docByID(docid_t did);
			document::CDocument*	getDoc(CHitDoc* pHitDoc);
		protected:
			CHitDoc**	m_ppHitDocs;
			int32_t		m_nNumHitdDocs;
			int32_t		m_nIterator;			

			CQuery*		m_pQuery;
			CSearcher*	m_pSearcher;
			count_t		m_totalHits;			

			friend class CIndexSearcher;
		};		
	}
}

#endif
