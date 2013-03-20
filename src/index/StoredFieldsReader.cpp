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
// Created	: 2006/4/12
//
#include "index/StoredFieldsReader.h"
#include "index/FieldsInfo.h"

namespace firtex
{
	namespace index
	{
		CStoredFieldsReader::CStoredFieldsReader(CDirectory* pDirectory,const tstring& barrel,CFieldsInfo* fieldsInfo)
			: m_pFieldsInfo(fieldsInfo)
		{
			m_pFieldValueReader = pDirectory->openInput(barrel + _T(".sfv"));
			m_pFieldIndexReader = pDirectory->openInput(barrel + _T(".sfm"));						
		}

		CStoredFieldsReader::~CStoredFieldsReader(void)
		{
			if (m_pFieldValueReader) 
			{
				delete m_pFieldValueReader;
				m_pFieldValueReader = NULL;
			}
			if (m_pFieldIndexReader) 
			{			
				delete m_pFieldIndexReader;
				m_pFieldIndexReader = NULL;
			}			
		}
		document::CDocument* CStoredFieldsReader::document(docid_t docID)
		{		
			document::CDocument* pDoc = NULL;
			try
			{
				m_pFieldIndexReader->seek(docID * sizeof(fileoffset_t));
				fileoffset_t off = m_pFieldIndexReader->readLong();				
				m_pFieldValueReader->seek(off);
				fieldid_t fid;
				int32_t fieldCount = m_pFieldValueReader->readVInt();
				if(fieldCount <= 0)
					return NULL;
				CFieldInfo* pFieldInfo = NULL;
				CField* pField;
				byte*	data;
				int32_t		length;
				string	str;
				pDoc = new document::CDocument(m_pFieldsInfo->numFields());								
				for (int32_t i = 0;i<fieldCount;i++)
				{
					fid = m_pFieldValueReader->readVInt();
					pFieldInfo = m_pFieldsInfo->getField(fid);
					FIRTEX_ASSERT((pFieldInfo!=NULL),_T("invalid field id"));
					switch(pFieldInfo->getType())
					{
					case CField::BINARY:
						{
							length = m_pFieldValueReader->readVInt();
							data = new byte[length];
							m_pFieldValueReader->read((char*)data,length);
							pField = CField::Binary(pFieldInfo->getName(),data,length,false,false);
							pField->fromFlag(pFieldInfo->getFlag());							
							pField->setID(fid);
							pDoc->addField(pField);
							delete[] data;
						}
						break;
					case CField::TEXT:
						{
							m_pFieldValueReader->readString(str);
							pField = CField::Text(pFieldInfo->getName(),str.c_str());
							pField->fromFlag(pFieldInfo->getFlag());							
							pField->setID(fid);
							pDoc->addField(pField);
						}
						break;					
					case CField::TEXTW:
						{
							//TODO:
						}
						break;									
					case CField::READER:
						{
							//TODO:
						}
						break;
					default:
						if( (pFieldInfo->getType() & CField::NUMBER) == CField::NUMBER)
						{
							numbervar nv;
							nv.nt = (number_t)(pFieldInfo->getType() >> 4);
							switch(nv.nt)
							{
							case CNumber::INT8:
								nv.value.i8val = (int8_t)m_pFieldValueReader->readByte();
								break;
							case CNumber::UINT8:
								nv.value.ui8val = (uint8_t)m_pFieldValueReader->readByte();
								break;
							case CNumber::INT16:
								{
									byte b = m_pFieldValueReader->readByte();
									nv.value.i16val = (int16_t)( ((b & 0xFF) << 8)| m_pFieldValueReader->readByte());
								}
								break;
							case CNumber::UINT16:
								{
									byte b = m_pFieldValueReader->readByte();
									nv.value.ui16val = (uint16_t)( ((b & 0xFF) << 8)| m_pFieldValueReader->readByte());
								}
								break;
							case CNumber::INT32:
								nv.value.i32val = m_pFieldValueReader->readVInt();
								break;
							case CNumber::UINT32:
								nv.value.ui32val = (uint32_t)m_pFieldValueReader->readVInt();
								break;
							case CNumber::INT64:
								nv.value.i64val = m_pFieldValueReader->readVLong();
								break;
							case CNumber::UINT64:
								nv.value.ui64val = (uint64_t)m_pFieldValueReader->readVLong();
								break;
							case CNumber::FLOAT:
								nv.value.fval = (float)m_pFieldValueReader->readVInt();//right?
								break;
							case CNumber::DOUBLE:
								nv.value.dbval = (double)m_pFieldValueReader->readVLong();//right?
								break;
							default:
								nv.value.i64val = 0;
								FIRTEX_CLOG(level::warn) << _T("Wrong number type.(CStoredFieldsReader::document())") << FIRTEX_ENDL;
								break;
							}
							pField = CField::Keyword(pFieldInfo->getName(),nv);
							pField->fromFlag(pFieldInfo->getFlag());							
							pField->setID(fid);
							pDoc->addField(pField);
						}
						else 
						{
							//FIRTEX_THROW3(INDEX_COLLAPSE_ERROR,_T("Wrong field data type."));
							FIRTEX_CLOG(level::warn) << pFieldInfo->getName() << _T(":no data stored.") << FIRTEX_ENDL;
						}
						break;
					}
				}
				return pDoc;
			}
			catch (std::bad_alloc& e)
			{
				std::string s = e.what();
				s +=  "CStoredFieldsReader::document(docid_t docID)";
				FIRTEX_THROW2(OUTOFMEM_ERROR,s);
			}
		}		
		document::CField* CStoredFieldsReader::field(docid_t docID,fieldid_t fid)
		{
			document::CField* pField = NULL;
			try
			{
				m_pFieldIndexReader->seek(docID * sizeof(fileoffset_t));
				fileoffset_t off = m_pFieldIndexReader->readLong();				
				m_pFieldValueReader->seek(off);
				fieldid_t fidRead;
				int32_t fieldCount = m_pFieldValueReader->readVInt();
				if(fieldCount <= 0)
					return NULL;
				CFieldInfo* pFieldInfo = NULL;
				CField* pField;				
				byte*	data;
				int32_t		length;
				string	str;				
				for (int32_t i = 0;i<fieldCount;i++)
				{
					fidRead = m_pFieldValueReader->readVInt();
					if(fidRead != fid)
					{
						length = m_pFieldValueReader->readVInt();
						m_pFieldValueReader->seek(m_pFieldValueReader->getFilePointer() + length);//Ìø¹ý
						continue;
					}
					else //ÕÒµ½
					{
						pFieldInfo = m_pFieldsInfo->getField(fid);						
						switch(pFieldInfo->getType())
						{
						case CField::BINARY:
							{
								length = m_pFieldValueReader->readVInt();
								data = new byte[length];
								m_pFieldValueReader->read((char*)data,length);
								pField = CField::Binary(pFieldInfo->getName(),data,length,false,false);
								pField->fromFlag(pFieldInfo->getFlag());
								pField->setID(fid);								
								delete[] data;
							}
							break;
						case CField::TEXT:
							{
								m_pFieldValueReader->readString(str);
								pField = CField::Text(pFieldInfo->getName(),str.c_str());
								pField->fromFlag(pFieldInfo->getFlag());							
								pField->setID(fid);
							}
							break;					
						case CField::TEXTW:
							{
								//TODO:
							}
							break;					
						case CField::READER:
							{
								//TODO:
							}
							break;										
						default:
							if( (pFieldInfo->getFlag() & CField::NUMBER) == CField::NUMBER)
							{
								numbervar nv;
								nv.nt = (number_t)(pFieldInfo->getFlag() >> 4);
								switch(nv.nt)
								{
								case CNumber::INT8:
									nv.value.i8val = (int8_t)m_pFieldValueReader->readByte();
									break;
								case CNumber::UINT8:
									nv.value.ui8val = (uint8_t)m_pFieldValueReader->readByte();
									break;
								case CNumber::INT16:
									{
										byte b = m_pFieldValueReader->readByte();
										nv.value.i16val = (int16_t)( ((b & 0xFF) << 8)| m_pFieldValueReader->readByte());
									}
									break;
								case CNumber::UINT16:
									{
										byte b = m_pFieldValueReader->readByte();
										nv.value.ui16val = (uint16_t)( ((b & 0xFF) << 8)| m_pFieldValueReader->readByte());
									}
									break;
								case CNumber::INT32:
									nv.value.i32val = m_pFieldValueReader->readVInt();
									break;
								case CNumber::UINT32:
									nv.value.ui32val = (uint32_t)m_pFieldValueReader->readVInt();
									break;
								case CNumber::INT64:
									nv.value.i64val = m_pFieldValueReader->readVLong();
									break;
								case CNumber::UINT64:
									nv.value.ui64val = (uint64_t)m_pFieldValueReader->readVLong();
									break;
								case CNumber::FLOAT:
									nv.value.fval = (float)m_pFieldValueReader->readVInt();//right?
									break;
								case CNumber::DOUBLE:
									nv.value.dbval = (double)m_pFieldValueReader->readVLong();//right?
									break;
								default:
									nv.value.i64val = 0;
									FIRTEX_CLOG(level::warn) << _T("Wrong number type.(CStoredFieldsReader::document())") << FIRTEX_ENDL;
									break;
								}
								pField = CField::Keyword(pFieldInfo->getName(),nv);
								pField->fromFlag(pFieldInfo->getFlag());							
								pField->setID(fid);								
							}
							break;
						}
						return pField;
					}					
				}
				return NULL;
			}
			catch (std::bad_alloc& e)
			{
				std::string s = e.what();
				s +=  "CStoredFieldsReader::field(docid_t docID,fieldid_t fid)";
				FIRTEX_THROW2(OUTOFMEM_ERROR,s);
			}
		}
		void CStoredFieldsReader::open(CDirectory* pDirectory,const tstring& barrel,CFieldsInfo* fieldsInfo)
		{
			close();
			m_pFieldValueReader = pDirectory->openInput(barrel + ".sfv");
			m_pFieldIndexReader = pDirectory->openInput(barrel + ".sfm");
			m_pFieldsInfo = fieldsInfo;
		}
		void CStoredFieldsReader::close()
		{
			if(m_pFieldValueReader)
			{
				m_pFieldValueReader->close();
				delete m_pFieldValueReader;
				m_pFieldValueReader = NULL;
			}
			if(m_pFieldIndexReader)
			{
				m_pFieldIndexReader->close();
				delete m_pFieldIndexReader;
				m_pFieldIndexReader = NULL;
			}			
		}
	}
}

