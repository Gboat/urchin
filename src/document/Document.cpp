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
// Created	: 2005/11/20
// Modified	: 2006/10/15 add uri,encoding,status and so on.-by GuoRuijie
//

#include "document/Document.h"
#include "plugin/ComDocument.h"
#include "plugin/ComDocumentInput.h"
#include "index/IndexWriter.h"


using namespace firtex::utility;
using namespace firtex::index;


namespace firtex
{
	namespace document
	{	
		//////////////////////////////////////////////////////////////////////////
		///CDocumentInput
		CDocumentInput::CDocumentInput()
			:m_pPathInput(NULL)
			,m_numBufferInput(0)
			,m_numReaderInput(0)
			,m_ppBufferInput(NULL)
			,m_ppReaderInput(NULL)			
			,m_pInterface(NULL)
			,m_pDelayedSchemaID(NULL)
		{			
		}
		CDocumentInput::CDocumentInput(schemaid_t sid,const tchar* szDocPath)
			:m_numBufferInput(0)
			,m_numReaderInput(0)
			,m_ppBufferInput(NULL)
			,m_ppReaderInput(NULL)
			,m_pInterface(NULL)
			,m_pDelayedSchemaID(NULL)
		{			
			m_pPathInput = new PathInput(sid,szDocPath);
			if(sid == -1)
				m_pDelayedSchemaID = &(m_pPathInput->sid);
		}
		CDocumentInput::CDocumentInput(schemaid_t sid,char* _buffer,int32_t len)
			:m_numBufferInput(1)
			,m_numReaderInput(0)
			,m_pPathInput(NULL)
			,m_ppReaderInput(NULL)
			,m_pInterface(NULL)
			,m_pDelayedSchemaID(NULL)
		{			
			m_ppBufferInput = new BufferInput*[1];
			m_ppBufferInput[0] = new BufferInput(sid,_buffer,len);

			if(sid == -1)
				m_pDelayedSchemaID = &(m_ppBufferInput[0]->sid);
		}					
		CDocumentInput::~CDocumentInput()
		{
			if(m_pPathInput)
			{
				delete m_pPathInput;
				m_pPathInput = NULL;
			}
			if(m_ppBufferInput)
			{
				for (int32_t i = 0;i<m_numBufferInput;i++)
				{
					delete m_ppBufferInput[i];
				}
				delete[] m_ppBufferInput;
			}				
			if(m_ppReaderInput)
			{
				for (int32_t i = 0;i<m_numReaderInput;i++)
				{
					delete m_ppReaderInput[i];
				}
				delete[] m_ppReaderInput;
			}	
			if(m_pInterface)
			{
				m_pInterface->Release();
				m_pInterface = NULL;
			}
			m_pDelayedSchemaID = NULL;
		}		
		void CDocumentInput::reserveBuffers(int32_t _count)
		{
			if(_count == m_numBufferInput)
				return;
			BufferInput** ppBI = new BufferInput*[_count];
			memset(ppBI,0,_count*sizeof(BufferInput*));
			if(_count > m_numBufferInput)
			{
				memcpy(ppBI,m_ppBufferInput,m_numBufferInput*sizeof(BufferInput*));
			}
			else 
			{
				memcpy(ppBI,m_ppBufferInput,_count*sizeof(BufferInput*));
				for(int32_t i = _count;i<m_numBufferInput;i++)
				{
					delete m_ppBufferInput[i];
				}
			}
			delete[] m_ppBufferInput;
			m_ppBufferInput = ppBI;
			m_numBufferInput = _count;
		}
		void CDocumentInput::reserveReaders(int32_t _count)
		{
			if(_count == m_numReaderInput)
				return;
			ReaderInput** ppRI = new ReaderInput*[_count];
			memset(ppRI,0,_count*sizeof(ReaderInput*));
			if(_count > m_numReaderInput)
			{
				memcpy(ppRI,m_ppReaderInput,m_numReaderInput*sizeof(ReaderInput*));
			}
			else 
			{
				memcpy(ppRI,m_ppReaderInput,_count*sizeof(ReaderInput*));
				for(int32_t i = _count;i<m_numReaderInput;i++)
				{
					delete m_ppReaderInput[i];
				}
			}
			delete[] m_ppReaderInput;
			m_ppReaderInput = ppRI;
			m_numReaderInput = _count;
		}		
		firtex::plugin::IDocumentInput* CDocumentInput::wrapInterface()
		{
			return firtex::plugin::CComDocumentInput::wrap(this);
		}
		firtex::plugin::IDocumentInput*	 CDocumentInput::getInterface()
		{
			if(!m_pInterface)
				m_pInterface = wrapInterface();
			return m_pInterface;
		}


		//////////////////////////////////////////////////////////////////////////
		//CDocument	
		
		CDocument::CDocument(int32_t numFields)
			:m_fBoost(1.0)
			,m_bModified(false)
			,m_pDocSchema(NULL)			
			,m_bSchemaOwner(false)			
			,m_docEncoding(charset::UNKNOWN)
			,m_docType(doc_type::UNKNOWN)
			,m_docStatus(doc_status::UNPARSED)
			,m_pInput(new CDocumentInput())
			,m_ppFields(NULL)
			,m_numFields(0)
			,m_maxFields(0)
			,m_ppInherentFields(NULL)
			,m_numInherentFields(numFields)
			,m_pInterface(NULL)
		{
			m_ppInherentFields = new EXTEND_FIELD[m_numInherentFields];
			memset(m_ppInherentFields,0,m_numInherentFields*sizeof(EXTEND_FIELD));
		}
		CDocument::CDocument(CDocumentSchema* pSchema)
			:m_fBoost(1.0)
			,m_bModified(false)
			,m_pDocSchema(NULL)			
			,m_bSchemaOwner(false)			
			,m_docEncoding(charset::UNKNOWN)
			,m_docType(doc_type::UNKNOWN)
			,m_docStatus(doc_status::UNPARSED)
			,m_pInput(new CDocumentInput())
			,m_ppFields(NULL)
			,m_numFields(0)
			,m_maxFields(0)
			,m_ppInherentFields(NULL)
			,m_numInherentFields(0)
			,m_pInterface(NULL)
		{
			setSchema(pSchema);
		}

		CDocument::~CDocument()
		{
			clear();
			clearInherent();
			if(m_ppFields)
			{
				delete[] m_ppFields;
				m_ppFields = NULL;
			}
			if(m_pDocSchema && m_bSchemaOwner)
			{
				delete m_pDocSchema;
				m_pDocSchema = NULL;
			}		
			if(m_pInput)
			{
				delete m_pInput;
				m_pInput = NULL;
			}			
			if(m_pInterface)
			{
				m_pInterface->Release();
				m_pInterface = NULL;
			}
		}				

		CField*	CDocument::getField(const tchar* name)
		{
			for (int32_t i = 0;i<m_numFields;i++)
			{
				if(_tcsicmp(m_ppFields[i].field->getName().c_str(),name)==0)
				{
					return m_ppFields[i].field;
					break;
				}
			}			
			return NULL;
		}		

		void CDocument::clear()
		{
			for (int32_t i = 0;i<m_numFields;i++)
			{
				if(m_ppFields[i].extend==true)
				{
					delete m_ppFields[i].field;					
				}
				m_ppFields[i].field = NULL;
			}			
			for (int32_t i = 0;i<m_numInherentFields;i++)
			{
				m_ppInherentFields[i].extend = false;				
			}		

			m_numFields = 0;
			setStatus(doc_status::UNPARSED);
			setModify();
		}
	
		void CDocument::setSchema(CDocumentSchema* pSchema)
		{			
			if(m_pDocSchema && m_bSchemaOwner)
				delete m_pDocSchema;
			m_pDocSchema = pSchema;
			m_bSchemaOwner = false;

			clear();
			clearInherent();

			if(m_pDocSchema == NULL)
				return;

			CSchemaItem* pSchemaItem;
			m_numInherentFields = 0;
			m_pDocSchema->startIterator();
			while (m_pDocSchema->hasNext())
			{
				pSchemaItem = m_pDocSchema->next();
				if(pSchemaItem->getId() != -1)
					m_numInherentFields++;
			}			
			if(m_numInherentFields > 0)
			{
				m_ppInherentFields = new EXTEND_FIELD[m_numInherentFields];
				memset(m_ppInherentFields,0,m_numInherentFields*sizeof(EXTEND_FIELD));
				int32_t cur = 0;
				CField* pField;				
				m_pDocSchema->startIterator();
				while (m_pDocSchema->hasNext())
				{
					pSchemaItem = m_pDocSchema->next();
					if(pSchemaItem->getId() != -1)
					{
						pField = new CField(pSchemaItem->getId(),pSchemaItem->getName(),pSchemaItem->getStore(),pSchemaItem->getIndex(),pSchemaItem->getForwardIndex());
						m_ppInherentFields[cur].field = pField;	
						m_ppInherentFields[cur++].extend = false; /// free 
					}					
				}
			}	
		}

		void CDocument::updateSchema(CDocumentSchema* pSchema)
		{
			if(pSchema && pSchema != m_pDocSchema)
			{
				if(m_pDocSchema && m_bSchemaOwner)
					delete m_pDocSchema;
				m_pDocSchema = pSchema;
				m_bSchemaOwner = false;
			}
			if(m_pDocSchema == NULL)
				return;		

			CSchemaItem* pSchemaItem;
			int32_t numInherentFields = 0;
			m_pDocSchema->startIterator();
			while (m_pDocSchema->hasNext())
			{
				pSchemaItem = m_pDocSchema->next();
				if(pSchemaItem->getId() != -1)
					numInherentFields++;
			}			
			if(numInherentFields > 0)
			{
				EXTEND_FIELD* ppInherentFields = new EXTEND_FIELD[numInherentFields];
				memset(ppInherentFields,0,numInherentFields*sizeof(EXTEND_FIELD));
				int32_t cur = 0;
				CField* pField;				
				m_pDocSchema->startIterator();
				while (m_pDocSchema->hasNext())
				{
					pSchemaItem = m_pDocSchema->next();
					if(pSchemaItem->getId() != -1)
					{
						if(cur < m_numInherentFields)
						{
							ppInherentFields[cur].field = m_ppInherentFields[cur].field;
							ppInherentFields[cur++].extend= m_ppInherentFields[cur].extend;
						}
						else
						{
							pField = new CField(pSchemaItem->getId(),pSchemaItem->getName(),pSchemaItem->getStore(),pSchemaItem->getIndex(),pSchemaItem->getForwardIndex());
							ppInherentFields[cur].field = pField;	
							ppInherentFields[cur++].extend = false; /// free 
						}						
					}					
				}
				delete[] m_ppInherentFields;
				m_ppInherentFields = ppInherentFields;
				m_numInherentFields = numInherentFields;
			}	
		}

		firtex::plugin::IDocument* CDocument::wrapInterface()
		{
			return firtex::plugin::CComDocument::wrap(this);
		}
		firtex::plugin::IDocument* CDocument::getInterface()
		{
			if(!m_pInterface)
				m_pInterface = wrapInterface();
			return m_pInterface;
		}
		void CDocument::growFields()
		{
			int32_t newNumFields = m_maxFields + 5;
			EXTEND_FIELD* ppNewFields = new EXTEND_FIELD[newNumFields];
			memset(ppNewFields,0,newNumFields*sizeof(EXTEND_FIELD));
			for (int32_t i = 0;i<m_numFields;i++)
			{
				ppNewFields[i].field = m_ppFields[i].field;
				ppNewFields[i].extend = m_ppFields[i].extend;
			}
			delete[] m_ppFields;
			m_ppFields = ppNewFields;
			m_maxFields = newNumFields;
            
		}
		void CDocument::reserveFields(int32_t numFields)
		{
			clear();
			clearInherent();
			
			m_ppInherentFields = new EXTEND_FIELD[numFields];
			memset(m_ppInherentFields,0,numFields*sizeof(EXTEND_FIELD));			
			m_numInherentFields = numFields;				
		}
		void CDocument::clearInherent()
		{
			for(int32_t i = 0;i<m_numInherentFields;i++)
			{
				delete m_ppInherentFields[i].field;
			}
			delete[] m_ppInherentFields;
			m_ppInherentFields = NULL;
			m_numInherentFields = 0;
		}
	}
}
