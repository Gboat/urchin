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
// Created	: 2006/7/02
//
#include "index/ForwardIndexWriter.h"
#include "index/FieldFactory.h"

using namespace firtex::document;
namespace firtex
{
	namespace index
	{
		CForwardIndexWriter::CForwardIndexWriter(CDirectory* pDirectory)
			: m_pDirectory(pDirectory)
			,m_pTVIOutput(NULL)
			,m_pTVVOutput(NULL)
			,m_pTVDOutput(NULL)
			,m_pSequenceWriter(NULL)
			,m_pFreqWriter(NULL)
			,m_pPositionWriter(NULL)
			,m_pFieldFactories(NULL)
		{
		}

		CForwardIndexWriter::~CForwardIndexWriter(void)
		{
			close();
			m_pDirectory = NULL;			
		}			
		void CForwardIndexWriter::open(const tchar* barrel)
		{
			close();
			tstring s = barrel;			
			m_pTVIOutput = m_pDirectory->createOutput(s + _T(".fim"));
			m_pTVDOutput = m_pDirectory->createOutput(s + _T(".fid"));
			m_pTVVOutput = m_pDirectory->createOutput(s + _T(".fiv"));
		}
		void CForwardIndexWriter::addDocument(document::CDocument* pDoc)
		{
			fieldid_t fid  = 0;
			int numFields = 0;
			CField* pField = NULL;			

			//Position of Term Vector
			m_pTVIOutput->seek(pDoc->getDocID() * sizeof(fileoffset_t));
			m_pTVIOutput->writeLong(m_pTVDOutput->getFilePointer());//<DocumentPosition(Int64)>

			pDoc->startIterator();			
			while(pDoc->hasNext())
			{
				pField = pDoc->next();				
				if(pField->isStoreForwardIndex())
				{
					numFields ++;
				}				
			}			
			m_pTVDOutput->writeVInt(numFields);//<NumFields(VInt32)>

			pDoc->startIterator();			
			while(pDoc->hasNext())
			{
				pField = pDoc->next();	
                if(pField->isStoreForwardIndex())
				{
					CTokens* pTokens = pField->tokensValue();
					if(!pTokens)
						return;
					m_pTVDOutput->writeVInt(pField->getID());					//<FieldNum(VInt32)>
					m_pTVDOutput->writeVLong(m_pTVVOutput->getFilePointer());	//<FieldPositions(VInt64)>
					switch(pField->getForwardIndex())
					{
						case ForwardIndex::SEQUENCE:
							{
								if(!m_pSequenceWriter)
									m_pSequenceWriter = m_pFieldFactories->getFactory(pField->getID())->createTermSequenceVector();
								if(m_pSequenceWriter == NULL)
									FIRTEX_CLOG(level::warn)<<_T("CForwardIndexWriter::addDocument()-> No correspond Term Freq Vector Writer.") << FIRTEX_ENDL;
								m_pSequenceWriter->addField(m_pTVVOutput,pField);								
							}
							break;
						case ForwardIndex::FREQ:
							{
								if(!m_pFreqWriter)
									m_pFreqWriter = m_pFieldFactories->getFactory(pField->getID())->createTermFreqVector();
								if(m_pFreqWriter == NULL)
									FIRTEX_CLOG(level::warn)<<_T("CForwardIndexWriter::addDocument()-> No correspond Term Freq Vector Writer.") << FIRTEX_ENDL;
								m_pFreqWriter->addField(m_pTVVOutput,pField);
							}
							break;
						case ForwardIndex::POSITION:
							{
								if(!m_pPositionWriter)
									m_pPositionWriter = m_pFieldFactories->getFactory(pField->getID())->createTermPositionVector();
								if(m_pPositionWriter == NULL)
									FIRTEX_CLOG(level::warn)<<_T("CForwardIndexWriter::addDocument()-> No correspond Term Position Vector Writer.") << FIRTEX_ENDL;
								m_pPositionWriter->addField(m_pTVVOutput,pField);								
							}
							break;
					}
				}
			}
		}
		void CForwardIndexWriter::close()
		{
			if(m_pTVIOutput)
			{
				delete m_pTVIOutput;
				m_pTVIOutput = NULL;
			}
			if(m_pTVDOutput)
			{
				delete m_pTVDOutput;
				m_pTVDOutput = NULL;
			}
			if(m_pTVVOutput)
			{
				delete m_pTVVOutput;
				m_pTVVOutput = NULL;
			}

			if(m_pSequenceWriter)
			{
				delete m_pSequenceWriter;
				m_pSequenceWriter = NULL;
			}
			if(m_pFreqWriter)
			{
				delete m_pFreqWriter;
				m_pFreqWriter = NULL;
			}
			if(m_pPositionWriter)
			{
				delete m_pPositionWriter;
				m_pPositionWriter = NULL;
			}			
		}
	}
}
