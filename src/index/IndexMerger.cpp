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
// Email	: ruijieguo@software.ict.ac.cn ruijieguo@gmail.com
// Created	: 2005/12/9
//
#include "index/IndexMerger.h"
#include "utility/Config.h"
#include "index/DictHashMerger.h"
#include "store/FSIndexOutput.h"
#include "store/Directory.h"
#include "index/FieldFactory.h"
#include "index/IndexWriter.h"
#include "utility/BitVector.h"
#include "utility/SpeedProbe.h"
#include "utility/ParamParser.h"
#include <sstream>
#include <memory>

#define  MAX_BARREL_LEVEL 32

namespace firtex
{
	namespace index
	{

		//////////////////////////////////////////////////////////////////////////
		///CMergeBarrelEntry
		CMergeBarrelEntry::CMergeBarrelEntry(CDirectory* pDirectory,CBarrelInfo*	pBarrelInfo)
			:m_pDirectory(pDirectory)
			,m_pBarrelInfo(pBarrelInfo)
			,m_pFieldsInfo(NULL)
			,m_pDocFilter(NULL)				
			,m_bDeleteFilter(false)
		{				
		}		
		CMergeBarrelEntry::~CMergeBarrelEntry()
		{
			if(m_pFieldsInfo)
			{
				delete m_pFieldsInfo;
				m_pFieldsInfo = NULL;
			}				

			if(m_bDeleteFilter && m_pDocFilter)
			{
				delete m_pDocFilter;
			}				
			m_pBarrelInfo = NULL;
		}
		void CMergeBarrelEntry::load(bool bGC)
		{
			CIndexBarrelWriter* pWriter = pWriter = m_pBarrelInfo->getWriter();
			if(!m_pFieldsInfo)
			{				
				if(pWriter)
					m_pFieldsInfo = new CFieldsInfo(*(pWriter->getFieldsInfo()));
				else m_pFieldsInfo = new CFieldsInfo();
			}
			if(m_bDeleteFilter && m_pDocFilter)
			{
				delete m_pDocFilter;
			}
			m_pDocFilter = NULL;

			if(!pWriter)
			{
				CIndexInput* fdiStream = m_pDirectory->openInput(m_pBarrelInfo->getName() + _T(".fdi"));
				m_pFieldsInfo->read(fdiStream);///read field info
				delete fdiStream;
			}						
			if(bGC)
			{
				if(!pWriter)
				{
					tstring dname = m_pBarrelInfo->getName() + _T(".del");
					if(m_pDirectory->fileExists(dname))
					{
						CIndexInput* pDelInput = m_pDirectory->openInput(dname);
						m_pDocFilter = new CBitVector(pDelInput);
						if(m_pDocFilter->count() != m_pBarrelInfo->numDeletedDocs())
						{
							FIRTEX_CLOG(level::info) << _T("doc filter error") << FIRTEX_ENDL;
						}
						delete pDelInput;
						m_bDeleteFilter = true;
					}					
				}
				else 
				{
					m_pDocFilter = pWriter->getDocFilter();
					m_bDeleteFilter = false;
				}
			}
		}
		//////////////////////////////////////////////////////////////////////////
		///CIndexMerger
		CIndexMerger::CIndexMerger()
			:m_pDirectory(NULL)
			,m_buffer(NULL)
			,m_bufsize(0)
			,m_bForceGC(false)
			,m_pFieldFactories(NULL)
			,m_bBorrowedBuffer(false)
			,m_pMergeBarrels(NULL)
			,m_pSpeedProbe(NULL)
		{
			m_gcParam.type = 0;
		}
		CIndexMerger::CIndexMerger(CDirectory* pDirectory)
			:m_pDirectory(pDirectory)
			,m_buffer(NULL)
			,m_bufsize(0)
			,m_bForceGC(false)
			,m_pFieldFactories(NULL)
			,m_bBorrowedBuffer(false)
			,m_pMergeBarrels(NULL)
			,m_pSpeedProbe(NULL)
		{
			m_gcParam.type = 0;
		}
		CIndexMerger::CIndexMerger(CDirectory* pDirectory,char* buffer,size_t bufsize)
			:m_pDirectory(pDirectory)
			,m_buffer(buffer)
			,m_bufsize(bufsize)
			,m_bForceGC(false)
			,m_pFieldFactories(NULL)
			,m_bBorrowedBuffer(false)
			,m_pMergeBarrels(NULL)
			,m_pSpeedProbe(NULL)
		{
			m_gcParam.type = 0;
		}
		CIndexMerger::~CIndexMerger()
		{			
			if(m_pMergeBarrels)				
			{				
				m_pMergeBarrels->clear();
				delete m_pMergeBarrels;
				m_pMergeBarrels = NULL;
			}			
			if(m_pSpeedProbe)
			{
				delete m_pSpeedProbe;
				m_pSpeedProbe = NULL;
			}
		}

		void CIndexMerger::setBuffer(char* buffer,size_t length)
		{
			m_buffer = buffer;
			m_bufsize = length;
		}

		void CIndexMerger::merge(CBarrelsInfo* pBarrelsInfo)
		{			
			if(!pBarrelsInfo || pBarrelsInfo->getBarrelCount() <= 1)
			{
				pendingUpdate(pBarrelsInfo);
				return ;
			}			
			
			m_pBarrelsInfo = pBarrelsInfo;

			CMergeBarrel mb(pBarrelsInfo->getBarrelCount());
			///put all index barrel into mb
			pBarrelsInfo->startIterator();
			CBarrelInfo* pBaInfo;
			while (pBarrelsInfo->hasNext())
			{				
				pBaInfo = pBarrelsInfo->next();
				
				mb.put(new CMergeBarrelEntry(m_pDirectory,pBaInfo));
			}			

			while (mb.size() > 0)
			{
				addBarrel(mb.pop());
			}						

			endMerge();
			pendingUpdate(pBarrelsInfo); ///update barrel name and base doc id
			m_pBarrelsInfo = NULL;

			if(m_bBorrowedBuffer)
			{
				///the buffer is borrowed from indexer, give it back to indexer
				setBuffer(NULL,0);
				m_bBorrowedBuffer = false;
			}			
			pBarrelsInfo->write(m_pDirectory);
		}

		void CIndexMerger::addToMerge(CBarrelsInfo* pBarrelsInfo,CBarrelInfo* pBarrelInfo)
		{			
			if(!m_pMergeBarrels)
			{
				m_pMergeBarrels = new vector<CMergeBarrelEntry*>();
			}									

			CMergeBarrelEntry* pEntry = NULL;
			m_pBarrelsInfo = pBarrelsInfo;			
			pEntry = new CMergeBarrelEntry(m_pDirectory,pBarrelInfo);
			m_pMergeBarrels->push_back(pEntry);
			addBarrel(pEntry);

			pendingUpdate(pBarrelsInfo); ///update barrel name and base doc id
			m_pBarrelsInfo = NULL;

			if(m_bBorrowedBuffer)
			{
				///the buffer is borrowed from indexer, give it back to indexer
				setBuffer(NULL,0);
				m_bBorrowedBuffer = false;
			}			
			pBarrelsInfo->write(m_pDirectory);
		}

		void CIndexMerger::pendingUpdate(CBarrelsInfo* pBarrelsInfo)
		{
			///TODO:LOCK
			///sort barrels
			pBarrelsInfo->sort(m_pDirectory);
			CBarrelInfo* pBaInfo;
			pBarrelsInfo->startIterator();
			while(pBarrelsInfo->hasNext())
			{
				pBaInfo = pBarrelsInfo->next();				
				pBaInfo->setWriter(NULL);///clear writer
			}			
			///TODO:UNLOCK
		}
		void CIndexMerger::continueDocIDs(CBarrelsInfo* pBarrelsInfo)
		{
			docid_t baseDocID = 0;
			CBarrelInfo* pBaInfo;			
			pBarrelsInfo->startIterator();
			while (pBarrelsInfo->hasNext())
			{
				pBaInfo = pBarrelsInfo->next();
				pBaInfo->setMinDocID(baseDocID);	///update base document id
				baseDocID += pBaInfo->getDocCount();				
			}
		}		
		
		void CIndexMerger::mergeBarrel(CMergeBarrel* pBarrel)
		{			
			//////////////////////////////////////////////////////////////////////////
			if(!m_pSpeedProbe)
				m_pSpeedProbe = new CSpeedProbe;			
			FIRTEX_CLOG(level::info) << _T("Begin merge: ") << FIRTEX_END;
			CBarrelInfo* pBaInfo;
			for(int32_t i = 0;i < (int32_t)pBarrel->size();i++)
			{
				pBaInfo = pBarrel->getAt(i)->m_pBarrelInfo;
				FIRTEX_CLOG(level::info) << _T("\t") << i << _T(":") << pBaInfo->getDocCount() << FIRTEX_END;
				if(pBaInfo->numDeletedDocs() > 0)
					FIRTEX_CLOG(level::info) << _T("(") << pBaInfo->numDeletedDocs() << _T(")") << FIRTEX_END;
			}
			FIRTEX_CLOG(level::info) << FIRTEX_ENDL;
			m_pSpeedProbe->begin();
			//////////////////////////////////////////////////////////////////////////
			bool bGC = m_bForceGC;
			if(bGC == false)
				bGC = isGC(pBarrel);
			pBarrel->load(bGC);

			tstring newBarrelName = pBarrel->getIdentifier();
			CBarrelInfo* pNewBarrelInfo = new CBarrelInfo(newBarrelName,0,0);

			tstring name = newBarrelName + _T(".voc");///the file name of new index barrel
			CIndexOutput* pVocStream = m_pDirectory->createOutput(name);
			name = newBarrelName + _T(".dfp");
			CIndexOutput* pDStream = m_pDirectory->createOutput(name);
			name = newBarrelName + _T(".pop");
			CIndexOutput* pPStream = m_pDirectory->createOutput(name);

			COutputDescriptor* pOutputDesc = new COutputDescriptor(pVocStream,pDStream,pPStream,true);
			
			int32_t nEntry;	
			CMergeBarrelEntry* pEntry = NULL;
			count_t nNumDocs = 0;	
			count_t nNumDelDocs = 0;
			int32_t nEntryCount = (int32_t)pBarrel->size();
			///update min doc id of index barrels,let doc id continuous
			for(nEntry = 0;nEntry < nEntryCount;nEntry++)
			{
				pEntry = pBarrel->getAt(nEntry);				
				pEntry->m_pBarrelInfo->setMinDocID(nNumDocs);
				nNumDocs += pEntry->m_pBarrelInfo->getDocCount();				
				nNumDelDocs += pEntry->m_pBarrelInfo->numDeletedDocs();
			}	
			
			if(bGC)
			{
				pNewBarrelInfo->setDocCount(nNumDocs - nNumDelDocs);
			}
			else 
			{
				pNewBarrelInfo->setDocCount(nNumDocs);
				pNewBarrelInfo->setDeletedDocs(nNumDelDocs);
			}
						
			CFieldsInfo fieldsInfo;			
			CFieldInfo* pFieldInfo = NULL;
			CFieldMerger* pFieldMerger = NULL;			
			fieldid_t fieldid = 0;
			bool bFinish = false;
			bool bHasPPosting = false;
			
			fileoffset_t vocOff1,vocOff2,dfiOff1,dfiOff2,ptiOff1 = 0,ptiOff2 = 0;
			fileoffset_t voffset = 0;
			while (!bFinish)
			{
				for(nEntry = 0;nEntry < nEntryCount;nEntry++)
				{
					pEntry = pBarrel->getAt(nEntry);
					
					if(pEntry->m_pFieldsInfo->numFields() > fieldid)
					{
						pFieldInfo = pEntry->m_pFieldsInfo->getField(fieldid);///get field information
						if(pFieldInfo)
						{
							if(pFieldInfo->isIndexed())///it's a index field
							{
								if(pFieldMerger == NULL)
								{
									pFieldMerger = m_pFieldFactories->getFactory(fieldid)->createMerger();
									pFieldMerger->setDirectory(m_pDirectory);
								}

								pFieldMerger->addField(pEntry->m_pBarrelInfo,pFieldInfo,pEntry->m_pDocFilter);///add to field merger
							}						
						}				
					}
				}				

				if(pFieldInfo)
				{
					if(pFieldMerger && pFieldMerger->numFields() > 0)
					{	
						vocOff1 = pVocStream->getFilePointer();
						dfiOff1 = pDStream->getFilePointer();
						if(pOutputDesc->getPPostingOutput())
							ptiOff1 = pOutputDesc->getPPostingOutput()->getFilePointer();							

						pFieldMerger->setBuffer(m_buffer,m_bufsize);		///set buffer for field merge
						voffset = pFieldMerger->merge(pOutputDesc);
						pFieldInfo->setIndexOffset(voffset);///set offset of this field's index data

						vocOff2 = pVocStream->getFilePointer();
						dfiOff2 = pDStream->getFilePointer();
						ptiOff2 = pOutputDesc->getPPostingOutput()->getFilePointer();
						pFieldInfo->setDistinctNumTerms(pFieldMerger->numMergedTerms());

						FIRTEX_ASSERT(((vocOff2-vocOff1)>=0),_T("tdi length out of range."));
						FIRTEX_ASSERT(((dfiOff2-dfiOff1)>=0),_T("dfi length out of range."));
						pFieldInfo->setLength(vocOff2-vocOff1,dfiOff2-dfiOff1,ptiOff2-ptiOff1);
						if( (bHasPPosting == false) && ((ptiOff2 - ptiOff1) > 0))
							bHasPPosting = true;

						delete pFieldMerger;
						pFieldMerger = NULL;
					}
					fieldsInfo.addField(pFieldInfo);
					fieldid++;
					pFieldInfo = NULL;
				}
				else
				{
					bFinish = true;
				}								
			}						

			///close files of index writer
			for(nEntry = 0;nEntry < nEntryCount;nEntry++)
			{										
				pEntry = pBarrel->getAt(nEntry);
				CIndexBarrelWriter* pWriter = pEntry->m_pBarrelInfo->getWriter();
				if(pWriter)///clear in-memory index
				{
					pWriter->closeFiles();
				}
			}

			///merge stored fields
			if(bGC)
				mergeStoredFields_GC(pBarrel);
			else 
				mergeStoredFields(pBarrel);
			///merge norms data
			if(bGC)
				mergeNorms_GC(pBarrel);
			else 
				mergeNorms(pBarrel);
			//merge bitmap of deleted documents
			mergeDeletedDocs(pBarrel,bGC);

			//merge term vector
			bool bHasTermVector = false;
			for(nEntry = 0;nEntry < nEntryCount;nEntry++)
			{
				CFieldInfo* pFieldInfo;							
				pEntry = pBarrel->getAt(nEntry);
				pEntry->m_pFieldsInfo->startIterator();
				while(pEntry->m_pFieldsInfo->hasNext())
				{
					pFieldInfo = pEntry->m_pFieldsInfo->next();
					if(pFieldInfo->isStoreForwardIndex())
					{
						bHasTermVector = true;
						break;
					}
				}								
			}
			if(bHasTermVector == true)
			{
				if(bGC)
					mergeTermVector_GC(pBarrel);
				else 
					mergeTermVector(pBarrel);
			}

			//deleted all merged barrels
			///TODO:LOCK 
			for(nEntry = 0;nEntry < nEntryCount;nEntry++)
			{										
				pEntry = pBarrel->getAt(nEntry);
				CIndexBarrelWriter* pWriter = pEntry->m_pBarrelInfo->getWriter();
				if(pWriter)///clear in-memory index
				{
					pWriter->resetCache(true);
					///borrow buffer from indexer
					setBuffer((char*)pWriter->getMemCache()->getBegin(),pWriter->getMemCache()->getSize());
					m_bBorrowedBuffer = true;
				}
				m_pBarrelsInfo->removeBarrel(m_pDirectory,pEntry->m_pBarrelInfo->getName());///delete merged barrels
			}
			m_pBarrelsInfo->addBarrel(pNewBarrelInfo,false);
			continueDocIDs(m_pBarrelsInfo);///let doc ids in a continuous form
			///TODO:UNLOCK 
			if(m_pMergeBarrels)
			{
				removeMergedBarrels(pBarrel);
			}
			pBarrel->clear();

			name = newBarrelName + _T(".fdi");
			CIndexOutput* fieldsStream = m_pDirectory->createOutput(name);
			fieldsInfo.write(fieldsStream);//field information			
			delete fieldsStream;

			if(bHasPPosting == false)
			{
				name = newBarrelName + _T(".pop");
				m_pDirectory->deleteFile(name);
			}

			delete pOutputDesc;
			pOutputDesc = NULL;

			//////////////////////////////////////////////////////////////////////////			
			m_pSpeedProbe->end();
			FIRTEX_CLOG(level::info) << _T("End merge: ") << pNewBarrelInfo->getDocCount() << FIRTEX_END;
			if(pNewBarrelInfo->numDeletedDocs() > 0)
				FIRTEX_CLOG(level::info) << _T("(") << pNewBarrelInfo->numDeletedDocs() << _T(")") << FIRTEX_END;
			FIRTEX_CLOG(level::info) << _T(" Time:") << m_pSpeedProbe->timeSpan() << _T("(Total:") << m_pSpeedProbe->totalTimeSpan() << _T(")") << _T(" s") << FIRTEX_ENDL;

			//////////////////////////////////////////////////////////////////////////			
			
			/*FIRTEX_CLOG(level::info) << _T(".sfm length:") << m_pDirectory->fileLength(pNewBarrelInfo->getName() + _T(".sfm")) << FIRTEX_ENDL;
			FIRTEX_CLOG(level::info) << _T(".n1 length:") << m_pDirectory->fileLength(pNewBarrelInfo->getName() + _T(".n1")) << FIRTEX_ENDL;*/
			
			/*if(m_pDirectory->fileLength(pNewBarrelInfo->getName() + _T(".sfm")) < pNewBarrelInfo->getDocCount()*8)
			{
				FIRTEX_CLOG(level::info) << _T("Stored field merge error:");				

			}
			if(m_pDirectory->fileLength(pNewBarrelInfo->getName() + _T(".n1")) < pNewBarrelInfo->getDocCount()*4)
			{
				FIRTEX_CLOG(level::info) << _T("norm merge error") << FIRTEX_ENDL;			
			}*/

			CMergeBarrelEntry* pNewEntry = new CMergeBarrelEntry(m_pDirectory,pNewBarrelInfo);
			addBarrel(pNewEntry);			
		}

		void CIndexMerger::mergeStoredFields(CMergeBarrel* pBarrel)
		{
			tstring newBarrelName = pBarrel->getIdentifier();
			tstring name = newBarrelName + _T(".sfm");

			CIndexOutput* pSFMWriter = m_pDirectory->createOutput(name);			
			name = newBarrelName + _T(".sfv");
			CIndexOutput* pSFVWriter = m_pDirectory->createOutput(name);

			int32_t nBarrel = 0;
			fileoffset_t nOffset = 0;
			fileoffset_t nFIPos = 0;
			CMergeBarrelEntry* pEntry = NULL;			
			for(int32_t nEntry = 0;nEntry < (int32_t)pBarrel->size();nEntry++)
			{
				pEntry = pBarrel->getAt(nEntry);
				
				CIndexInput* pSFMReader =  m_pDirectory->openInput(pEntry->m_pBarrelInfo->getName() + _T(".sfm"));
				CIndexInput* pSFVReader = m_pDirectory->openInput(pEntry->m_pBarrelInfo->getName() + _T(".sfv"));
				if(nBarrel == 0)
				{
					pSFMWriter->write(pSFMReader,pSFMReader->length());
					pSFVWriter->write(pSFVReader,pSFVReader->length());					
				}
				else 
				{
					while (!pSFMReader->isEof()) 
					{
						nFIPos = pSFMReader->readLong();
						if(nFIPos != -1)
						{
							nFIPos += nOffset;
							pSFMWriter->writeLong(nFIPos);
						}
						else 
						{
							pSFMWriter->writeLong(nFIPos);
						}
					}					
					pSFVWriter->write(pSFVReader,pSFVReader->length());
				}
				nOffset += pSFVReader->length();

				pSFMReader->close();
				delete pSFMReader;
				pSFVReader->close();
				delete pSFVReader;
				nBarrel ++;				
			}

			pSFMWriter->close();
			delete pSFMWriter;
			pSFVWriter->close();
			delete pSFVWriter;
			
		}
		void CIndexMerger::mergeStoredFields_GC(CMergeBarrel* pBarrel)
		{
			tstring newBarrelName = pBarrel->getIdentifier();
			tstring name = newBarrelName + _T(".sfm");

			CIndexOutput* pSFMWriter = m_pDirectory->createOutput(name);			
			name = newBarrelName + _T(".sfv");
			CIndexOutput* pSFVWriter = m_pDirectory->createOutput(name);

			int32_t				nBarrel = 0;			
			fileoffset_t		nSFPos = 0;
			fileoffset_t		nSFPosPre = -1;
			size_t				nCurDoc = 0;
			docid_t				nIDFreed = 0;
			CMergeBarrelEntry*	pEntry = NULL;			
			CBitVector*			pFilter = NULL;
			for(int32_t nEntry = 0;nEntry < (int32_t)pBarrel->size();nEntry++)
			{
				pEntry = pBarrel->getAt(nEntry);
				pFilter = pEntry->m_pDocFilter;

				CIndexInput* pSFMReader =  m_pDirectory->openInput(pEntry->m_pBarrelInfo->getName() + _T(".sfm"));
				CIndexInput* pSFVReader = m_pDirectory->openInput(pEntry->m_pBarrelInfo->getName() + _T(".sfv"));
				nCurDoc = 0;
				nSFPosPre = -1;				
				
				if( (nBarrel ==0) && (!pFilter || pFilter->none()) )
				{
					pSFMWriter->write(pSFMReader,pSFMReader->length());
					pSFVWriter->write(pSFVReader,pSFVReader->length());
					//nCurDoc += (size_t)(pSFMReader->length()/sizeof(fileoffset_t));
				}
				else 
				{	
					if(pFilter)
					{
						nIDFreed = (docid_t)pFilter->firstNotSet();
						if(nIDFreed > 0)
						{
							pSFMReader->seek(nIDFreed * sizeof(fileoffset_t)); ///skip deleted documents						
						}
						nCurDoc += nIDFreed;

						while (!pSFMReader->isEof()) 
						{
							nSFPos = pSFMReader->readLong();
							if((nSFPosPre != -1) && ((nSFPos - nSFPosPre) > 0)) ///write stored field data of previous undeleted document
							{
								pSFVReader->seek(nSFPosPre);
								pSFVWriter->write(pSFVReader,(size_t)(nSFPos - nSFPosPre));
							}
							if(!pFilter->test(nCurDoc))///this document has not been deleted
							{					
								if(nSFPos != -1)
									pSFMWriter->writeLong(pSFVWriter->getFilePointer());
								else 
									pSFMWriter->writeLong(nSFPos);
								nSFPosPre = nSFPos;
							}
							else ///this document has been deleted
							{
								pSFMWriter->writeLong(-1);
								nSFPosPre = -1;
							}
							nCurDoc++;
						}	
						nSFPos = pSFVReader->length();
						if((nSFPosPre != -1) && ((nSFPos - nSFPosPre) > 0)) ///write stored field data of previous undeleted document
						{
							pSFVReader->seek(nSFPosPre);
							pSFVWriter->write(pSFVReader,(size_t)(nSFPos - nSFPosPre));
						}
						nSFPosPre = -1;
					}	
					else 
					{
						fileoffset_t nOffset = pSFVWriter->getFilePointer();
						while (!pSFMReader->isEof()) 
						{							
							nSFPos = pSFMReader->readLong();
							if(nSFPos != -1)
							{
								nSFPos += nOffset;
								pSFMWriter->writeLong(nSFPos);
							}
							else 
							{
								pSFMWriter->writeLong(nSFPos);
							}
						}					
						pSFVWriter->write(pSFVReader,pSFVReader->length());
					}
				}											
				pSFMReader->close();
				delete pSFMReader;
				pSFVReader->close();
				delete pSFVReader;
				nBarrel ++;				
			}

			pSFMWriter->close();
			delete pSFMWriter;
			pSFVWriter->close();
			delete pSFVWriter;

		}

		void CIndexMerger::mergeNorms(CMergeBarrel* pBarrel)
		{
			tstring newBarrelName = pBarrel->getIdentifier();
			tstring name = newBarrelName;

			fieldid_t fid = 0;
			CIndexOutput* pNormsWriter;
			bool bFinish = true;
			CMergeBarrelEntry* pEntry;
			CFieldInfo* pFieldInfo;
			do
			{
				bFinish = true;
				pNormsWriter = NULL;				
				for(int32_t nEntry = 0;nEntry < (int32_t)pBarrel->size();nEntry++)
				{
					pEntry = pBarrel->getAt(nEntry);					

					if(pEntry->m_pFieldsInfo->numFields() > fid)
					{
						pFieldInfo = pEntry->m_pFieldsInfo->getField(fid);
						if(pFieldInfo && (pFieldInfo->isIndexed()))
						{
							if(pNormsWriter == NULL)
							{
								name = newBarrelName + _T(".n");
								name = firtex::string_utils::append(name,fid);											
								pNormsWriter = m_pDirectory->createOutput(name);
							}						
							name = pEntry->m_pBarrelInfo->getName() + _T(".n");
							name = firtex::string_utils::append(name,fid);						
							CIndexInput* pNormsReader = m_pDirectory->openInput(name);
							pNormsWriter->write(pNormsReader,pNormsReader->length());
							pNormsReader->close();
							delete pNormsReader;
						}
					}					
					if(pEntry->m_pFieldsInfo->numFields() > fid)
						bFinish = false;					
				}
				if(pNormsWriter)
				{
					pNormsWriter->close();
					delete pNormsWriter;
					pNormsWriter = NULL;
				}
				fid++;
			}while(!bFinish);
		}
		void CIndexMerger::mergeNorms_GC(CMergeBarrel* pBarrel)
		{
			tstring newBarrelName = pBarrel->getIdentifier();
			tstring name = newBarrelName;

			fieldid_t			fid = 0;
			CIndexOutput*		pNormsWriter;
			bool				bFinish = true;
			size_t				nCurDoc = 0;
			size_t				nTotalDocs = 0;
			CBitVector*			pFilter = NULL;
			fileoffset_t		nNormBeg = -1;
			docid_t				nIDFreed = 0;

			CMergeBarrelEntry*	pEntry;
			CFieldInfo*			pFieldInfo;
			do
			{
				bFinish = true;
				pNormsWriter = NULL;				
				for(int32_t nEntry = 0;nEntry < (int32_t)pBarrel->size();nEntry++)
				{
					pEntry = pBarrel->getAt(nEntry);					
					pFilter = pEntry->m_pDocFilter;
					nCurDoc = 0;
					nNormBeg = -1;
					if(pEntry->m_pFieldsInfo->numFields() > fid)
					{
						pFieldInfo = pEntry->m_pFieldsInfo->getField(fid);
						if(pFieldInfo && (pFieldInfo->isIndexed()))
						{
							if(pNormsWriter == NULL)
							{
								name = newBarrelName + _T(".n");
								name = firtex::string_utils::append(name,fid);											
								pNormsWriter = m_pDirectory->createOutput(name);
							}						
							name = pEntry->m_pBarrelInfo->getName() + _T(".n");
							name = firtex::string_utils::append(name,fid);						
							CIndexInput* pNormsReader = m_pDirectory->openInput(name);

							if(!pFilter || pFilter->none())
							{
								pNormsWriter->write(pNormsReader,pNormsReader->length());
							}
							else 
							{								
								nIDFreed = (docid_t)pFilter->firstNotSet();
								if(nIDFreed > 0)
								{
									pNormsReader->seek(nIDFreed * sizeof(norm_t)); ///skip deleted documents						
								}
								nCurDoc += nIDFreed;
								
								nTotalDocs = (size_t)(pNormsReader->length()/sizeof(norm_t));
								while (nCurDoc < nTotalDocs)
								{
									if(pFilter->test(nCurDoc)) ///this document has been deleted
									{
										if(nNormBeg >= 0)
										{
											pNormsReader->seek(nNormBeg*sizeof(norm_t));
											pNormsWriter->write(pNormsReader,(size_t)(nCurDoc - nNormBeg)*sizeof(norm_t));
											nNormBeg = -1;
										}

										if(sizeof(norm_t) == 4)
											pNormsWriter->writeInt(0);
										else if(sizeof(norm_t) == 1) 
											pNormsWriter->writeByte(0);													
									}
									else ///this document has not been deleted
									{
										if(nNormBeg == -1)
											nNormBeg = nCurDoc;
									}
									nCurDoc++;
								}
								if(nNormBeg >= 0)
								{
									pNormsReader->seek(nNormBeg*sizeof(norm_t));
									pNormsWriter->write(pNormsReader,(size_t)(nCurDoc - nNormBeg)*sizeof(norm_t));
									nNormBeg = -1;
								}																
							}
							pNormsReader->close();
							delete pNormsReader;
						}
					}					
					if(pEntry->m_pFieldsInfo->numFields() > fid)
						bFinish = false;					
				}
				if(pNormsWriter)
				{
					pNormsWriter->close();
					delete pNormsWriter;
					pNormsWriter = NULL;
				}
				fid++;
			}while(!bFinish);
		}
		void CIndexMerger::mergeDeletedDocs(CMergeBarrel* pBarrel,bool bGC)
		{			
			CIndexOutput* delWriter = NULL;
			CIndexInput* delReader = NULL;
			try
			{
				CBitVector bMerged;				
				CBarrelInfo* pBaInfo = NULL;
				tstring sDel;
				count_t nNumNotSet = 0; 
				for(int32_t nEntry = 0;nEntry < (int32_t)pBarrel->size();nEntry++)
				{					
					pBaInfo = pBarrel->getAt(nEntry)->m_pBarrelInfo;
					sDel = pBaInfo->getName() + _T(".del");					
					if(m_pDirectory->fileExists(sDel))
					{
						if(bGC)
						{
							m_pDirectory->deleteFile(sDel);
						}
						else 
						{
							delReader =  m_pDirectory->openInput(pBaInfo->getName() + _T(".del"));
							CBitVector b(delReader);
							b.length(pBaInfo->getDocCount());
							if(nNumNotSet > 0)
							{
								bMerged.length(bMerged.length() + nNumNotSet);
								nNumNotSet = 0;
							}
							bMerged += b;
							delete delReader;
							delReader = NULL;
						}
					}
					else 
					{
						if(bGC == false)
							nNumNotSet += pBaInfo->getDocCount();
					}
				}
				if(bMerged.any())
				{
					tstring newBarrelName = pBarrel->getIdentifier();
					newBarrelName += _T(".del");					
					
					delWriter = m_pDirectory->createOutput(newBarrelName);
					bMerged.write(delWriter);
					delete delWriter;
					delWriter = NULL;
				}				
			}			
			catch (CFirteXException& fe) 
			{
				if(delWriter)
					delete delWriter;
				if(delReader)
					delete delWriter;
				FIRTEX_RETHROW(fe);
			}
			catch (...) 
			{
				if(delWriter)
					delete delWriter;
				if(delReader)
					delete delWriter;
				FIRTEX_THROW3(UNKNOWN_ERROR,"void CIndexMerger::mergeDeletedDocs(CMergeBarrel* pBarrel,bool bGC).");
			}
		}
		void CIndexMerger::mergeTermVector(CMergeBarrel* pBarrel)
		{
			tstring newBarrelName = pBarrel->getIdentifier();
			tstring name = newBarrelName + _T(".fim");

			try
			{
				CIndexOutput* pTVIWriter = m_pDirectory->createOutput(name);			
				name = newBarrelName + _T(".fid");
				CIndexOutput* pTVDWriter = m_pDirectory->createOutput(name);
				name = newBarrelName + _T(".fiv");
				CIndexOutput* pTVVWriter = m_pDirectory->createOutput(name);

				std::auto_ptr<CIndexOutput> tviWriterPtr(pTVIWriter);
				std::auto_ptr<CIndexOutput> tvdWriterPtr(pTVDWriter);
				std::auto_ptr<CIndexOutput> tvvWriterPtr(pTVVWriter);

				CIndexInput* pTVIReader = NULL;
				CIndexInput* pTVDReader = NULL;
				CIndexInput* pTVVReader = NULL;

				int32_t			nBarrel = 0;
				fileoffset_t	nOffset = 0;
				fileoffset_t	nFIPos = 0;				
				int32_t numFields,fieldNum;
				CMergeBarrelEntry* pEntry = NULL;
				for(int32_t nEntry = 0;nEntry < (int32_t)pBarrel->size();nEntry++)
				{
					pEntry = pBarrel->getAt(nEntry);

					pTVIReader =  m_pDirectory->openInput(pEntry->m_pBarrelInfo->getName() + _T(".fim"));
					pTVDReader = m_pDirectory->openInput(pEntry->m_pBarrelInfo->getName() + _T(".fid"));
					pTVVReader = m_pDirectory->openInput(pEntry->m_pBarrelInfo->getName() + _T(".fiv"));

					std::auto_ptr<CIndexInput> tviReaderPtr(pTVIReader);
					std::auto_ptr<CIndexInput> tvdReaderPtr(pTVDReader);
					std::auto_ptr<CIndexInput> tvvReaderPtr(pTVVReader);

					if(nBarrel == 0)
					{
						pTVIWriter->write(pTVIReader,pTVIReader->length());
						pTVDWriter->write(pTVDReader,pTVDReader->length());
						pTVVWriter->write(pTVVReader,pTVVReader->length());
					}
					else 
					{
						while (!pTVIReader->isEof()) 
						{
							tviWriterPtr->writeLong(tvdWriterPtr->getFilePointer());	//tvi.DocumentPosition(Int64)
							nFIPos = pTVIReader->readLong();

							pTVDReader->seek(nFIPos);
							numFields = pTVDReader->readVInt();					//tvd.NumFields(VInt32)
							tvdWriterPtr->writeVInt(numFields);				
							for(int32_t i = 0;i<numFields;i++)
							{
								fieldNum = pTVDReader->readVInt();				//tvd.FieldNum(VInt32)
								pTVDWriter->writeVInt(fieldNum);
								nFIPos = pTVDReader->readVLong();					//tvd.FieldPositions(VInt64)
								nFIPos += nOffset;
								pTVDWriter->writeVLong(nFIPos);
							}																		
						}					
						pTVVWriter->write(pTVVReader,pTVVReader->length());
					}
					nOffset = pTVVWriter->length();

					nBarrel ++;				
				}				
			}			
			catch (...) 
			{
				FIRTEX_THROW3(FILEIO_ERROR,_T("CIndexMerger_Old.mergeTermVector() failed."));
			}	
		}		

		void CIndexMerger::mergeTermVector_GC(CMergeBarrel* pBarrel)
		{
			tstring newBarrelName = pBarrel->getIdentifier();
			tstring name = newBarrelName + _T(".fim");			            
			try
			{
				CIndexOutput* pTVIWriter = m_pDirectory->createOutput(name);			
				name = newBarrelName + _T(".fid");
				CIndexOutput* pTVDWriter = m_pDirectory->createOutput(name);
				name = newBarrelName + _T(".fiv");
				CIndexOutput* pTVVWriter = m_pDirectory->createOutput(name);

				std::auto_ptr<CIndexOutput> tviWriterPtr(pTVIWriter);
				std::auto_ptr<CIndexOutput> tvdWriterPtr(pTVDWriter);
				std::auto_ptr<CIndexOutput> tvvWriterPtr(pTVVWriter);

				CIndexInput*	pTVIReader = NULL;
				CIndexInput*	pTVDReader = NULL;
				CIndexInput*	pTVVReader = NULL;

				size_t			nCurDoc = 0;			
				CBitVector*		pFilter = NULL;
				docid_t			nIDFreed = 0;

				int32_t			nBarrel = 0;
				fileoffset_t	nFIPos = 0;
				fileoffset_t	nFIVPos = 0;
				fileoffset_t	nFIVPosPre = -1;
				int32_t numFields,fieldNum;
				CMergeBarrelEntry* pEntry = NULL;
				for(int32_t nEntry = 0;nEntry < (int32_t)pBarrel->size();nEntry++)
				{
					pEntry = pBarrel->getAt(nEntry);
					pFilter = pEntry->m_pDocFilter;

					pTVIReader =  m_pDirectory->openInput(pEntry->m_pBarrelInfo->getName() + _T(".fim"));
					pTVDReader = m_pDirectory->openInput(pEntry->m_pBarrelInfo->getName() + _T(".fid"));
					pTVVReader = m_pDirectory->openInput(pEntry->m_pBarrelInfo->getName() + _T(".fiv"));

					std::auto_ptr<CIndexInput> tviReaderPtr(pTVIReader);
					std::auto_ptr<CIndexInput> tvdReaderPtr(pTVDReader);
					std::auto_ptr<CIndexInput> tvvReaderPtr(pTVVReader);

					nCurDoc = 0;
					nFIVPosPre = -1;				
					if( (nBarrel == 0) && (!pFilter || pFilter->none()) )
					{
						pTVIWriter->write(pTVIReader,pTVIReader->length());
						pTVDWriter->write(pTVDReader,pTVDReader->length());
						pTVVWriter->write(pTVVReader,pTVVReader->length());
						nCurDoc += (size_t)(pTVIReader->length()/sizeof(fileoffset_t));
					}
					else 
					{
						if(pFilter)
						{
							nIDFreed = (docid_t)pFilter->firstNotSet();
							if(nIDFreed > 0)
							{
								pTVIReader->seek(nIDFreed * sizeof(fileoffset_t)); ///skip deleted documents						
							}
							nCurDoc += nIDFreed;

							while (!pTVIReader->isEof()) 
							{
								nFIPos = pTVIReader->readLong();								
								if(!pFilter->test(nCurDoc)) ///this document has not been deleted
								{
									tviWriterPtr->writeLong(tvdWriterPtr->getFilePointer());	///tvi.DocumentPosition(Int64)								

									pTVDReader->seek(nFIPos);
									numFields = pTVDReader->readVInt();							///tvd.NumFields(VInt32)
									tvdWriterPtr->writeVInt(numFields);				
									for(int32_t i = 0;i<numFields;i++)
									{
										fieldNum = pTVDReader->readVInt();						///tvd.FieldNum(VInt32)
										pTVDWriter->writeVInt(fieldNum);
										nFIVPos = pTVDReader->readVLong();						///tvd.FieldPositions(VInt64)

										if((nFIVPosPre != -1) && ((nFIVPos - nFIVPosPre) > 0)) ///write data of previous undeleted document
										{
											pTVVReader->seek(nFIVPosPre);
											pTVVWriter->write(pTVVReader,(size_t)(nFIVPos - nFIVPosPre));
										}
										nFIVPosPre = nFIVPos;
										pTVDWriter->writeVLong(pTVVWriter->getFilePointer());										
									}																		
								}
								else 
								{
									tviWriterPtr->writeLong(-1);
									if((nFIVPosPre != -1) && ((nFIVPos - nFIVPosPre) > 0)) ///write data of previous undeleted document
									{
										pTVVReader->seek(nFIVPosPre);
										pTVVWriter->write(pTVVReader,(size_t)(nFIVPos - nFIVPosPre));
									}
									nFIVPosPre = -1;
								}																	
							}											
							if((nFIVPosPre != -1) && ((nFIVPos - nFIVPosPre) > 0)) ///write data of previous undeleted document
							{
								pTVVReader->seek(nFIVPosPre);
								pTVVWriter->write(pTVVReader,(size_t)(nFIVPos - nFIVPosPre));
							}
							nFIVPosPre = -1;
						}
						else 
						{
							fileoffset_t nOffset = pTVVWriter->getFilePointer();
							while (!pTVIReader->isEof()) 
							{
								tviWriterPtr->writeLong(tvdWriterPtr->getFilePointer());	//tvi.DocumentPosition(Int64)
								nFIPos = pTVIReader->readLong();

								pTVDReader->seek(nFIPos);
								numFields = pTVDReader->readVInt();					//tvd.NumFields(VInt32)
								tvdWriterPtr->writeVInt(numFields);				
								for(int32_t i = 0;i<numFields;i++)
								{
									fieldNum = pTVDReader->readVInt();				//tvd.FieldNum(VInt32)
									pTVDWriter->writeVInt(fieldNum);
									nFIPos = pTVDReader->readVLong();					//tvd.FieldPositions(VInt64)
									nFIPos += nOffset;
									pTVDWriter->writeVLong(nFIPos);
								}																		
							}					
							pTVVWriter->write(pTVVReader,pTVVReader->length());
						}
					}

					nBarrel ++;				
				}				
			}			
			catch (...) 
			{
				FIRTEX_THROW3(FILEIO_ERROR,_T("CIndexMerger_Old.mergeTermVector() failed."));
			}	
		}		

		void CIndexMerger::removeMergedBarrels(CMergeBarrel* pBarrel)
		{
			if(!m_pMergeBarrels)
				return;
			vector<CMergeBarrelEntry*>::iterator iter = m_pMergeBarrels->begin();
			size_t nEntry = 0;
			bool bRemoved = false;
			int32_t nRemoved = 0;
			while (iter != m_pMergeBarrels->end())
			{
				bRemoved = false;
				for(nEntry = 0;nEntry < pBarrel->size();nEntry++)
				{
					if((*iter) == pBarrel->getAt(nEntry))
					{
						iter = m_pMergeBarrels->erase(iter);
						bRemoved = true;
						nRemoved++;
					}
				}
				if(nRemoved == pBarrel->size())
					break;
				if(!bRemoved)
					iter++;
			}
		}
		void CIndexMerger::transferToDisk(const tchar* pszBarrelName)
		{
			if(!m_pMergeBarrels)
				return;
			vector<CMergeBarrelEntry*>::iterator iter = m_pMergeBarrels->begin();
			CMergeBarrelEntry* pEntry = NULL;
			while (iter != m_pMergeBarrels->end())
			{
				if(!_tcscmp((*iter)->m_pBarrelInfo->getName().c_str(),pszBarrelName))
				{
					pEntry = (*iter);					
					pEntry->load(m_bForceGC && (pEntry->m_pDocFilter != NULL));
					break;
				}
				iter++;
			}
		}

		void CIndexMerger::setThreshold(const tchar* pszThreshold)
		{
			if(pszThreshold && *pszThreshold)
			{		
				CParamParser pparser;
				if(pparser.parse(pszThreshold))
				{
					float fv;
					int32_t n;
					if(pparser.getParam(_T("r"),fv))
					{
						m_gcParam.type = 1;
						m_gcParam.fThreshold = fv;					
					}
					else if(pparser.getParam(_T("n"),n))
					{
						m_gcParam.type = 2;
						m_gcParam.nThreshold = n;
					}					
				}
			}
		}

		bool CIndexMerger::isGC(CMergeBarrel* pBarrel)
		{
			
			if(!pBarrel)
			{
				return (m_gcParam.type != 0);
			}

			count_t nDelDocs = 0;
			count_t nTotalDocs = 0;
			CBarrelInfo* pBarrelInfo;
			for (size_t i = 0;i < pBarrel->size(); i++)
			{
				pBarrelInfo = pBarrel->getAt(i)->m_pBarrelInfo;
				nDelDocs += pBarrelInfo->numDeletedDocs();
				nTotalDocs += pBarrelInfo->getDocCount();
			}
			
			if(m_gcParam.type == 1)
			{
				if( ((float)((float)nDelDocs/(float)nTotalDocs)) >= m_gcParam.fThreshold)
					return true;
			}
			else if(m_gcParam.type == 2)
			{
				if( (nDelDocs > 0) && ((nDelDocs > m_gcParam.nThreshold)) )
					return true;
			}
			return false;
		}
	}
}
