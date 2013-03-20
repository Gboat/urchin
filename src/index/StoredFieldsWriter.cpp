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
// Created	: 2005/12/9
//
#include "index/StoredFieldsWriter.h"
#include "index/FieldsInfo.h"
#include "index/StoredFieldsReader.h"

namespace firtex
{
	namespace index
	{
		CStoredFieldsWriter::CStoredFieldsWriter(CDirectory* pDirectory,const tstring& barrel,CFieldsInfo* fieldsInfo)
			: m_pSFReader(NULL)
			, m_bDirty(false)
			, m_pDirectory(NULL)
			, m_pFieldValueWriter(NULL)
			, m_pFieldIndexWriter(NULL)
			, m_pFieldsInfo(NULL)
		{			
			open(pDirectory,barrel,fieldsInfo);
		}
		CStoredFieldsWriter::CStoredFieldsWriter()
			: m_pSFReader(NULL)
			, m_bDirty(false)
			, m_pDirectory(NULL)
			, m_sBarrel(NULL)
			, m_pFieldValueWriter(NULL)
			, m_pFieldIndexWriter(NULL)
			, m_pFieldsInfo(NULL)
		{
			
		}

		CStoredFieldsWriter::~CStoredFieldsWriter(void)
		{
			close();
		}
		void CStoredFieldsWriter::addDocument(document::CDocument* pDoc)
		{			
			fieldid_t fid  = 0;
			int fdCount = 0;
			CField* pField = NULL;

			m_bDirty = true;
			
			//Position of Stored Value
			m_pFieldIndexWriter->seek(pDoc->getDocID() * sizeof(fileoffset_t));
			m_pFieldIndexWriter->writeLong(m_pFieldValueWriter->getFilePointer());

			pDoc->startIterator();			
			while(pDoc->hasNext())
			{
				pField = pDoc->next();				
				if(pField->isStored())
				{
					fdCount ++;
				}				
			}
			//´æ´¢Óò¸öÊý
			m_pFieldValueWriter->writeVInt(fdCount);
			
			pDoc->startIterator();			
			while(pDoc->hasNext())
			{
				pField = pDoc->next();				
				if(pField->isStored())
				{					
					fid = pField->getID();
					m_pFieldValueWriter->writeVInt(fid);		//write fieldNum
					switch(pField->getType())
					{
					case CField::BINARY:
						{
							datarecord dr = pField->binaryValue();
							m_pFieldValueWriter->writeVInt((int32_t)dr.length);		//Stored Value Length
							m_pFieldValueWriter->write((const char*)dr.data,dr.length);	//write field Value
						}
						break;					
					case CField::READER:
						{
							CReader* pReader = pField->readerValue();
							size_t len = (size_t)pReader->length();
							m_pFieldValueWriter->writeVInt((int32_t)len);		//Stored Value Length

							char* data = pReader->readWithoutCopy(len);
							while (len > 0)
							{
								m_pFieldValueWriter->write(data,len);
								data = pReader->readWithoutCopy(len);
							}
						}
						break;
					case CField::TEXT:
						{
							CFXStringA* pText = pField->textAValue();	
							m_pFieldValueWriter->writeString((const CFXStringA&)*pText);		//Stored Value Length
						}
						break;					
					case CField::TEXTW:
						{				
							//TODO:
						}
						break;	
					case CField::NUMBER:
						{		
							numbervar nv = pField->numberValue();
							switch(nv.nt)
							{
							case CNumber::INT8:
								m_pFieldValueWriter->writeByte((byte)nv.value.i8val);
								break;
							case CNumber::UINT8:
								m_pFieldValueWriter->writeByte((byte)nv.value.ui8val);
								break;
							case CNumber::INT16:
								{
									byte b = (byte)(nv.value.i16val >> 8);
									m_pFieldValueWriter->writeByte(b);
									m_pFieldValueWriter->writeByte((byte)nv.value.i16val);
								}
								break;
							case CNumber::UINT16:
								{
									byte b = (byte)(nv.value.ui16val >> 8);
									m_pFieldValueWriter->writeByte(b);
									m_pFieldValueWriter->writeByte((byte)nv.value.ui16val);
								}
								break;
							case CNumber::INT32:
								m_pFieldValueWriter->writeVInt(nv.value.i32val);
								break;
							case CNumber::UINT32:
								m_pFieldValueWriter->writeVInt(nv.value.ui32val);
								break;
							case CNumber::INT64:
								m_pFieldValueWriter->writeVLong(nv.value.i64val);
								break;
							case CNumber::UINT64:
								m_pFieldValueWriter->writeVLong(nv.value.ui64val);
								break;
							case CNumber::FLOAT:
								m_pFieldValueWriter->writeVInt((int32_t)nv.value.fval);//right?
								break;
							case CNumber::DOUBLE:
								m_pFieldValueWriter->writeVLong((int64_t)nv.value.dbval);//right?
								break;
							default:
								FIRTEX_CLOG(level::warn) << _T("Wrong number type.(CStoredFieldsWriter::addDocument())") << FIRTEX_ENDL;
							    break;
							}
							//TODO:
						}
						break;	
					}
					
				}				
			}					
		}				

		document::CDocument* CStoredFieldsWriter::document(docid_t docID)
		{
			if(!m_pSFReader)
			{
				if(m_bDirty)
				{
					m_pFieldIndexWriter->flush();
					m_pFieldValueWriter->flush();
					m_bDirty = false;
				}
				m_pSFReader = new CStoredFieldsReader(m_pDirectory,m_sBarrel,m_pFieldsInfo);
			}
			else if(m_bDirty)
			{
				m_pSFReader->close();
				m_pFieldIndexWriter->flush();
				m_pFieldValueWriter->flush();
				m_bDirty = false;
				m_pSFReader->open(m_pDirectory,m_sBarrel,m_pFieldsInfo);
			}
			return m_pSFReader->document(docID);
		}
		document::CField* CStoredFieldsWriter::field(docid_t docID,fieldid_t fid)
		{
			if(!m_pSFReader)
			{
				m_pSFReader = new CStoredFieldsReader(m_pDirectory,m_sBarrel,m_pFieldsInfo);
			}
			else if(m_bDirty)
			{
				m_pSFReader->close();
				m_pFieldIndexWriter->flush();
				m_pFieldValueWriter->flush();
				m_bDirty = false;
				m_pSFReader->open(m_pDirectory,m_sBarrel,m_pFieldsInfo);
			}
			return m_pSFReader->field(docID,fid);
		}

		void CStoredFieldsWriter::open(CDirectory* pDirectory,const tstring& barrel,CFieldsInfo* fieldsInfo)
		{
			close();			
			m_pFieldValueWriter = pDirectory->createOutput(barrel + ".sfv");
			m_pFieldIndexWriter = pDirectory->createOutput(barrel + ".sfm");
			m_pFieldsInfo = fieldsInfo;
			m_pDirectory = pDirectory;
			m_sBarrel = barrel;
		}
		void CStoredFieldsWriter::close()
		{
			if(m_pFieldValueWriter)
			{
				m_pFieldValueWriter->close();
				delete m_pFieldValueWriter;
				m_pFieldValueWriter = NULL;
			}
			if(m_pFieldIndexWriter)
			{
				m_pFieldIndexWriter->close();
				delete m_pFieldIndexWriter;
				m_pFieldIndexWriter = NULL;
			}		
			if(m_pSFReader)
			{
				delete m_pSFReader;
				m_pSFReader = NULL;
			}
			m_pDirectory = NULL;
			m_pFieldsInfo = NULL;
		}		
	}
}

