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
// Email	: ruijieguo@software.ict.ac.cn,ruijieguo@gmail.com
// Created	: 2005/11/20
//
#include "parser/PlainParser.h"
#include "utility/Config.h"
#include "index/IndexWriter.h"


using namespace firtex::index;


namespace firtex
{
	namespace parser
	{
		CPlainParser::CPlainParser()
		{			
			m_pReader = NULL;
		
			m_nTextId = 0;
		}
		CPlainParser::CPlainParser(const tchar* field)
			: CParser(field)	
		{
			m_pReader = NULL;			
			m_nTextId = 0;
		}
		CPlainParser::~CPlainParser()
		{		
			if(m_pReader)
			{
				delete m_pReader;
				m_pReader = NULL;
			}			
		}

		schemaid_t CPlainParser::defineSchema(CDocumentSchema* pSchema)
		{
			m_nTextId = pSchema->addItem(getField(),getIndex(),getStore(),getForwardIndex());
			return m_nTextId;
		}

		bool CPlainParser::parseInternal(document::CDocumentInput* pDocInput,document::CDocument* pResultDoc)
		{
			document::CDocumentInput::PathInput* pPI = pDocInput->getPathInput();			
			if(pPI)
			{				

				CField* pField = pResultDoc->addField(pPI->sid);
				FIRTEX_ASSERT(pField,_T("Assert failed:CPlainParser::parseInternal()"));
				CFileReader* pReader = (CFileReader*)pField->readerValue();
				if(!pReader)
				{
					pReader = new CFileReader();
					pField->readerValue(pReader,true);
				}
				if(pReader->open(pPI->docPath.c_str()))
				{					
					if(pReader->length() == 0)
						return false;
					return true;
				}	
				else
				{
					FIRTEX_CLOG(level::err) << _T("Open doc file error: ") << pPI->docPath.c_str() << FIRTEX_ENDL;
					return false;
				}
			}

			int32_t numBuffers;
			document::CDocumentInput::BufferInput** ppBI = pDocInput->getBufferInputs(numBuffers);
			if(numBuffers > 0)
			{
				for(int32_t i = 0 ;i<numBuffers;i++)
				{					
					pResultDoc->addField(ppBI[i]->sid,ppBI[i]->buffer,ppBI[i]->length,true);
				}
			}
			
			int32_t numReaders;
			document::CDocumentInput::ReaderInput** ppRI = pDocInput->getReaderInputs(numReaders);
			if(numReaders > 0)
			{
				for(int32_t ii = 0 ;ii<numReaders;ii++)
				{					
					pResultDoc->addField(ppRI[ii]->sid,ppRI[ii]->reader,false);
				}
			}
			return true;
		}				
	}
}
