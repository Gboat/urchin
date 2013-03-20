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
// Created	: 2005/12/04
//

#ifndef __FIELDSINFO_H
#define __FIELDSINFO_H

#include "../utility/StdHeader.h"
#include <string>
#include "../document/Field.h"
#include "../store/IndexInput.h"
#include "../store/IndexOutput.h"
#include "../document/DocumentSchema.h"
#include "../index/Posting.h"

#include <map>
using namespace std;
using namespace firtex::document;
using namespace firtex::store;


namespace firtex
{
	namespace index
	{	
		class CFieldInfo
		{
		public:

			CFieldInfo(void)
				:m_flag(0)
				,m_id(-1)
				,m_fBoost(1.0f)				
				,m_eIndexLevel(WORD_LEVEL)
			{				
				reset();
			}
			CFieldInfo(fieldid_t fid,const tchar* name,fieldflag_t flag)
				:m_id(fid)
				,m_name(name)
				,m_flag(flag)
				,m_fBoost(1.0f)				
				,m_eIndexLevel(WORD_LEVEL)
			{				
				reset();
			}
			CFieldInfo(const CFieldInfo& src)
			{
				m_flag = src.m_flag;
				m_id = src.m_id;
				m_indexOffset = src.m_indexOffset;
				m_name = src.m_name;

				m_eIndexLevel = src.m_eIndexLevel;			

				m_fBoost = src.m_fBoost;

				m_totalTerms = src.m_totalTerms;
				m_distTerms = src.m_distTerms;
				m_indexOffset = src.m_indexOffset;

				m_tdiLength = src.m_tdiLength;
				m_dfiLength = src.m_dfiLength;
				m_ptiLength = src.m_ptiLength;
			}
			~CFieldInfo(void)
			{			
			}
		public:
			/** 获取域编号 */
			fieldid_t		getID(){return m_id;}

			/** 设置域编号 */
			void			setID(fieldid_t fid){ m_id = fid;}

			/** 获取/设置域名称 */
			const tchar*	getName(){return m_name.c_str();}
			void			setName(const tchar* name){m_name = name;}		
			

			/** 获取/设置索引级别 */
			INDEX_LEVEL	getIndexLevel(){return m_eIndexLevel;}
			void		setIndexLevel(INDEX_LEVEL eLevel){m_eIndexLevel = eLevel;}

			/** 获取/设置域Boost */
			float	getBoost(){return m_fBoost;}
			void	setBoost(float fBoost){m_fBoost = fBoost;}					
			

			/** 获取设置索引数据偏移量 */
			fileoffset_t	getIndexOffset(){return m_indexOffset;}
			void			setIndexOffset(fileoffset_t offset){m_indexOffset = offset;}

			/** 获取/设置Term总数 */
			uint64_t		numTerms(){return m_totalTerms;}
			void			setNumTerms(uint64_t n){m_totalTerms = n;}

			/** 获取/设置非重复Term总数 */
			uint64_t		distinctNumTerms(){return m_distTerms;}			
			void			setDistinctNumTerms(uint64_t n){m_distTerms = n;}

			/** 设置/获取 */
			void			setLength(int64_t nVocLen,int64_t dfiLen,int64_t ptiLen){m_tdiLength = nVocLen;m_dfiLength=dfiLen;m_ptiLength=ptiLen;}
			void			getLength(int64_t* pNVocLen,int64_t* pNDLen,int64_t* pNPLen);


			fieldflag_t		getFlag(){return m_flag;}
			void			setFlag(fieldflag_t flag){m_flag = flag;}

			field_t		getType(){return FIELDTYPE(m_flag);}

			token_t			getTokenType(){return TOKENTYPE(m_flag);}

			bool			isIndexed(){return ((m_flag & BIT_INDEX_MASK) == BIT_INDEX_MASK);}
			bool			isAnalyzed(){return ((m_flag & BIT_ANALYZE_MASK)==BIT_ANALYZE_MASK);}

			bool			isStored(){return ((m_flag & BIT_STORE_MASK) == BIT_STORE_MASK);}
			bool			isCompressed(){return ((m_flag & BIT_COMPRESS_MASK)==BIT_COMPRESS_MASK);}
			
			bool			isStoreForwardIndex(){return (isSequenceVector() || isFreqVector() || isPositionVector());}
			bool			isSequenceVector(){return ((m_flag & BIT_FORWARDINDEX_SEQUENCE_MASK) == BIT_FORWARDINDEX_SEQUENCE_MASK);}
			bool			isFreqVector(){return ((m_flag & BIT_FORWARDINDEX_FREQ_MASK)==BIT_FORWARDINDEX_FREQ_MASK);}
			bool			isPositionVector(){return ((m_flag & BIT_FORWARDINDEX_POSITION_MASK)==BIT_FORWARDINDEX_POSITION_MASK);}

			void			reset(){m_totalTerms = m_distTerms = 0;m_indexOffset = -1;m_tdiLength = m_dfiLength = m_ptiLength = 0;}
		protected:
			fieldid_t		m_id;			///域编号
			CFXString		m_name;			///域名
			fieldflag_t		m_flag;			///域标志
			
			/** 以下9项只有当该域是索引域时有效 */
			INDEX_LEVEL		m_eIndexLevel;	///索引级别
			float			m_fBoost;		///重要度提升值
			
			uint64_t		m_totalTerms;	///总共出现词数
			uint64_t		m_distTerms;	///非重复词数
			fileoffset_t	m_indexOffset;	///该域的索引数据在.voc文件中的偏移		

			int64_t			m_tdiLength;	///该域的词典数据(.voc)的长度
			int64_t			m_dfiLength;	///该域的文档和频率数据(.dfp)的长度
			int64_t			m_ptiLength;	///该域的位置数据(.pop)的长度

			friend class CFieldsInfo;
		};
		//////////////////////////////////////////////////////////////////////////
		//
		inline void CFieldInfo::getLength(int64_t* pNVocLen,int64_t* pNDLen,int64_t* pNPLen)
		{
			if(pNVocLen)
				*pNVocLen = m_tdiLength;
			if(pNDLen)
				*pNDLen = m_dfiLength;
			if(pNPLen)
				*pNPLen = m_ptiLength;
		}

		
		class CFieldsInfo
		{
		public:
			CFieldsInfo();
			CFieldsInfo(const CFieldsInfo& src);
			virtual ~CFieldsInfo();
		public:

			/**
			 * 设置索引文档模式
			 * @param pSchema 索引文档模式
			 */
			void			setSchema(CDocumentSchema* pSchema);

			/**
			 *	更新域信息
			 */
			void			updateField(CField* pField);

			/**
			 *	添加域
			 */
			void			addField(CFieldInfo* pFieldInfo);

			/**
			 *	读取域的信息
			 * @param pIndexInput 读取
			 */
			void			read(CIndexInput* pIndexInput);
			/**
			 *	写域的信息
			 */
			void			write(CIndexOutput* pIndexOutput);

			/**
			 * 清除所有域信息
			 */
			void			clear();	

			/**
			 * 重设各个域的数据
			 */
			void			reset();
			 

			/**
			 * 获取指定域的域id
			 * @param fname 域名
			 * @return 域id
			 */
			fieldid_t		getFieldID(const tchar* fname);
			
			/**
			 * 获取指定域的域名称
			 * @param fid 域id
			 * @return 域名
			 */
			inline const tchar* 	getFieldName(fieldid_t fid);

			/**
			 * 获取指定域的域
			 * @param fid 域id
			 * @return 域
			 */
			inline CFieldInfo*		getField(fieldid_t fid);

			/**
			 * 获取指定域的域
			 * @param fid 域id
			 * @return 域
			 */
			CFieldInfo*		getField(const tchar* field);

			/**
			 * 设置指定域的索引数据在tdi文件中的偏移量
			 * @param fname 域编号
			 *        offset 偏移量			 
			 */
			void			setFieldOffset(fieldid_t fid,fileoffset_t offset);
			/**
			 * 获取指定域的索引数据在tdi文件中的偏移量
			 * @param fid 域编号
			 * @return 偏移量			 
			 */
			fileoffset_t	getFieldOffset(fieldid_t fid);

			/**
			 * 设置非重复词(distinct terms)总数
			 * @param fid 域编号
			 */ 
			void			setDistinctNumTerms(fieldid_t fid,uint64_t distterms);

			/**
			 * 获取非重复词(distinct terms)总数
			 * @param fid 域编号
			 */ 
			uint64_t		distinctNumTerms(fieldid_t fid);

			/**
			 * 获取词总数
			 * @param fid 域编号
			 */ 
			uint64_t		numTerms(fieldid_t fid);
			
			/**
			 * 获取索引域总数
			 */
			int32_t			numFields(){return (int)m_nNumFieldInfo;}

			/**
			 * 获取域总数
			 */
			int32_t			numIndexFields();
		public:
			CFieldInfo*		operator[](int32_t i){return m_ppFieldsInfo[i];}
			void			startIterator();
			bool			hasNext();
			CFieldInfo*		next();					
		protected:
			map<tstring,CFieldInfo*>	m_fdInfosByName;

			CFieldInfo**				m_ppFieldsInfo;		///Field Info Array
			int32_t						m_nNumFieldInfo;		///
			
			int32_t						m_fdInfosIterator;			
		};
		//////////////////////////////////////////////////////////////////////////
		//Inline functions
		inline void CFieldsInfo::startIterator()
		{
			m_fdInfosIterator = 0;
		}
		inline bool CFieldsInfo::hasNext()
		{
			return (m_fdInfosIterator != m_nNumFieldInfo);
		}
		inline CFieldInfo* CFieldsInfo::next()
		{
			return m_ppFieldsInfo[m_fdInfosIterator++];
		}
		inline int32_t CFieldsInfo::numIndexFields()
		{
			int32_t indFields = 0;
			for (int32_t i = 0;i<m_nNumFieldInfo;i++)
			{
				if(m_ppFieldsInfo[i]->isIndexed())
					indFields++;
			}
			return indFields;
		}
		inline const tchar* CFieldsInfo::getFieldName(fieldid_t fid)
		{
			FIRTEX_ASSERT((fid>=0 && fid<m_nNumFieldInfo),_T("Field ID is out of range."));
			return m_ppFieldsInfo[fid]->getName();			
		}
		inline CFieldInfo* CFieldsInfo::getField(fieldid_t fid)
		{
			FIRTEX_ASSERT((fid>=0 && fid<m_nNumFieldInfo),_T("Field ID is out of range."));
			return m_ppFieldsInfo[fid];						
		}
	}
}



#endif
