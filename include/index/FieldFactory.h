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
// Email	: ruijieguo@software.ict.ac.cn ruijieguo@gmail.com
// Created	: 2006/6/10
//
#ifndef _FIELDFACTORY_H
#define _FIELDFACTORY_H

#include "../utility/StdHeader.h"
#include "DictHashIndexer.h"
#include "FieldMerger.h"
#include "TermReader.h"
#include "TermSequenceVector.h"
#include "TermFreqVector.h"
#include "TermPositionVector.h"
#include "../document/Field.h"
#include <map>

using namespace std;
using namespace firtex::document;

namespace firtex
{
	namespace index
	{
		typedef map<tstring,CFieldFactory*> factory_map;
		class CFieldFactory
		{		
		protected:
			CFieldFactory(void);
		public:
			~CFieldFactory(void);

		public:
			
			/**
			 * register a field factory
			 * @param pFactory factory
			 */
			static void				registerFactory(CFieldFactory* pFactory);

			/**
			 * get a registered factory
			 * @param _identifier factory identifier
			 */
			static CFieldFactory*	getFactory(const tchar* _identifier);

			/**
			 * get factory map
			 */
			static factory_map&		getFactoryMap();
		public:
			/**
			 * get identifier  
			 */
			virtual const tchar*			getIdentifier() { return m_identifier.c_str();}
		public:
			virtual CFieldIndexer*			createIndexer(CMemCache* pMemCache) = 0;
			virtual CFieldMerger*			createMerger() = 0;
			virtual CTermReader*			createTermReader() = 0;

			virtual CTermSequenceVector*	createTermSequenceVector() = 0;
			virtual CTermFreqVector*		createTermFreqVector() = 0;
			virtual CTermPositionVector*	createTermPositionVector() = 0;

		protected:
			tstring					m_identifier;
		};

		class CFactoryFinder
		{
		public:
			CFactoryFinder();
			~CFactoryFinder();		
		public:
			/**
			 * �趨�򹤳�
			 * @param fid ����
			 * @param _identifier �򹤳���ʶ
			 * @return factory created
			 */
			CFieldFactory*	mapFactory(fieldid_t fid,const tchar* _identifier);			

			/**
			 * �������Ż�ȡ�򹤳�
			 * @param fid ����
			 * @return �򹤳������ڲ������ⲿ����ɾ��������������򷵻�NULL
			 */
			CFieldFactory*	getFactory(fieldid_t fid);

			/**
			 * �����򹤳���ʶ��ȡ�򹤳�
			 * @param _identifier �򹤳���ʶ
			 * @return �򹤳������ڲ������ⲿ����ɾ��������������򷵻�NULL
			 */
			CFieldFactory*	getFactoryByIdentifier(const tchar* _identifier);

			/** ��ȡ�򹤳���Ŀ */
			int32_t			numFactories(){return (int32_t)m_factoryById.size();}

			/**
			 * ����򹤳�
			 */
			void			clear();
		private:			
			map<fieldid_t,CFieldFactory*> m_factoryById;	
		};		 
	}
}

#endif
