//
// Copyright(C) 2005--2006 Institute of Computing Tech, Chinese Academy of Sciences. 
// All rights reserved.
// This file is part of FirteX (www.firtex.org)
//
// Use of the FirteX is subject to the terms of the software license set forth in 
// the LICENSE file included with this software, and also available at
// http://www.firtex.org/license.html
//
// Author	: 郭瑞杰(GuoRuijie)
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
			 * 设定域工厂
			 * @param fid 域编号
			 * @param _identifier 域工厂标识
			 * @return factory created
			 */
			CFieldFactory*	mapFactory(fieldid_t fid,const tchar* _identifier);			

			/**
			 * 根据域编号获取域工厂
			 * @param fid 域编号
			 * @return 域工厂对象，内部对象，外部不能删除，如果不存在则返回NULL
			 */
			CFieldFactory*	getFactory(fieldid_t fid);

			/**
			 * 根据域工厂标识获取域工厂
			 * @param _identifier 域工厂标识
			 * @return 域工厂对象，内部对象，外部不能删除，如果不存在则返回NULL
			 */
			CFieldFactory*	getFactoryByIdentifier(const tchar* _identifier);

			/** 获取域工厂数目 */
			int32_t			numFactories(){return (int32_t)m_factoryById.size();}

			/**
			 * 清除域工厂
			 */
			void			clear();
		private:			
			map<fieldid_t,CFieldFactory*> m_factoryById;	
		};		 
	}
}

#endif
