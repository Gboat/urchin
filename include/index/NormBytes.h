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
// Modified : 2006/10/3 change norms from byte to int32_t,no precision lost,by GuoRuijie.
//
#ifndef _NORMBYTES_H
#define _NORMBYTES_H
#include "../utility/StdHeader.h"
#include "../utility/Config.h"

#define NORMBYTE_8		1
#define NORMBYTE_16		2
#define NORMBYTE_32		4

namespace firtex
{
	namespace index
	{
		//class CNormBytes
		//{		
		//public:
		//	CNormBytes()
		//		:m_bytes(NULL)
		//		,m_length(0)
		//		,m_normByteLen(NORMBYTE_32)
		//	{				
		//	}
		//	CNormBytes(size_t normsLength)
		//		:m_normByteLen(NORMBYTE_32)
		//	{					
		//		m_length = normsLength;
		//		m_bytes = new byte[normsLength*m_normByteLen];
		//	}
		//	CNormBytes(size_t normsLength,byte normByteLen)				
		//	{					
		//		if((normByteLen != NORMBYTE_8) 
		//			&& (normByteLen != NORMBYTE_16) 
		//			&& (normByteLen != NORMBYTE_32))
		//		{
		//			normByteLen = NORMBYTE_32;
		//		}
		//		m_normByteLen = normByteLen;
		//		m_length = normsLength;
		//		m_bytes = new byte[normsLength*m_normByteLen];
		//	}
		//	~CNormBytes()
		//	{
		//		if(m_bytes)
		//		{
		//			delete[] m_bytes;
		//			bytes = NULL;
		//		}
		//		m_length = 0;
		//	}
		//public:
		//	/**
		//	 * get norms data
		//	 * @return norms data
		//	 */
		//	inline byte*	getBytes(){return m_bytes;}

		//	/**
		//	 * get length of norms, not the length of norms data. it is equal to the number of documents, 
		//	 * it is also equal to the norms data length when norm byte length is NORMBYTE_32.
		//	 * length of norms data = getLength()*length of single norm byte
		//	 * @return norms length
		//	 */
		//	inline size_t	getLength(){return m_length;}

		//	/**
		//	 * get length of single norm byte
		//	 * @return length of single norm byte
		//	 */
		//	inline byte		getByteLength(){return m_normByteLen;}
		//protected:
		//	byte*	m_bytes;			///norms data, 
		//	size_t	m_length;			///length of norms
		//	byte	m_normByteLen;		///length of single norm byte

		//	friend class CNormsBytes;
		//};

		class CNormBytes
		{		
		public:
			CNormBytes()
				:m_bytes(NULL)
				,m_length(0)
			{				
			}		
			CNormBytes(size_t normsLength)
			{				
				m_length = normsLength;
				m_bytes = new int32_t[normsLength];
			}
			~CNormBytes()
			{
				if(m_bytes)
				{
					delete[] m_bytes;
					m_bytes = NULL;
				}
				m_length = 0;
			}
		public:
			/**
			 * get norms data
			 * @return norms data
			 */
			inline int32_t*	getBytes(){return m_bytes;}

			/**
			 * get length of norms
			 * @return norms length
			 */
			inline size_t	getLength(){return m_length;}		
		protected:
			int32_t*	m_bytes;			///norms data, 
			size_t		m_length;			///length of norms

			friend class CNormsBytes;
		};

		class CNormsBytes
		{
		public:
			CNormsBytes();
			CNormsBytes(size_t numSubNorms);
			~CNormsBytes();
		public:
			/**
			 * add norms
			 * @param _off the order of norms
			 * @param pNorms norms
			 */
			void		setNorms(size_t _off,CNormsBytes* pNorms);

			/**
			 * set norm bytes
			 * @param _off offset in sub norms array
			 */
			void		setNormBytes(int32_t _off,CNormBytes* pNormByte);

			/**
			 * get norm bytes
			 * @param _off offset in sub norms array
			 */
			CNormBytes*	getNormBytes(int32_t _off){return m_ppSubNorms[_off];}
		public:
			/**
			 * get norm of document
			 * @param did document id
			 * @return norm of document
			 */
			inline int32_t		getNorm(docid_t did);

			/**
			 * get number of sub norms
			 * @return number of sub norms
			 */
			inline size_t		numSubNorms(){return m_nNumSubNorms;}			
		protected:
			CNormBytes**	m_ppSubNorms;
			size_t			m_nNumSubNorms;

			friend class CNormsReader;
		};
		//////////////////////////////////////////////////////////////////////////
		///inline function
		inline int32_t CNormsBytes::getNorm(docid_t did)
		{
			if(m_nNumSubNorms == 1)
			{
				return m_ppSubNorms[0]->m_bytes[did];
			}
			else 
			{
				size_t len = 0;
				for (size_t i = 0;i<m_nNumSubNorms;i++)
				{
					len += m_ppSubNorms[i]->m_length;
					if((size_t)did < len)
					{
						return m_ppSubNorms[i]->m_bytes[did - len + m_ppSubNorms[i]->m_length];
					}
				}
				FIRTEX_ASSERT(0,_T("search norms failed."));
			}
			return 0;
		}		
	}
}
#endif
