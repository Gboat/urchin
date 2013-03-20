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
// Email	: ruijieguo@software.ict.ac.cn
// Created	: 2006/1/4
// Modified : 2006/10/3 change norms from byte to int32_t,no precision lost.
//

#ifndef _NORMSWRITER_H
#define _NORMSWRITER_H

#include "../utility/StdHeader.h"
#include "../document/Field.h"
#include "../store/IndexOutput.h"
#include "../store/Directory.h"
#include "../search/Similarity.h"
#include "NormBytes.h"
#include <sstream>

using namespace std;


namespace firtex
{
	namespace index
	{
#define NORMS_CACHE_FIELD	2	
#define CACHED_NORMBYTES	32768	///32k
		
		class CCachedNormBytes : public CNormBytes
		{
		public:
			CCachedNormBytes(size_t nNumCachedNormBytes)
				:m_nMaxLength(nNumCachedNormBytes)
			{					
				m_bytes = new int32_t[m_nMaxLength];
			}
			~CCachedNormBytes()
			{					
			}
		public:
			void	growCache()
			{
				int32_t* ppBytes = new int32_t[m_nMaxLength*2];					
				memcpy(ppBytes,m_bytes,m_nMaxLength*sizeof(int32_t));
				memset(ppBytes+m_nMaxLength,0,m_nMaxLength*sizeof(int32_t));
				delete[] m_bytes;
				m_bytes = ppBytes;
				m_nMaxLength = 2*m_nMaxLength ;
			}
			bool	isCacheFull(){return (m_length >= m_nMaxLength);}
		protected:			
			size_t	m_nMaxLength;	

			friend class CNormsWriter;
		};

		class CNormsReader;
		class CFieldsInfo;
		class CNormsWriter
		{					
		public:
			CNormsWriter(firtex::store::CDirectory* pDirectory);
			CNormsWriter(firtex::store::CDirectory* pDirectory,const tchar* barrelName);
			~CNormsWriter(void);
		public:
			/** 
			 * 打开新索引桶
			 * @param barrelName			 
			 */
			void			open(const tchar* barrelName);			

			/**
			 * 添加域
			 * @param docID 文档编号
			 * @param pField 域对象
			 */
			inline void		addField(docid_t docID,firtex::document::CField* pField);

			/** 
			 * 获取Norms Reader，一般在On-line Indexing时使用
			 * @param pFieldsInfo field info 
			 * @return 返回Norms Reader，内部对象，外部不能删除
			 */
			CNormsReader*	normsReader(CFieldsInfo* pFieldsInfo);

			/** 关闭norm文件 */
			void			close();
		protected:
			firtex::store::CDirectory*	m_pDirectory;
			tstring						m_barrelName;	

			CCachedNormBytes**			m_ppCachedNormBytes;
			int32_t						m_nFieldNum;
		};
		//////////////////////////////////////////////////////////////////////////
		//inline Functions
		inline void CNormsWriter::addField(docid_t docID,firtex::document::CField* pField)
		{
			fieldid_t fid = pField->getID();
			if(fid >= m_nFieldNum)
			{
				CCachedNormBytes** ppCachedNormBytes = new CCachedNormBytes*[fid + 1];
				memset(ppCachedNormBytes,0,(fid + 1)*sizeof(CCachedNormBytes*));
				if(m_ppCachedNormBytes)
				{
					memcpy(ppCachedNormBytes,m_ppCachedNormBytes,m_nFieldNum*sizeof(CCachedNormBytes*));
					delete[] m_ppCachedNormBytes;
				}
				m_nFieldNum = fid + 1;				
				m_ppCachedNormBytes = ppCachedNormBytes;
			}
			CCachedNormBytes* pCachedNorms = m_ppCachedNormBytes[fid];
			if(pCachedNorms == NULL)
			{				
				m_ppCachedNormBytes[fid] = pCachedNorms = new CCachedNormBytes(CACHED_NORMBYTES);
			}
			int32_t norm = firtex::search::CSimilarity::getDefault()->encodeNorm(pField->getBoost(),pField->tokensValue()?(int32_t)pField->tokensValue()->getNumTokens():1);
			int32_t nSkip = (int32_t)docID - (int32_t)pCachedNorms->getLength();
			if(nSkip < 0)
				FIRTEX_THROW3(ILLEGALARGUMENT_ERROR,_T("CNormsWriter::addField():docID is out of order."));
			while (nSkip > 0)
			{				
				pCachedNorms->m_bytes[pCachedNorms->m_length++] = 0;
				nSkip--;
			}
			//FIRTEX_ASSERT(( ((size_t)docID*sizeof(int32_t)) >= m_ppCachedNormBytes[fid]->getLength() ),_T("CNormsWriter::addField():docID is out of order."))			
			
			if(pCachedNorms->isCacheFull())
				pCachedNorms->growCache();
			pCachedNorms->m_bytes[pCachedNorms->m_length++] = norm;
		}		
	}
}


#endif
