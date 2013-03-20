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
// Email	: ruijieguo@software.ict.ac.cn,ruijieguo@gmail.com
// Created	: 2007/1/9
//

#ifndef _ZIPTRECCOLLECTION_H
#define _ZIPTRECCOLLECTION_H

#include "TrecCollection.h"

namespace firtex
{
	namespace collection
	{
		class CZIPTrecCollection :	public CTrecCollection
		{
		public:		
			class Creator : public CCollection::Creator
			{
			public:
				Creator()
				{
					m_identifier += _T(".zip");
				}
				~Creator(){}
			public:
				CCollection* create(const tchar* path){return new CZIPTrecCollection(path);}

			public:
				static Creator* getInstance()
				{
					static Creator INSTANCE;
					return &INSTANCE;
				}
			};
		public:
			/**
			 * 构造函数
			 * @param dir 文档目录			 
			 */
			CZIPTrecCollection(const tchar* dir);

			/**
			 * constructor
			 * @param dir 文档目录
			 * @param pIndexWriter Index Writer对象
			 */
			CZIPTrecCollection(const tchar* dir,CIndexWriter* pWriter);			
			virtual ~CZIPTrecCollection(void);
		protected:
			/**
			 * enumerate directory and index files in it
			 * @param dir directory path
			 */
			virtual void	indexDocs(const tchar* dir);
		};
	}
}

#endif
