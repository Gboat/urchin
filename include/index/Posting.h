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
// Created	: 2005/11/20
//
#ifndef __POSTING_H
#define __POSTING_H

#include "../utility/StdHeader.h"
#include "../store/IndexOutput.h"
#include "../utility/MemCache.h"
#include <vector>
#include <fstream>

using namespace std;
using namespace firtex;
using namespace firtex::store;

namespace firtex
{
	namespace utility
	{
		class CBitVector;
	}
}

namespace firtex
{
	namespace index
	{
		class COutputDescriptor;
		class CInputDescriptor;

		///index level define
		enum INDEX_LEVEL
		{
			DOCUMENT_LEVEL_0	= 0x00,		///contain only document id,no term frequency,no term position
			DOCUMENT_LEVEL		= 0x01,		///contain only document id and term frequency,no term position
			WORD_LEVEL			= 0x02,		///contain document id, term frequency and term position
		};

#pragma pack(push,1)
		struct PostingChunk
		{			
			int32_t			size;
			PostingChunk*	next;
			uint8_t			data[1];
		};
#pragma pack(pop)

		/// the descriptor of posting
		struct PostingDescriptor
		{
			int64_t			length;		///length of the posting
			count_t			df;			///document frequency
			int64_t			ctf;		///global  term frequency
			fileoffset_t	poffset;	///offset of the position postings in the .pop file 
		};

		/// the descriptor of chunk
		struct ChunkDescriptor 
		{
			int64_t			length;		///length of the chunk
			docid_t			lastdocid;	///the last doc id of the chunk
		};

		///compressed posting list
		class CCompressedPostingList
		{
		public:
			CCompressedPostingList()
				:m_pHeadChunk(NULL)
				,m_pTailChunk(NULL)
				,m_nTotalSize(0)
				,m_nPosInCurChunk(0)
				,m_nTotalUnused(0)
			{
			}
			CCompressedPostingList(const CCompressedPostingList& src)
				:m_pHeadChunk(src.m_pHeadChunk)
				,m_pTailChunk(src.m_pTailChunk)
				,m_nTotalSize(src.m_nTotalSize)
				,m_nPosInCurChunk(src.m_nPosInCurChunk)
				,m_nTotalUnused(src.m_nTotalUnused)
			{
			}
			~CCompressedPostingList()
			{								
			}
		public:
			/**
			 * add posting which is 32 bit
			 * @param posting32 32 bit posting
			 * @return return false if no enough space, else return true.
			 */
			inline bool		addPosting(int32_t posting32);

			/**
			 * add posting which is 64 bit
			 * @param posting64 64 bit posting
			 * @return return false if no enough space, else return true.
			 */
			inline bool		addPosting(int64_t posting64);

			/**
			 * decode 32bit posting
			 * @param posting the posting
			 * @return decoded value
			 */
			inline static int32_t	decodePosting32(uint8_t*& posting);

			/**
			 * decode 64bit posting
			 * @param posting the posting
			 * @return decoded value
			 */
			inline static int64_t	decodePosting64(uint8_t*& posting);

			/**
			 * encode 32bit posting
			 * @param posting the encoded posting
			 * @param val value needed to encode
			 */
			inline static void		encodePosting32(uint8_t*& posting,int32_t val);

			/**
			 * encode 64bit posting
			 * @param posting the posting
			 * @param val value needed to encode
			 */
			inline static void		encodePosting64(uint8_t*& posting,int64_t val);

			/**
			 * add a new node to the list
			 * @param pNode the node 			 
			 */
			inline void		addChunk(PostingChunk* pChunk);

			/** truncation the tail chunk,let chunk size=real used size of this chunk */
			inline void		truncTailChunk();

			/** get the real size of the list */
			inline int32_t	getRealSize();

			/**
			 * reset the list for using at next time
			 */
			inline void		reset();			
		protected:
			PostingChunk*			m_pHeadChunk;		///Posting list header
			PostingChunk*			m_pTailChunk;		///Posting list tail
			int32_t					m_nTotalSize;		///Total size
			int32_t					m_nTotalUnused;		///Total Unused size
			int32_t					m_nPosInCurChunk;	

			friend class CInMemoryPosting_WL;
			friend class CInMemoryPosting_DL;
			friend class CInMemoryPosting_DL0;

			friend class CPostingMerger_WL;
		};		
		
		class CInMemoryPosting;
		class COnDiskPosting;
		///abstract class of posting
		class CPosting
		{
		public:
			CPosting();
			CPosting(CBitVector* pDocFilter);
			virtual~CPosting();
		public:
			/**
			 * Get the posting data
			 * @param ppState the state of decode process
			 * @param pPosing the address to store posting data
			 * @param the length of pPosting,also tell us the length of actually decoded data
			 * @return decoded posting count
			 */
			virtual int32_t	decodeNext(int32_t* pPosting,int32_t length) = 0;

			/**
			 * Get the position posting data			 
			 * @param pPosing the address to store posting data
			 * @param length length of pPosting			 
			 */
			virtual void	decodeNextPositions(int32_t* pPosting,int32_t length) = 0;

			/**
			 * Get the position posting data			 
			 * @param pPosing the address to store posting data
			 * @param pFreqs freqs array
			 * @param nFreqs size of freqs array			 
			 */
			virtual void	decodeNextPositions(int32_t* pPosting,int32_t* pFreqs,int32_t nFreqs) = 0;

			/**
			 * reset the base position which used in d-gap encoding
			 */
			virtual	void	resetPosition() = 0;

			/** 
			 * reset the content of Posting list.
			 */
			virtual void	reset() = 0;

			/**
			 * clone the object
			 * @return the clone object
			 */
			virtual CPosting*	clone() = 0;

			/** 
			 * get document frequency
			 * @return DF value
			 */
			virtual count_t	docFreq()const = 0;

			/** 
			 * get collection's total term frequency 
			 * @return CTF value
			 */
			virtual int64_t getCTF()const = 0;

			/** 
			 * set buffer for posting reading
			 * @param buffer buffer for posting 
			 * @param nBufSize size of buffer
			 * @return size of used buffer
			 */
			virtual size_t	setBuffer(int32_t* buffer,size_t nBufSize){return 0;}

			/** 
			 * get buffer size of posting
			 * @return buffer size
			 */
			virtual size_t	getBufferSize(){return 0;}
		public:
			inline void			setFilter(CBitVector* pDocFilter){m_pDocFilter = pDocFilter;}
			inline CBitVector*	getFilter(){return m_pDocFilter;}
		protected:		
			CBitVector*			m_pDocFilter;
		};

		///abstract class for on-disk posting
		class COnDiskPosting : public CPosting
		{
		public:
			COnDiskPosting(CInputDescriptor* pInputDescriptor)
				:m_pInputDescriptor(pInputDescriptor)				
			{}
			COnDiskPosting()
				:m_pInputDescriptor(NULL)
			{}
			COnDiskPosting(CInputDescriptor* pInputDescriptor,CBitVector* pDocFilter)
				:CPosting(pDocFilter)
				,m_pInputDescriptor(pInputDescriptor)				
			{}
			virtual ~COnDiskPosting(){}
		public:
			/**
			 * reset to a new posting 
			 * @param newOffset offset to the target posting
			 */
			virtual void reset(fileoffset_t newOffset) = 0;
		public:
			CInputDescriptor*	getInputDescriptor(){return m_pInputDescriptor;}
		protected:					
			CInputDescriptor*	m_pInputDescriptor;
		};


		///abstract class for in-memory posting
		class CInMemoryPosting : public CPosting
		{
		public:
			///the strategy of memory allocation
			enum MEMALLOC_STRATEGY
			{
				ALLOC_CONST		= 0x01,		
				ALLOC_EXP		= 0x02,
				ALLOC_EXPLIMIT	= 0x03,
			};
			struct AllocStrategy
			{
				MEMALLOC_STRATEGY	strategy;
				int32_t				n;
				int32_t				k;
				int32_t				l;
			};			
			
		public:
			CInMemoryPosting();
			CInMemoryPosting(CMemCache* pMemCache);
			~CInMemoryPosting();
		public:
			/**
			 * allocate new chunk
			 * @param the size of chunk
			 * @return the chunk object
			 **/
			PostingChunk*	newChunk(int32_t chunkSize);

			/**
			 * Get the next chunk size
			 * @param nCurSize current accumulated chunk size
			 * @param as memory allocation strategy
			 */
			inline int32_t	getNextChunkSize(int32_t nCurSize,AllocStrategy& as);							
		public:
			/** 
			 * Is there no chunk?
			 * @return false: has allocated; true:no memory has allocated
			 **/
			virtual bool	hasNoChunk() = 0;			
			
			/**
			 * write index data
			 * @param pOutputDescriptor output place
			 * @return  offset of posting descriptor
			 */
			virtual fileoffset_t	write(COutputDescriptor* pOutputDescriptor) = 0;

			/** 
			 * reset the content of Posting list.
			 */
			virtual void	reset() = 0;	
		protected:
			CMemCache*	m_pMemCache;	/// memory cache
		public:
			///these static members will be initialized in CConfig::Config() 
			static AllocStrategy WL_ALLOCSTRATEGY;		///WORD_LEVEL index memory allocation strategy
			static AllocStrategy DL_ALLOCSTRATEGY;		///DOCUMENT_LEVEL index memory allocation strategy
			static AllocStrategy DL0_ALLOCSTRATEGY;		///DOCUMENT_LEVEL_0 memory index allocation strategy

			static int32_t	UPTIGHT_ALLOC_CHUNKSIZE;	
			static int32_t	UPTIGHT_ALLOC_MEMSIZE;
		};
		
        //////////////////////////////////////////////////////////////////////////
		///WORD_LEVEL Posting

		///In-memory WORD_LEVEL Posting,include docid,term frequency and term location
		class CInMemoryPosting_WL : public CInMemoryPosting
		{	
			struct  DecodeState
			{			
				PostingChunk*	decodingDChunk;				
				int32_t			decodingDChunkPos;
				docid_t			lastDecodedDocID;		///the latest decoded doc id
				int32_t			decodedDocCount;
				PostingChunk*	decodingPChunk;
				int32_t			decodingPChunkPos;
				loc_t			lastDecodedPos;			///the latest decode
				int32_t			decodedPosCount;								
			};		
		public:
			CInMemoryPosting_WL();
			CInMemoryPosting_WL(CMemCache* pMemCache);
			~CInMemoryPosting_WL();
		public:
			/**
			 * add (docid,position) pair
			 * @param docid the identifier of document
			 * @param location the location of term
			 */
			void			addLocation(docid_t docid, loc_t location);	
			
			/** 
			 * Is there any chunk?
			 * @return false: has allocated; true:no memory has allocated
			 **/
			bool			hasNoChunk(){return (m_pDocFreqList->m_pTailChunk==NULL);}				

			/** 
			 * get document frequency
			 * @return DF value
			 */
			count_t			docFreq()const{return m_nDF;};

			/** 
			 * get collection's total term frequency 
			 * @return CTF value
			 */
			int64_t			getCTF()const{return m_nCTF;};

			/** get last added doc id */
			docid_t			lastDocID(){return m_nLastDocID;}

			/**
			 * write index data
			 * @param pOutputDescriptor output place
			 * @return  offset of posting descriptor
			 */
			fileoffset_t	write(COutputDescriptor* pOutputDescriptor);

			/** 
			 * reset the content of Posting list.
			 */
			void			reset();

			/**
			 * clone the object
			 * @return the clone object
			 */
			CPosting*		clone();
		public:
			/**
			 * Get the posting data
			 * @param ppState the state of decode process
			 * @param pPosing the address to store posting data
			 * @param the length of pPosting,also tell us the length of actually decoded data
			 * @return decoded posting count
			 */
			int32_t	decodeNext(int32_t* pPosting,int32_t length);

			/**
			 * Get the position posting data			 
			 * @param pPosing the address to store posting data
			 * @param the length of pPosting
			 * @return true:success,false: error or reach end
			 */
			void	decodeNextPositions(int32_t* pPosting,int32_t length);		

			/**
			 * Get the position posting data			 
			 * @param pPosing the address to store posting data
			 * @param pFreqs freqs array
			 * @param nFreqs size of freqs array			 
			 */
			void	decodeNextPositions(int32_t* pPosting,int32_t* pFreqs,int32_t nFreqs);

			/**
			 * reset the base position which used in d-gap encoding
			 */
			void	resetPosition();

			/**
			 * flush last document 
			 * @param bTruncTail trunc the tail of posting or not
			 */
			inline void flushLastDoc(bool bTruncTail);
		protected:
			/**
			 * write document posting to output
			 * @param pDOutput document posting output
			 */
			inline void	writeDPosting(CIndexOutput* pDOutput);

			/**
			 * write position posting to output
			 * @param pPOutput position posting output
			 */
			inline fileoffset_t	writePPosting(CIndexOutput* pPOutput);

			/**
			 * write descriptors to output
			 * @param pDOutput document posting output
			 * @param poffset position offset
			 */
			inline void	writeDescriptor(CIndexOutput* pDOutput,fileoffset_t poffset);			
		protected:	
			CCompressedPostingList*	m_pDocFreqList;	/// Doc freq list	
			CCompressedPostingList*	m_pLocList;		/// Location list

			DecodeState*	m_pDS;			///decoding state
			int32_t			m_nCTF;			///Collection's total term frequency
			count_t			m_nDF;			///document frequency
			count_t			m_nCurTermFreq;	///current term freq
			docid_t			m_nLastDocID;	///current added doc id
			loc_t			m_nLastLoc;		///current added location			

			friend class CPostingMerger_WL;
		};

		///On-Disk WORD_LEVEL Posting,include docid,term frequency and term location
		class COnDiskPosting_WL : public COnDiskPosting
		{
			struct  DecodeState
			{
				docid_t		lastDecodedDocID;		///the latest decoded doc id
				int32_t		decodedDocCount;		///decoded doc count
				loc_t		lastDecodedPos;			///the latest decoded position posting
				int32_t		decodedPosCount;		///decoded position posting count
			};			 
		public:
			COnDiskPosting_WL();
			COnDiskPosting_WL(CInputDescriptor* pInputDescriptor,fileoffset_t poffset);
			~COnDiskPosting_WL();
		public:
			/**
			 * Get the posting data			 
			 * @param pPosing the address to store posting data
			 * @param the length of pPosting,also tell us the length of actually decoded data
			 * @return decoded posting count
			 */
			int32_t	decodeNext(int32_t* pPosting,int32_t length);

			/**
			 * Get the position posting data			 
			 * @param pPosing the address to store posting data
			 * @param the length of pPosting
			 * @return true:success,false: error or reach end
			 */
			void	decodeNextPositions(int32_t* pPosting,int32_t length);

			/**
			 * Get the position posting data			 
			 * @param pPosing the address to store posting data
			 * @param pFreqs freqs array
			 * @param nFreqs size of freqs array			 
			 */
			void	decodeNextPositions(int32_t* pPosting,int32_t* pFreqs,int32_t nFreqs);

			/**
			 * reset the base position which used in d-gap encoding
			 */
			void	resetPosition();

			/** 
			 * reset the content of Posting list.
			 */
			void	reset();

			/**
			 * reset to a new posting 
			 * @param newOffset offset to the target posting
			 */
			void	reset(fileoffset_t newOffset);

			/**
			 * clone the object
			 * @return the clone object
			 */
			CPosting*		clone();

			/** 
			 * get document frequency
			 * @return DF value
			 */
			count_t	docFreq()const{return m_postingDesc.df;};

			/** 
			 * get collection's total term frequency 
			 * @return CTF value
			 */
			int64_t getCTF()const{return m_postingDesc.ctf;};

			/** 
			 * set buffer for posting reading
			 * @param buffer buffer for posting 
			 * @param nBufSize size of buffer
			 * @return size of used buffer
			 */
			size_t	 setBuffer(int32_t* buffer,size_t nBufSize);

			/** 
			 * get buffer size of posting
			 * @return buffer size
			 */
			virtual size_t	getBufferSize(){return m_nBufSize;}
		protected:					
			PostingDescriptor	m_postingDesc;
			ChunkDescriptor		m_chunkDesc;
			fileoffset_t		m_postingOffset;
			int64_t				m_nPPostingLength;
			size_t				m_nBufSize;

			COnDiskPosting_WL::DecodeState m_ds;

			friend class CPostingMerger_WL;
		};		

		//////////////////////////////////////////////////////////////////////////
		///DOCUMENT_LEVEL Posting,include docid,term frequency
		class CInMemoryPosting_DL : public CInMemoryPosting
		{
		public:
			CInMemoryPosting_DL(CMemCache* pMemCache);
			~CInMemoryPosting_DL();
		public:
			/**
			 * add doc id 
			 * @param docid the identifier of document			 
			 */
			inline void		addDocument(docid_t docid);	

			/** 
			 * 判断有没有分配内存
			 * @return false: has allocated; true:no memory has allocated
			 **/
			bool		hasNoChunk(){return (m_pDocFreqList->m_pTailChunk==NULL);}				

			/** 
			 * get document frequency
			 * @return DF value
			 */
			count_t	docFreq()const{return m_nDF;};

			/** 
			 * get collection's total term frequency 
			 * @return CTF value
			 */
			int64_t getCTF()const {return m_nCTF;};

			/** get last added doc id */
			docid_t		lastDocID(){return m_nLastDocID;}

			/**
			 * write index data
			 * @param pOutputDescriptor output place
			 * @return  offset of posting descriptor
			 */
			fileoffset_t	write(COutputDescriptor* pOutputDescriptor);

			/** 
			* reset the content of Posting list.
			*/
			void		reset();

			/**
			 * clone the object
			 * @return the clone object
			 */
			CPosting*		clone(){return NULL;}
		public:
			/**
			 * Get the posting data			 
			 * @param pPosing the address to store posting data
			 * @param the length of pPosting,also tell us the length of actually decoded data
			 * @return decoded posting count
			 */
			int32_t	decodeNext(int32_t* pPosting,int32_t length);

			/**
			 * Get the position posting data			 
			 * @param pPosing the address to store posting data
			 * @param the length of pPosting
			 * @return true:success,false: error or reach end
			 */
			void	decodeNextPositions(int32_t* pPosting,int32_t length);

			/**
			 * Get the position posting data			 
			 * @param pPosing the address to store posting data
			 * @param pFreqs freqs array
			 * @param nFreqs size of freqs array			 
			 */
			void	decodeNextPositions(int32_t* pPosting,int32_t* pFreqs,int32_t nFreqs);

			/**
			 * reset the base position which used in d-gap encoding
			 */
			void	resetPosition();
		protected:			
			CCompressedPostingList*	m_pDocFreqList;

			int32_t			m_nCTF;			///Collection's total term frequency
			count_t			m_nDF;			///document frequency
			count_t			m_nCurTermFreq;	///current term freq
			docid_t			m_nLastDocID;	///last added doc id
		};

		///On-Disk DOCUMENT_LEVEL Posting
		class COnDiskPosting_DL : public COnDiskPosting
		{
			struct  DecodeState
			{
				docid_t		lastDecodedDocID;		///the latest decoded doc id
				int32_t		decodedDocCount;		///decoded doc count				
			};			 
		public:
			COnDiskPosting_DL(CInputDescriptor* pInputDescriptor);
			~COnDiskPosting_DL();
		public:
			/**
			 * Get the posting data			 
			 * @param pPosing the address to store posting data
			 * @param the length of pPosting,also tell us the length of actually decoded data
			 * @return decoded posting count
			 */
			int32_t	decodeNext(int32_t* pPosting,int32_t length);

			/**
			 * Get the position posting data			 
			 * @param pPosing the address to store posting data
			 * @param the length of pPosting
			 * @return true:success,false: error or reach end
			 */
			void	decodeNextPositions(int32_t* pPosting,int32_t length);

			/**
			 * Get the position posting data			 
			 * @param pPosing the address to store posting data
			 * @param pFreqs freqs array
			 * @param nFreqs size of freqs array			 
			 */
			void	decodeNextPositions(int32_t* pPosting,int32_t* pFreqs,int32_t nFreqs);

			/**
			 * reset the base position which used in d-gap encoding
			 */
			void	resetPosition();

			/** 
			 * reset the content of Posting list.
			 */
			void	reset();

			/**
			 * reset to a new posting 
			 * @param newOffset offset to the target posting
			 */
			void reset(fileoffset_t newOffset);

			/**
			 * clone the object
			 * @return the clone object
			 */
			CPosting*		clone(){return NULL;}

			/** 
			 * get document frequency
			 * @return DF value
			 */
			count_t	docFreq()const{return m_postingDesc.df;};

			/** 
			 * get collection's total term frequency 
			 * @return CTF value
			 */
			int64_t getCTF()const{return m_postingDesc.ctf;};
		protected:					
			PostingDescriptor	m_postingDesc;
			ChunkDescriptor		m_chunkDesc;			

			COnDiskPosting_DL::DecodeState m_ds;
		};		

		//////////////////////////////////////////////////////////////////////////
		///DOCUMENT_LEVEL_0 Posting,only include docid
		class CInMemoryPosting_DL0 : public CInMemoryPosting
		{
		struct  DecodeState
			{			
				PostingChunk*	decodingDChunk;				
				int32_t			decodingDChunkPos;
				docid_t			lastDecodedDocID;		///the latest decoded doc id
				int32_t			decodedDocCount;				
			};		
		public:
			CInMemoryPosting_DL0();
			CInMemoryPosting_DL0(CMemCache* pMemCache);
			~CInMemoryPosting_DL0();
		public:
			/**
			 * add doc id 
			 * @param docid the identifier of document			 
			 */
			inline void		addDocument(docid_t docid);	
			
			/** 
			 * Is there any chunk?
			 * @return false: has allocated; true:no memory has allocated
			 **/
			bool			hasNoChunk(){return (m_pDocList->m_pTailChunk==NULL);}				

			/** 
			 * get document frequency
			 * @return DF value
			 */
			count_t	docFreq()const{return m_nDF;};

			/** 
			 * get collection's total term frequency 
			 * @return CTF value
			 */
			int64_t getCTF()const{return m_nDF;};

			/** get last added doc id */
			docid_t			lastDocID(){return m_nLastDocID;}

			/**
			 * write index data
			 * @param pOutputDescriptor output place
			 * @return  offset of posting descriptor
			 */
			fileoffset_t	write(COutputDescriptor* pOutputDescriptor);

			/** 
			 * reset the content of Posting list.
			 */
			void			reset();

			/**
			 * clone the object
			 * @return the clone object
			 */
			CPosting*		clone();
		public:
			/**
			 * Get the posting data
			 * @param ppState the state of decode process
			 * @param pPosing the address to store posting data
			 * @param the length of pPosting,also tell us the length of actually decoded data
			 * @return decoded posting count
			 */
			int32_t	decodeNext(int32_t* pPosting,int32_t length);

			/**
			 * Get the position posting data			 
			 * @param pPosing the address to store posting data
			 * @param the length of pPosting
			 * @return true:success,false: error or reach end
			 */
			void	decodeNextPositions(int32_t* pPosting,int32_t length);		

			/**
			 * Get the position posting data			 
			 * @param pPosing the address to store posting data
			 * @param pFreqs freqs array
			 * @param nFreqs size of freqs array			 
			 */
			void	decodeNextPositions(int32_t* pPosting,int32_t* pFreqs,int32_t nFreqs);

			/**
			 * reset the base position which used in d-gap encoding
			 */
			void	resetPosition();			
		protected:
			/**
			 * write document posting to output
			 * @param pDOutput document posting output
			 */
			inline void	writeDPosting(CIndexOutput* pDOutput);		

			/**
			 * write descriptors to output
			 * @param pDOutput document posting output
			 * @param poffset position offset
			 */
			inline void	writeDescriptor(CIndexOutput* pDOutput,fileoffset_t poffset);			
		protected:	
			CCompressedPostingList*	m_pDocList;	/// Doc freq list	

			DecodeState*	m_pDS;			///decoding state
			count_t			m_nDF;			///document frequency
			docid_t			m_nLastDocID;	///current added doc id

			friend class CPostingMerger_DL0;
		};

		///On-Disk DOCUMENT_LEVEL_0 Posting
		class COnDiskPosting_DL0 : public COnDiskPosting
		{
			struct  DecodeState
			{
				docid_t		lastDecodedDocID;		///the latest decoded doc id
				int32_t		decodedDocCount;		///decoded doc count				
			};			 
		public:
			COnDiskPosting_DL0();
			COnDiskPosting_DL0(CInputDescriptor* pInputDescriptor,fileoffset_t poffset);
			~COnDiskPosting_DL0();
		public:
			/**
			 * Get the posting data			 
			 * @param pPosing the address to store posting data
			 * @param the length of pPosting,also tell us the length of actually decoded data
			 * @return decoded posting count
			 */
			int32_t	decodeNext(int32_t* pPosting,int32_t length);

			/**
			 * Get the position posting data			 
			 * @param pPosing the address to store posting data
			 * @param the length of pPosting
			 * @return true:success,false: error or reach end
			 */
			void	decodeNextPositions(int32_t* pPosting,int32_t length);

			/**
			 * Get the position posting data			 
			 * @param pPosing the address to store posting data
			 * @param pFreqs freqs array
			 * @param nFreqs size of freqs array			 
			 */
			void	decodeNextPositions(int32_t* pPosting,int32_t* pFreqs,int32_t nFreqs);

			/**
			 * reset the base position which used in d-gap encoding
			 */
			void	resetPosition(){}

			/** 
			 * reset the content of Posting list.
			 */
			void	reset();

			/**
			 * reset to a new posting 
			 * @param newOffset offset to the target posting
			 */
			void	reset(fileoffset_t newOffset);

			/**
			 * clone the object
			 * @return the clone object
			 */
			CPosting*		clone();

			/** 
			 * get document frequency
			 * @return DF value
			 */
			count_t	docFreq()const{return m_postingDesc.df;};

			/** 
			 * get collection's total term frequency 
			 * @return CTF value
			 */
			int64_t getCTF()const{return m_postingDesc.df;};

			/** 
			 * set buffer for posting reading
			 * @param buffer buffer for posting 
			 * @param nBufSize size of buffer
			 * @return size of used buffer
			 */
			size_t	 setBuffer(int32_t* buffer,size_t nBufSize);

			/** 
			 * get buffer size of posting
			 * @return buffer size
			 */
			virtual size_t	getBufferSize(){return m_nBufSize;}
		protected:					
			PostingDescriptor	m_postingDesc;
			ChunkDescriptor		m_chunkDesc;
			fileoffset_t		m_postingOffset;			
			size_t				m_nBufSize;

			COnDiskPosting_DL0::DecodeState m_ds;

			friend class CPostingMerger_DL0;
		};		


		//////////////////////////////////////////////////////////////////////////
		//inline functions

		//////////////////////////////////////////////////////////////////////////
		///CCompressedPostingList
		inline bool CCompressedPostingList::addPosting(int32_t posting32)
		{
			if(m_pTailChunk == NULL)
				return false;
			int32_t left = m_pTailChunk->size - m_nPosInCurChunk;
			if(left < 4)///at least 4 free space
			{
				m_nTotalUnused += left;///Unused size
				m_pTailChunk->size = m_nPosInCurChunk;///the real size
				return false;
			}

			uint32_t ui = posting32;
			while ((ui & ~0x7F) != 0) 
			{
				m_pTailChunk->data[m_nPosInCurChunk++] = ((uint8_t)((ui & 0x7f) | 0x80));				
				ui >>= 7; 
			}
			m_pTailChunk->data[m_nPosInCurChunk++] = (uint8_t)ui;		
			
			return true;
		}

		inline bool CCompressedPostingList::addPosting(int64_t posting64)
		{
			if(m_pTailChunk == NULL)
				return false;
			int32_t left = m_pTailChunk->size - m_nPosInCurChunk;
			if(left < 8)///at least 4 free space
			{
				m_nTotalUnused += left;///Unused size
				m_pTailChunk->size = m_nPosInCurChunk;///the real size
				return false;
			}

			uint64_t ui = posting64;
			while ((ui & ~0x7F) != 0) 
			{
				m_pTailChunk->data[m_nPosInCurChunk++] = ((uint8_t)((ui & 0x7f) | 0x80));
				ui >>= 7; 
			}
			m_pTailChunk->data[m_nPosInCurChunk++] = ((uint8_t)ui);
			
			return true;
		}
		inline int32_t CCompressedPostingList::decodePosting32(uint8_t*& posting)
		{
			uint8_t b = *posting++;
			int32_t i = b & 0x7F;
			for (int32_t shift = 7; (b & 0x80) != 0; shift += 7) 
			{
				b = *posting++;
				i |= (b & 0x7FL) << shift;
			}
			return i;
		}

		inline int64_t CCompressedPostingList::decodePosting64(uint8_t*& posting)
		{
			uint8_t b = *posting++;
			int64_t i = b & 0x7F;
			for (int32_t shift = 7; (b & 0x80) != 0; shift += 7)
			{
				b = *posting++;
				i |= (b & 0x7FLL) << shift;
			}
			return i;
		}
		inline  void CCompressedPostingList::encodePosting32(uint8_t*& posting,int32_t val)
		{
			uint32_t ui = val;
			while ((ui & ~0x7F) != 0) 
			{
				*posting++ = ((uint8_t)((ui & 0x7f) | 0x80));				
				ui >>= 7; 
			}
			*posting++ = (uint8_t)ui;				
		}
		inline void CCompressedPostingList::encodePosting64(uint8_t*& posting,int64_t val)
		{
			uint64_t ui = val;
			while ((ui & ~0x7F) != 0) 
			{
				*posting++ = ((uint8_t)((ui & 0x7f) | 0x80));
				ui >>= 7; 
			}
			*posting++ = ((uint8_t)ui);
		}

		inline void CCompressedPostingList::truncTailChunk()
		{
			m_nTotalUnused += m_pTailChunk->size - m_nPosInCurChunk;
			m_pTailChunk->size = m_nPosInCurChunk;
		}

		inline void CCompressedPostingList::addChunk(PostingChunk* pChunk)
		{
			if(m_pTailChunk)
				m_pTailChunk->next = pChunk;
			m_pTailChunk = pChunk;
			if(!m_pHeadChunk)
				m_pHeadChunk = m_pTailChunk;
			m_nTotalSize += pChunk->size;
			m_nPosInCurChunk = 0;
		}
		inline int32_t CCompressedPostingList::getRealSize()
		{
			return m_nTotalSize - m_nTotalUnused;
		}

		inline void CCompressedPostingList::reset()
		{
			m_pHeadChunk = m_pTailChunk = NULL;
			m_nTotalSize = m_nPosInCurChunk = m_nTotalUnused = 0;
		}
		
		//////////////////////////////////////////////////////////////////////////
		///CInMemoryPosting		
		inline int32_t CInMemoryPosting::getNextChunkSize(int32_t nCurSize,AllocStrategy& as)
		{
			int32_t newSize = 0;
			if(as.strategy == ALLOC_EXPLIMIT)
			{
				newSize = min(as.l,max(as.n,(int32_t)(nCurSize*(as.k - 1) + 0.5)) );
			}
			else if(as.strategy == ALLOC_EXP)
			{
				newSize = max(as.n,(int32_t)(nCurSize*(as.k - 1) + 0.5));
			}
			else if(as.strategy == ALLOC_CONST)
				newSize = as.n;
			return (int32_t)CMisc::LOG2_UP(newSize);
		}

		//////////////////////////////////////////////////////////////////////////
		///CInMemoryPosting_WL		
		inline void CInMemoryPosting_WL::addLocation(docid_t docid, loc_t location) 
		{			
			///see it first time?
			if (docid == m_nLastDocID) 
			{		
				if(!m_pLocList->addPosting(location - m_nLastLoc))				
				{
					///chunk is exhausted
					int32_t newSize = getNextChunkSize(m_pLocList->m_nTotalSize,CInMemoryPosting::WL_ALLOCSTRATEGY);
					m_pLocList->addChunk(newChunk(newSize));
					m_pLocList->addPosting(location - m_nLastLoc);///d-gap encoding
				}
				m_nCurTermFreq++;
				m_nLastLoc = location;				
			} 
			else///first see it
			{
				if(m_nCurTermFreq > 0)///write previous document's term freq
				{
					if(!m_pDocFreqList->addPosting(m_nCurTermFreq))
					{
						///chunk is exhausted
						int32_t newSize = getNextChunkSize(m_pDocFreqList->m_nTotalSize,CInMemoryPosting::DL_ALLOCSTRATEGY);
						m_pDocFreqList->addChunk(newChunk(newSize));
						m_pDocFreqList->addPosting(m_nCurTermFreq);
					}
				}		
				else if(m_nLastDocID == -1)///first see it
				{					
					m_nLastDocID = 0;
				}
				if(!m_pDocFreqList->addPosting(docid - m_nLastDocID))
				{
					///chunk is exhausted
					int32_t newSize = getNextChunkSize(m_pDocFreqList->m_nTotalSize,CInMemoryPosting::DL_ALLOCSTRATEGY);
					m_pDocFreqList->addChunk(newChunk(newSize));
					m_pDocFreqList->addPosting(docid - m_nLastDocID);
				}
				if(!m_pLocList->addPosting(location))
				{
					///chunk is exhausted
					int32_t newSize = getNextChunkSize(m_pLocList->m_nTotalSize,CInMemoryPosting::WL_ALLOCSTRATEGY);
					m_pLocList->addChunk(newChunk(newSize));					
					m_pLocList->addPosting(location);
				}

				m_nCTF += m_nCurTermFreq;
				m_nCurTermFreq = 1;

				m_nLastDocID = docid;

				///save the last location in order to enable d-gap encoding
				m_nLastLoc = location;

				m_nDF++;
			}
		}

		inline void CInMemoryPosting_WL::writeDPosting(CIndexOutput* pDOutput)
		{
			///write chunk data
			PostingChunk* pChunk = m_pDocFreqList->m_pHeadChunk;
			while(pChunk)
			{				
				pDOutput->write((const char*)pChunk->data,pChunk->size);
				pChunk = pChunk->next;
			}			
		}
		
		inline fileoffset_t CInMemoryPosting_WL::writePPosting(CIndexOutput* pPOutput)
		{
			///write position posting data
			PostingChunk* pChunk = m_pLocList->m_pHeadChunk;
			while(pChunk)
			{				
				pPOutput->write((const char*)pChunk->data,pChunk->size);
				pChunk = pChunk->next;
			}
			fileoffset_t poffset = pPOutput->getFilePointer();
			pPOutput->writeVLong(m_pLocList->getRealSize());///<ChunkLength(VInt64)>
			return poffset;
		}		
		inline void CInMemoryPosting_WL::writeDescriptor(CIndexOutput* pDOutput,fileoffset_t poffset)
		{
			///begin write posting descriptor
			pDOutput->writeVLong(m_pDocFreqList->getRealSize());///<PostingLength(VInt64)>
			pDOutput->writeVInt(m_nDF);							///<DF(VInt32)>
			pDOutput->writeVLong(m_nCTF);						///<CTF(VInt64)>			
			pDOutput->writeVLong(poffset);						///<PositionPointer(VInt64)>
			///end write posting descriptor

			pDOutput->writeVInt(1);								///<ChunkCount(VInt32)>
			///begin write chunk descriptor
			pDOutput->writeVLong(m_pDocFreqList->getRealSize());///<ChunkLength(VInt64)>
			pDOutput->writeVInt(m_nLastDocID);					///<LastDocID(VInt32)>
			///end write posting descriptor
		}

		inline void CInMemoryPosting_WL::flushLastDoc(bool bTruncTail)
		{
			if(m_nCurTermFreq > 0)
			{
				if(!m_pDocFreqList->addPosting(m_nCurTermFreq))
				{
					//没内存了					
					int32_t newSize = getNextChunkSize(m_pDocFreqList->m_nTotalSize,CInMemoryPosting::DL_ALLOCSTRATEGY);
					m_pDocFreqList->addChunk(newChunk(newSize));
					m_pDocFreqList->addPosting(m_nCurTermFreq);
				}
				if(bTruncTail)
				{
					m_pDocFreqList->truncTailChunk();///update real size
					m_pLocList->truncTailChunk();///update real size
				}
				m_nCTF += m_nCurTermFreq;
				m_nCurTermFreq = 0;
			}
			else if(bTruncTail)
			{
				m_pDocFreqList->truncTailChunk();///update real size
				m_pLocList->truncTailChunk();///update real size
			}
		}

		//////////////////////////////////////////////////////////////////////////
		///CInMemoryPosting_DL
		inline void CInMemoryPosting_DL::addDocument(docid_t docid)
		{
			//判断是否旧文档
			if (docid == m_nLastDocID) 
			{
				m_nCurTermFreq++;			
			} 
			else//新文档
			{
				if(m_nCurTermFreq > 0)//写上一文档的term freq
				{
					if(!m_pDocFreqList->addPosting(m_nCurTermFreq))
					{
						//没内存了
						int32_t newSize = getNextChunkSize(m_pDocFreqList->m_nTotalSize,CInMemoryPosting::DL_ALLOCSTRATEGY);
						m_pDocFreqList->addChunk(newChunk(newSize));						
						m_pDocFreqList->addPosting(m_nCurTermFreq);
					}
				}		
				else 
				{
					//第一个文档
					if(m_nLastDocID == -1)
						m_nLastDocID = 0;
				}
				if(!m_pDocFreqList->addPosting(docid - m_nLastDocID))
				{
					//没内存了
					int32_t newSize = getNextChunkSize(m_pDocFreqList->m_nTotalSize,CInMemoryPosting::DL_ALLOCSTRATEGY);
					m_pDocFreqList->addChunk(newChunk(newSize));						
					m_pDocFreqList->addPosting(docid - m_nLastDocID);
				}
				m_nCTF += m_nCurTermFreq;
				m_nCurTermFreq = 1;
				m_nLastDocID = docid;
				m_nDF++;
			}
		}

		//////////////////////////////////////////////////////////////////////////
		///CInMemoryPosting_DL
		inline void CInMemoryPosting_DL0::addDocument(docid_t docid)
		{
			//判断是否旧文档
			if (docid == m_nLastDocID) 
			{
				return ;
			} 
			else//新文档
			{
				
				//第一个文档
				if(m_nLastDocID == -1)
					m_nLastDocID = 0;				
				if(!m_pDocList->addPosting(docid - m_nLastDocID))
				{
					//没内存了
					int32_t newSize = getNextChunkSize(m_pDocList->m_nTotalSize,CInMemoryPosting::DL0_ALLOCSTRATEGY);
					m_pDocList->addChunk(newChunk(newSize));						
					m_pDocList->addPosting(docid - m_nLastDocID);
				}				
				m_nLastDocID = docid;
				m_nDF++;
			}
		}		
		inline void CInMemoryPosting_DL0::writeDescriptor(CIndexOutput* pDOutput,fileoffset_t poffset)
		{
			///begin write posting descriptor
			pDOutput->writeVLong(m_pDocList->getRealSize());	///<PostingLength(VInt64)>
			pDOutput->writeVInt(m_nDF);							///<DF(VInt32)>
			pDOutput->writeVLong(m_nDF);						///<CTF(VInt64)>,DF as CTF			
			pDOutput->writeVLong(poffset);						///<PositionPointer(VInt64)>
			///end write posting descriptor

			pDOutput->writeVInt(1);								///<ChunkCount(VInt32)>
			///begin write chunk descriptor
			pDOutput->writeVLong(m_pDocList->getRealSize());	///<ChunkLength(VInt64)>
			pDOutput->writeVInt(m_nLastDocID);					///<LastDocID(VInt32)>
			///end write posting descriptor
		}
		inline void CInMemoryPosting_DL0::writeDPosting(CIndexOutput* pDOutput)
		{
			///write chunk data
			PostingChunk* pChunk = m_pDocList->m_pHeadChunk;
			while(pChunk)
			{				
				pDOutput->write((const char*)pChunk->data,pChunk->size);
				pChunk = pChunk->next;
			}
		}
	}
}



#endif
