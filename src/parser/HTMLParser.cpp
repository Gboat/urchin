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
// Created	: 2007/5/1
//

#include "parser/HTMLParser.h"

namespace firtex
{
	namespace parser
	{
		CHTMLParser::CHTMLParser()
			: m_pReadBuffer(NULL)
			, m_nReadBufferSize(0)
			, m_nFileSize(0)
			, m_pTextBuffer(NULL)
			, m_nBufferSize(0)
			, m_nDataSize (0)
			, m_nTextId(0)
		{
		}


		CHTMLParser::~CHTMLParser()
		{
			if(m_pTextBuffer != NULL)
			{
				delete[] m_pTextBuffer;
				m_pTextBuffer = NULL;
			}	

			if(m_pReadBuffer)
			{
				delete m_pReadBuffer;
				m_pReadBuffer = NULL;
			}
		}

		schemaid_t CHTMLParser::defineSchema(firtex::document::CDocumentSchema* pSchema)
		{	
			m_nTextId = pSchema->addItem(getField(),getIndex(),getStore(),getForwardIndex());
			return m_nTextId;
		}

		bool CHTMLParser::parseInternal(firtex::document::CDocumentInput* pDocInput,firtex::document::CDocument* pResultDoc)
		{
			firtex::document::CDocumentInput::PathInput* pPI = pDocInput->getPathInput();			
			if(pPI)
			{				
				parse(pPI->docPath.c_str());
				if(m_nDataSize <= 0)
					return false;
				pResultDoc->addField(pPI->sid,m_pTextBuffer,m_nDataSize,true);
				return true;
			}

			int32_t numBuffers;
			firtex::document::CDocumentInput::BufferInput** ppBI = pDocInput->getBufferInputs(numBuffers);
			if(numBuffers > 0)
			{				
				for(int32_t i = 0 ;i<numBuffers;i++)
				{	
					parse(ppBI[i]->buffer,ppBI[i]->length);
					pResultDoc->addField(ppBI[i]->sid,m_pTextBuffer,m_nDataSize,true);
				}
			}

			int32_t numReaders;
			firtex::document::CDocumentInput::ReaderInput** ppRI = pDocInput->getReaderInputs(numReaders);
			if(numReaders > 0)
			{
				/*for(int32_t ii = 0 ;ii<numReaders;ii++)
				{					
				pResultDoc->addField(ppRI[ii]->sid,ppRI[ii]->reader,false);
				}*/
				//TODO:
			}
			return true;
		}		

		void CHTMLParser::parse(const char *szHTML, size_t nLen)
		{
			if(szHTML == NULL || nLen <= 0)
				return ;

			m_nDataSize = 0;
			if(m_pTextBuffer == NULL)
			{
				m_nBufferSize = nLen + 1;
				if(m_nBufferSize < HTMLPARSER_BUFFERGROW)
					m_nBufferSize = HTMLPARSER_BUFFERGROW;
				m_pTextBuffer = new char[m_nBufferSize];
			}
			else if(m_nBufferSize < nLen + 1)
			{
				m_nBufferSize += HTMLPARSER_BUFFERGROW;
				if(m_nBufferSize < nLen + 1)
					m_nBufferSize = nLen + 1;
				delete[] m_pTextBuffer;
				m_pTextBuffer = new char[m_nBufferSize];
			}			

			register char* pCur = (char*)szHTML;
			char* pEnd = (char*)szHTML + nLen;
			parseBody(pCur,pEnd);
			m_pTextBuffer[m_nDataSize] = 0;
		}	

		void CHTMLParser::parse(const tchar* htmlFile)
		{
			int32_t fileHandle = ::fileOpen(htmlFile, O_BINARY | O_RDONLY | O_RANDOM, _S_IREAD );
			if (fileHandle < 0)
			{
				FIRTEX_CLOG(level::warn) << _T("Open file error:") << htmlFile << FIRTEX_ENDL;
				return ;
			}			
			m_nFileSize = (size_t)fileSize(fileHandle);
			if(m_nFileSize > MAX_FILESIZE - 1)
			{				
				m_nFileSize = MAX_FILESIZE - 1;
			}

			if(!m_pReadBuffer)
			{
				m_nReadBufferSize = DEFAULT_READBUFFER_SIZE;
				if(m_nReadBufferSize < m_nFileSize + 1)
					m_nReadBufferSize = m_nFileSize + 1;
				m_pReadBuffer = new char[m_nReadBufferSize];
			}
			else if(m_nFileSize + 1 > m_nReadBufferSize)
			{				
				m_nReadBufferSize = m_nFileSize + 1;				
				delete[] m_pReadBuffer;
				m_pReadBuffer = new char[m_nReadBufferSize];
			}
			size_t nRet = fileRead(fileHandle,m_pReadBuffer,(int)m_nFileSize);
			if(nRet != m_nFileSize)
			{
				FIRTEX_CLOG(level::warn) << _T("Read file error:") << htmlFile << FIRTEX_ENDL;
				fileClose(fileHandle);
				return;
			}
			fileClose(fileHandle);
			parse(m_pReadBuffer,m_nFileSize);
		}

		const char* CHTMLParser::getFullText(size_t& nLength)
		{
			nLength = m_nDataSize;
			return m_pTextBuffer;
		}
	}
}
