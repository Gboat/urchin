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
// Created	: 2006/7/2
//
#include "index/ForwardIndexReader.h"
#include "index/FieldFactory.h"

namespace firtex
{
	namespace index
	{
		CForwardIndexReader::CForwardIndexReader()
			:m_pDirectory(NULL)
			,m_pFieldsInfo(NULL)
			,m_pFIMInput(NULL)
			,m_pFIDInput(NULL)
			,m_pFIVInput(NULL)
			,m_pFieldFactories(NULL)
		{
		}
		CForwardIndexReader::CForwardIndexReader(CDirectory* pDirectory,CFieldsInfo* pFieldsInfo)
			:m_pDirectory(pDirectory)
			,m_pFieldsInfo(pFieldsInfo)
			,m_pFIMInput(NULL)
			,m_pFIDInput(NULL)
			,m_pFIVInput(NULL)
			,m_pFieldFactories(NULL)
		{
		}

		CForwardIndexReader::~CForwardIndexReader(void)
		{
			m_pDirectory = NULL;
			m_pFieldsInfo = NULL;
			m_pFieldFactories = NULL;
			close();
		}
		
		void CForwardIndexReader::open(const tchar* barrelName)
		{
			close();
			tstring strName = barrelName;			
			m_pFIMInput = m_pDirectory->openInput(strName + _T(".fim"));
			m_pFIDInput = m_pDirectory->openInput(strName + _T(".fid"));
			m_pFIVInput = m_pDirectory->openInput(strName + _T(".fiv"));
		}		
		void CForwardIndexReader::close()
		{
			if(m_pFIMInput)
			{
				delete m_pFIMInput;
				m_pFIMInput = NULL;
			}
			if(m_pFIDInput)
			{
				delete m_pFIDInput;
				m_pFIDInput = NULL;
			}
			if(m_pFIVInput)
			{
				delete m_pFIVInput;
				m_pFIVInput = NULL;
			}
		}

		forwardindex_t	CForwardIndexReader::getForwardIndexType(const tchar* field)
		{
			fieldid_t fid = m_pFieldsInfo->getFieldID(field);
			if(fid == -1)
				return ForwardIndex::NO;
			CFieldInfo* pFieldInfo = m_pFieldsInfo->getField(fid);
			if(pFieldInfo->isSequenceVector())
				return ForwardIndex::SEQUENCE;
			if(pFieldInfo->isFreqVector())
				return ForwardIndex::FREQ;
			if(pFieldInfo->isPositionVector())
				return ForwardIndex::POSITION;
			return ForwardIndex::NO;
		}

		CTermFreqVector* CForwardIndexReader::getFreqVector(docid_t did,const tchar* field)
		{			
			fieldid_t fid = m_pFieldsInfo->getFieldID(field);
			if(fid == -1)
				return NULL;
			CFieldInfo* pFieldInfo = m_pFieldsInfo->getField(fid);
			if(pFieldInfo->isFreqVector() == false)
				return NULL;			

			m_pFIMInput->seek(did*sizeof(fileoffset_t));
			fileoffset_t tvdOff = m_pFIMInput->readLong();	//<DocumentPosition(Int64)> 
			m_pFIDInput->seek(tvdOff);
			int32_t numFields = m_pFIDInput->readVInt();	//<NumFields(VInt32)>
			fileoffset_t tvvOff = -1;
			fieldid_t	f;
			for(int32_t i = 0;i<numFields;i++)
			{
				f = m_pFIDInput->readVInt();				//<FieldNum(VInt32)>
				if(f == fid)
				{
					tvvOff = m_pFIDInput->readVLong();		//<FieldPositions(VInt64)>
					break;
				}
				else 
				{
					m_pFIDInput->readVLong();				//skip
				}
			}
			if(tvvOff == -1)
				return NULL;
			m_pFIVInput->seek(tvvOff);
			CTermFreqVector* pVector = m_pFieldFactories->getFactory(pFieldInfo->getID())->createTermFreqVector();
			pVector->open(field,m_pFIVInput);
			return pVector;
		}
			
		CForwardIndexArray<CTermFreqVector*>* CForwardIndexReader::getFreqVectors(docid_t did)
		{
			vector<CTermFreqVector*> freqVectors;
			CTermFreqVector* pFreqVector;

			CFieldInfo* pFieldInfo ;
			m_pFIMInput->seek(did*sizeof(fileoffset_t));
			fileoffset_t tvdOff = m_pFIMInput->readLong();	//<DocumentPosition(Int64)> 
			m_pFIDInput->seek(tvdOff);
			int32_t numFields = m_pFIDInput->readVInt();	//<NumFields(VInt32)>
			fileoffset_t tvvOff = -1;
			fieldid_t	f;
			for(int32_t i = 0;i<numFields;i++)
			{
				f = m_pFIDInput->readVInt();		//<FieldNum(VInt32)>
				tvvOff = m_pFIDInput->readVLong();	//<FieldPositions(VInt64)>
				pFieldInfo = m_pFieldsInfo->getField(f);
				if(pFieldInfo && pFieldInfo->isFreqVector())
				{
					m_pFIVInput->seek(tvvOff);
					pFreqVector = m_pFieldFactories->getFactory(pFieldInfo->getID())->createTermFreqVector();
					pFreqVector->open(pFieldInfo->getName(),m_pFIVInput);
					if(pFreqVector)
						freqVectors.push_back(pFreqVector);
				}				
				else 
				{
					m_pFIDInput->readVLong();//skip
				}
			}

			if(freqVectors.size() > 0)
				return new CForwardIndexArray<CTermFreqVector*>(&freqVectors);
			return NULL;
		}
		CTermPositionVector* CForwardIndexReader::getPositionVector(docid_t did,const tchar* field)
		{
			fieldid_t fid = m_pFieldsInfo->getFieldID(field);
			if(fid == -1)
				return NULL;
			CFieldInfo* pFieldInfo = m_pFieldsInfo->getField(fid);
			if(pFieldInfo->isPositionVector() == false)
				return NULL;			

			m_pFIMInput->seek(did*sizeof(fileoffset_t));
			fileoffset_t tvdOff = m_pFIMInput->readLong();//<DocumentPosition(Int64)> 
			m_pFIDInput->seek(tvdOff);
			int32_t numFields = m_pFIDInput->readVInt();//<NumFields(VInt32)>
			fileoffset_t tvvOff = -1;
			fieldid_t	f;
			for(int32_t i = 0;i<numFields;i++)
			{
				f = m_pFIDInput->readVInt();	//<FieldNum(VInt32)>
				if(f == fid)
				{
					tvvOff = m_pFIDInput->readVLong();//<FieldPositions(VInt64)>
					break;
				}
				else 
				{
					m_pFIDInput->readVLong();//skip
				}
			}
			if(tvvOff == -1)
				return NULL;
			m_pFIVInput->seek(tvvOff);
			CTermPositionVector* pPVector = m_pFieldFactories->getFactory(pFieldInfo->getID())->createTermPositionVector();
			pPVector->open(field,m_pFIVInput);
			return pPVector;
		}

		CForwardIndexArray<CTermPositionVector*>* CForwardIndexReader::getPositionVectors(docid_t did)
		{
			vector<CTermPositionVector*> freqVectors;
			CTermPositionVector* pPosVector;

			CFieldInfo* pFieldInfo ;
			m_pFIMInput->seek(did*sizeof(fileoffset_t));
			fileoffset_t tvdOff = m_pFIMInput->readVLong();	//<DocumentPosition(Int64)> 
			m_pFIDInput->seek(tvdOff);
			int32_t numFields = m_pFIDInput->readVInt();	//<NumFields(VInt32)>
			fileoffset_t tvvOff = -1;
			fieldid_t	f;
			for(int32_t i = 0;i<numFields;i++)
			{
				f = m_pFIDInput->readVInt();		//<FieldNum(VInt32)>
				tvvOff = m_pFIDInput->readVLong();	//<FieldPositions(VInt64)>
				pFieldInfo = m_pFieldsInfo->getField(f);
				if(pFieldInfo && pFieldInfo->isPositionVector())
				{
					m_pFIVInput->seek(tvvOff);
					pPosVector = m_pFieldFactories->getFactory(pFieldInfo->getID())->createTermPositionVector();
					pPosVector->open(pFieldInfo->getName(),m_pFIVInput);
					if(pPosVector)
						freqVectors.push_back(pPosVector);
				}				
				else 
				{
					m_pFIDInput->readVLong();//skip
				}
			}

			if(freqVectors.size() > 0)
				return new CForwardIndexArray<CTermPositionVector*>(&freqVectors);		
			return NULL;
		}

		CTermSequenceVector* CForwardIndexReader::getSquenceVector(docid_t did,const tchar* field)
		{
			fieldid_t fid = m_pFieldsInfo->getFieldID(field);
			if(fid == -1)
				return NULL;
			CFieldInfo* pFieldInfo = m_pFieldsInfo->getField(fid);
			if(pFieldInfo->isSequenceVector() == false)
				return NULL;			

			m_pFIMInput->seek(did*sizeof(fileoffset_t));
			fileoffset_t tvdOff = m_pFIMInput->readLong();//<DocumentPosition(Int64)> 
			m_pFIDInput->seek(tvdOff);
			int32_t numFields = m_pFIDInput->readVInt();//<NumFields(VInt32)>
			fileoffset_t tvvOff = -1;
			fieldid_t	f;
			for(int32_t i = 0;i<numFields;i++)
			{
				f = m_pFIDInput->readVInt();	//<FieldNum(VInt32)>
				if(f == fid)
				{
					tvvOff = m_pFIDInput->readVLong();//<FieldPositions(VInt64)>
					break;
				}
				else 
				{
					m_pFIDInput->readVLong();//skip
				}
			}
			if(tvvOff == -1)
				return NULL;
			m_pFIVInput->seek(tvvOff);
			CTermSequenceVector* pSV = m_pFieldFactories->getFactory(pFieldInfo->getID())->createTermSequenceVector();
			pSV->open(field,m_pFIVInput);
			return pSV;
		}			
		CForwardIndexArray<CTermSequenceVector*>*	CForwardIndexReader::getSquenceVectors(docid_t did)
		{
			vector<CTermSequenceVector*> freqVectors;
			CTermSequenceVector* pSeqVector;

			CFieldInfo* pFieldInfo ;
			m_pFIMInput->seek(did*sizeof(fileoffset_t));
			fileoffset_t tvdOff = m_pFIMInput->readLong();	//<DocumentPosition(Int64)> 
			m_pFIDInput->seek(tvdOff);
			int32_t numFields = m_pFIDInput->readVInt();	//<NumFields(VInt32)>
			fileoffset_t tvvOff = -1;
			fieldid_t	f;
			for(int32_t i = 0;i<numFields;i++)
			{
				f = m_pFIDInput->readVInt();		//<FieldNum(VInt32)>
				tvvOff = m_pFIDInput->readVLong();	//<FieldPositions(VInt64)>
				pFieldInfo = m_pFieldsInfo->getField(f);
				if(pFieldInfo && pFieldInfo->isSequenceVector())
				{
					m_pFIVInput->seek(tvvOff);
					pSeqVector = m_pFieldFactories->getFactory(pFieldInfo->getID())->createTermSequenceVector();
					pSeqVector->open(pFieldInfo->getName(),m_pFIVInput);
					if(pSeqVector)
						freqVectors.push_back(pSeqVector);
				}				
				else 
				{
					m_pFIDInput->readVLong();//skip
				}
			}

			if(freqVectors.size() > 0)
				return new CForwardIndexArray<CTermSequenceVector*>(&freqVectors);		
			return NULL;
		}
	}
}

