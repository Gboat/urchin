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
// Created	: 2006/7/14
//
#include "search/SortFieldCacheImpl.h"
#include "index/TermIterator.h"
#include "index/TermReader.h"
#include <memory>

using namespace firtex::index;

namespace firtex
{
	namespace search
	{
		CSortFieldCacheImpl::CSortFieldCacheImpl(void)
		{
		}

		CSortFieldCacheImpl::~CSortFieldCacheImpl(void)
		{
		}
		CSortFieldCompArray* CSortFieldCacheImpl::getCompArray(CIndexReader* pReader, const tchar* field)
		{
			CSortFieldCompArray* pArray = getCachedArray(field);
			if(pArray)
				return pArray;
			CFieldsInfo& fieldsInfo = pReader->getFieldsInfo();
			CFieldInfo* pInfo = fieldsInfo.getField(field);
			if(pInfo == NULL)
			{				
				FIRTEX_CLOG(level::warn) << _T("No field info of ") << field << FIRTEX_ENDL;
				return NULL;
			}			
			token_t ttype = pInfo->getTokenType();
			
			switch(ttype)
			{
			case firtex::utility::CNumber::INT8:
				pArray = getI1(pReader,pInfo);
				break;
			case firtex::utility::CNumber::UINT8:
				pArray = getUI1(pReader,pInfo);
				break;
			case firtex::utility::CNumber::INT16:
				pArray = getI2(pReader,pInfo);
			case firtex::utility::CNumber::UINT16:
				pArray = getUI2(pReader,pInfo);
				break;
			case firtex::utility::CNumber::INT32:
				pArray = getI4(pReader,pInfo);
				break;
			case firtex::utility::CNumber::UINT32:
				pArray = getUI4(pReader,pInfo);
				break;
			case firtex::utility::CNumber::INT64:
				pArray = getI8(pReader,pInfo);
				break;
			case firtex::utility::CNumber::UINT64:
				pArray = getUI8(pReader,pInfo);
				break;
			case firtex::utility::CNumber::FLOAT:
				pArray = getR4(pReader,pInfo);
				break;
			case firtex::utility::CNumber::DOUBLE:
				pArray = getR8(pReader,pInfo);
				break;				
			default:
				FIRTEX_CLOG(level::warn) << _T("Only numeric field can use as a sort field.") << FIRTEX_ENDL;
				break;
			}
			if(pArray)
				storeInCache(field,pArray);
			return pArray;		
		}

		CSortFieldCompArray* CSortFieldCacheImpl::getI1(CIndexReader* pReader,CFieldInfo* pFieldInfo)
		{			
			int8_t* i8array  = NULL;
			int32_t len = pReader->maxDoc();
			if(len <= 0)
				return NULL;
			try
			{
				i8array = new int8_t[len];
				memset(i8array,0,len*sizeof(int8_t));
				CTermDocFreqs* pTermDocFreqs = NULL;

				firtex::index::CTermReader* pTermReader = pReader->termReader();
				std::auto_ptr<firtex::index::CTermReader> termreader_ptr;

				CTermIterator* pTermIterator = pTermReader->termIterator(pFieldInfo->getName());
				std::auto_ptr<firtex::index::CTermIterator> it_ptr(pTermIterator);

				CTerm* pTerm = NULL;
				while (pTermIterator->next())
				{
					pTerm = (CTerm*)pTermIterator->term();
					if(!pTermDocFreqs)
					{
						pTermReader->seek(pTerm);
						pTermDocFreqs = pTermReader->termDocFreqs();
						while (pTermDocFreqs->next())
						{
							i8array[pTermDocFreqs->doc()] = ((CTermX<int8_t>*)pTerm)->getValue();
						}
					}
					else if(pTermDocFreqs->seek(pTerm))
					{
						while (pTermDocFreqs->next())
						{
							i8array[pTermDocFreqs->doc()] = ((CTermX<int8_t>*)pTerm)->getValue();
						}
					}					
				}
				CSortFieldCompArray* pArray = new CSortFieldCompArray();
				pArray->arrayType = CSortFieldCompArray::I1_ARRAY;
				pArray->arrayLen = len;
				pArray->i8array = i8array;
				return pArray;
			}
			catch (std::bad_alloc& ) 
			{
				if(i8array)
					delete[] i8array;
				FIRTEX_THROW3(OUTOFMEM_ERROR,_T("CSortFieldCacheImpl::getI1()"));
			}
			catch (CFirteXException& e)
			{
				if(i8array)
					delete[] i8array;
				FIRTEX_RETHROW(e);
			}
		}
		CSortFieldCompArray* CSortFieldCacheImpl::getUI1(CIndexReader* pReader, CFieldInfo* pFieldInfo)
		{
			uint8_t* ui8array  = NULL;
			int32_t len = pReader->maxDoc();
			if(len <= 0)
				return NULL;
			try
			{
				ui8array = new uint8_t[len];
				memset(ui8array,0,len*sizeof(uint8_t));
				CTermDocFreqs* pTermDocFreqs = NULL;
				

				firtex::index::CTermReader* pTermReader = pReader->termReader();
				std::auto_ptr<firtex::index::CTermReader> termreader_ptr;
				CTermIterator* pTermIterator = pTermReader->termIterator(pFieldInfo->getName());
				std::auto_ptr<firtex::index::CTermIterator> it_ptr(pTermIterator);

				CTerm* pTerm = NULL;
				while (pTermIterator->next())
				{
					pTerm = (CTerm*)pTermIterator->term();
					if(!pTermDocFreqs)
					{
						pTermReader->seek(pTerm);
						pTermDocFreqs = pTermReader->termDocFreqs();
						while (pTermDocFreqs->next())
						{
							ui8array[pTermDocFreqs->doc()] = ((CTermX<uint8_t>*)pTerm)->getValue();
						}
					}
					else if(pTermDocFreqs->seek(pTerm))
					{
						while (pTermDocFreqs->next())
						{
							ui8array[pTermDocFreqs->doc()] = ((CTermX<uint8_t>*)pTerm)->getValue();
						}
					}					
				}
				CSortFieldCompArray* pArray = new CSortFieldCompArray();
				pArray->arrayType = CSortFieldCompArray::UI1_ARRAY;
				pArray->arrayLen = len;
				pArray->ui8array = ui8array;
				return pArray;
			}
			catch (std::bad_alloc& ) 
			{
				if(ui8array)
					delete[] ui8array;
				FIRTEX_THROW3(OUTOFMEM_ERROR,_T("CSortFieldCacheImpl::getUI1()"));
			}
			catch (CFirteXException& e)
			{
				if(ui8array)
					delete[] ui8array;
				FIRTEX_RETHROW(e);
			}
		}
		CSortFieldCompArray* CSortFieldCacheImpl::getI2(CIndexReader* pReader, CFieldInfo* pFieldInfo)
		{
			int16_t* i16array  = NULL;
			int32_t len = pReader->maxDoc();
			if(len <= 0)
				return NULL;
			try
			{
				i16array = new int16_t[len];
				memset(i16array,0,len*sizeof(int16_t));
				CTermDocFreqs* pTermDocFreqs = NULL;

				firtex::index::CTermReader* pTermReader = pReader->termReader();
				std::auto_ptr<firtex::index::CTermReader> termreader_ptr;

				CTermIterator* pTermIterator = pTermReader->termIterator(pFieldInfo->getName());
				std::auto_ptr<firtex::index::CTermIterator> it_ptr(pTermIterator);

				CTerm* pTerm = NULL;
				while (pTermIterator->next())
				{
					pTerm = (CTerm*)pTermIterator->term();
					if(!pTermDocFreqs)
					{
						pTermReader->seek(pTerm);
						pTermDocFreqs = pTermReader->termDocFreqs();
						while (pTermDocFreqs->next())
						{
							i16array[pTermDocFreqs->doc()] = ((CTermX<int16_t>*)pTerm)->getValue();
						}
					}
					else if(pTermDocFreqs->seek(pTerm))
					{
						while (pTermDocFreqs->next())
						{
							i16array[pTermDocFreqs->doc()] = ((CTermX<int16_t>*)pTerm)->getValue();
						}
					}					
				}
				CSortFieldCompArray* pArray = new CSortFieldCompArray();
				pArray->arrayType = CSortFieldCompArray::I2_ARRAY;
				pArray->arrayLen = len;
				pArray->i16array = i16array;
				return pArray;
			}
			catch (std::bad_alloc& ) 
			{
				if(i16array)
					delete[] i16array;
				FIRTEX_THROW3(OUTOFMEM_ERROR,_T("CSortFieldCacheImpl::getI1()"));
			}
			catch (CFirteXException& e)
			{
				if(i16array)
					delete[] i16array;
				FIRTEX_RETHROW(e);
			}

		}
		CSortFieldCompArray* CSortFieldCacheImpl::getUI2(CIndexReader* pReader, CFieldInfo* pFieldInfo)
		{
			uint16_t* ui16array  = NULL;
			int32_t len = pReader->maxDoc();
			if(len <= 0)
				return NULL;
			try
			{
				ui16array = new uint16_t[len];
				memset(ui16array,0,len*sizeof(uint16_t));
				CTermDocFreqs* pTermDocFreqs = NULL;

				firtex::index::CTermReader* pTermReader = pReader->termReader();
				std::auto_ptr<firtex::index::CTermReader> termreader_ptr;

				CTermIterator* pTermIterator = pTermReader->termIterator(pFieldInfo->getName());
				std::auto_ptr<firtex::index::CTermIterator> it_ptr(pTermIterator);

				CTerm* pTerm = NULL;
				while (pTermIterator->next())
				{
					pTerm = (CTerm*)pTermIterator->term();
					if(!pTermDocFreqs)
					{
						pTermReader->seek(pTerm);
						pTermDocFreqs = pTermReader->termDocFreqs();
						while (pTermDocFreqs->next())
						{
							ui16array[pTermDocFreqs->doc()] = ((CTermX<uint16_t>*)pTerm)->getValue();
						}
					}
					else if(pTermDocFreqs->seek(pTerm))
					{
						while (pTermDocFreqs->next())
						{
							ui16array[pTermDocFreqs->doc()] = ((CTermX<uint16_t>*)pTerm)->getValue();
						}
					}					
				}
				CSortFieldCompArray* pArray = new CSortFieldCompArray();
				pArray->arrayType = CSortFieldCompArray::UI2_ARRAY;
				pArray->arrayLen = len;
				pArray->ui16array = ui16array;
				return pArray;
			}
			catch (std::bad_alloc& ) 
			{
				if(ui16array)
					delete[] ui16array;
				FIRTEX_THROW3(OUTOFMEM_ERROR,_T("CSortFieldCacheImpl::getI1()"));
			}
			catch (CFirteXException& e)
			{
				if(ui16array)
					delete[] ui16array;
				FIRTEX_RETHROW(e);
			}
		}
		CSortFieldCompArray* CSortFieldCacheImpl::getI4(CIndexReader* pReader, CFieldInfo* pFieldInfo)
		{
			int32_t* i32array  = NULL;
			int32_t len = pReader->maxDoc();
			if(len <= 0)
				return NULL;
			try
			{
				i32array = new int32_t[len];
				memset(i32array,0,len*sizeof(int32_t));
				CTermDocFreqs* pTermDocFreqs = NULL;

				firtex::index::CTermReader* pTermReader = pReader->termReader();
				std::auto_ptr<firtex::index::CTermReader> termreader_ptr;

				CTermIterator* pTermIterator = pTermReader->termIterator(pFieldInfo->getName());
				std::auto_ptr<firtex::index::CTermIterator> it_ptr(pTermIterator);

				CTerm* pTerm = NULL;
				while (pTermIterator->next())
				{
					pTerm = (CTerm*)pTermIterator->term();
					if(!pTermDocFreqs)
					{
						pTermReader->seek(pTerm);
						pTermDocFreqs = pTermReader->termDocFreqs();
						while (pTermDocFreqs->next())
						{
							i32array[pTermDocFreqs->doc()] = ((CTermX<int32_t>*)pTerm)->getValue();
						}
					}
					else if(pTermDocFreqs->seek(pTerm))
					{
						while (pTermDocFreqs->next())
						{
							i32array[pTermDocFreqs->doc()] = ((CTermX<int32_t>*)pTerm)->getValue();
						}
					}					
				}
				CSortFieldCompArray* pArray = new CSortFieldCompArray();
				pArray->arrayType = CSortFieldCompArray::I4_ARRAY;
				pArray->arrayLen = len;
				pArray->i32array = i32array;
				return pArray;
			}
			catch (std::bad_alloc& ) 
			{
				if(i32array)
					delete[] i32array;
				FIRTEX_THROW3(OUTOFMEM_ERROR,_T("CSortFieldCacheImpl::getI4()"));
			}
			catch (CFirteXException& e)
			{
				if(i32array)
					delete[] i32array;
				FIRTEX_RETHROW(e);
			}
		}
		CSortFieldCompArray* CSortFieldCacheImpl::getUI4(CIndexReader* pReader, CFieldInfo* pFieldInfo)
		{
			uint32_t* ui32array  = NULL;
			int32_t len = pReader->maxDoc();
			if(len <= 0)
				return NULL;
			try
			{
				ui32array = new uint32_t[len];
				memset(ui32array,0,len*sizeof(uint32_t));
				CTermDocFreqs* pTermDocFreqs = NULL;

				firtex::index::CTermReader* pTermReader = pReader->termReader();
				std::auto_ptr<firtex::index::CTermReader> termreader_ptr;

				CTermIterator* pTermIterator = pTermReader->termIterator(pFieldInfo->getName());
				std::auto_ptr<firtex::index::CTermIterator> it_ptr(pTermIterator);

				CTerm* pTerm = NULL;
				while (pTermIterator->next())
				{
					pTerm = (CTerm*)pTermIterator->term();
					if(!pTermDocFreqs)
					{
						pTermReader->seek(pTerm);
						pTermDocFreqs = pTermReader->termDocFreqs();
						while (pTermDocFreqs->next())
						{
							ui32array[pTermDocFreqs->doc()] = ((CTermX<uint32_t>*)pTerm)->getValue();
						}
					}
					else if(pTermDocFreqs->seek(pTerm))
					{
						while (pTermDocFreqs->next())
						{
							ui32array[pTermDocFreqs->doc()] = ((CTermX<uint32_t>*)pTerm)->getValue();
						}
					}					
				}
				CSortFieldCompArray* pArray = new CSortFieldCompArray();
				pArray->arrayType = CSortFieldCompArray::UI4_ARRAY;
				pArray->arrayLen = len;
				pArray->ui32array = ui32array;
				return pArray;
			}
			catch (std::bad_alloc& ) 
			{
				if(ui32array)
					delete[] ui32array;
				FIRTEX_THROW3(OUTOFMEM_ERROR,_T("CSortFieldCacheImpl::getUI4()"));
			}
			catch (CFirteXException& e)
			{
				if(ui32array)
					delete[] ui32array;
				FIRTEX_RETHROW(e);
			}
		}
		CSortFieldCompArray* CSortFieldCacheImpl::getI8(CIndexReader* pReader, CFieldInfo* pFieldInfo)
		{
			int64_t* i64array  = NULL;
			int32_t len = pReader->maxDoc();
			if(len <= 0)
				return NULL;
			try
			{
				i64array = new int64_t[len];
				memset(i64array,0,len*sizeof(int64_t));
				CTermDocFreqs* pTermDocFreqs = NULL;

				firtex::index::CTermReader* pTermReader = pReader->termReader();
				std::auto_ptr<firtex::index::CTermReader> termreader_ptr;

				CTermIterator* pTermIterator = pTermReader->termIterator(pFieldInfo->getName());
				std::auto_ptr<firtex::index::CTermIterator> it_ptr(pTermIterator);

				CTerm* pTerm = NULL;
				while (pTermIterator->next())
				{
					pTerm = (CTerm*)pTermIterator->term();
					if(!pTermDocFreqs)
					{
						pTermReader->seek(pTerm);
						pTermDocFreqs = pTermReader->termDocFreqs();
						while (pTermDocFreqs->next())
						{
							i64array[pTermDocFreqs->doc()] = ((CTermX<int64_t>*)pTerm)->getValue();
						}
					}
					else if(pTermDocFreqs->seek(pTerm))
					{
						while (pTermDocFreqs->next())
						{
							i64array[pTermDocFreqs->doc()] = ((CTermX<int64_t>*)pTerm)->getValue();
						}
					}					
				}
				CSortFieldCompArray* pArray = new CSortFieldCompArray();
				pArray->arrayType = CSortFieldCompArray::I8_ARRAY;
				pArray->arrayLen = len;
				pArray->i64array = i64array;
				return pArray;
			}
			catch (std::bad_alloc& ) 
			{
				if(i64array)
					delete[] i64array;
				FIRTEX_THROW3(OUTOFMEM_ERROR,_T("CSortFieldCacheImpl::getI8()"));
			}
			catch (CFirteXException& e)
			{
				if(i64array)
					delete[] i64array;
				FIRTEX_RETHROW(e);
			}
		}
		CSortFieldCompArray* CSortFieldCacheImpl::getUI8(CIndexReader* pReader, CFieldInfo* pFieldInfo)
		{
			uint64_t* ui64array = NULL;
			int32_t len = pReader->maxDoc();
			if(len <= 0)
				return NULL;
			try
			{
				ui64array = new uint64_t[len];
				memset(ui64array,0,len*sizeof(uint64_t));
				CTermDocFreqs* pTermDocFreqs = NULL;

				firtex::index::CTermReader* pTermReader = pReader->termReader();
				std::auto_ptr<firtex::index::CTermReader> termreader_ptr;

				CTermIterator* pTermIterator = pTermReader->termIterator(pFieldInfo->getName());
				std::auto_ptr<firtex::index::CTermIterator> it_ptr(pTermIterator);

				CTerm* pTerm = NULL;
				while (pTermIterator->next())
				{
					pTerm = (CTerm*)pTermIterator->term();
					if(!pTermDocFreqs)
					{
						pTermReader->seek(pTerm);
						pTermDocFreqs = pTermReader->termDocFreqs();
						while (pTermDocFreqs->next())
						{
							ui64array[pTermDocFreqs->doc()] = ((CTermX<uint64_t>*)pTerm)->getValue();
						}
					}
					else if(pTermDocFreqs->seek(pTerm))
					{
						while (pTermDocFreqs->next())
						{
							ui64array[pTermDocFreqs->doc()] = ((CTermX<uint64_t>*)pTerm)->getValue();
						}
					}					
				}
				CSortFieldCompArray* pArray = new CSortFieldCompArray();
				pArray->arrayType = CSortFieldCompArray::UI8_ARRAY;
				pArray->arrayLen = len;
				pArray->ui64array = ui64array;
				return pArray;
			}
			catch (std::bad_alloc& ) 
			{
				if(ui64array)
					delete[] ui64array;
				FIRTEX_THROW3(OUTOFMEM_ERROR,_T("CSortFieldCacheImpl::getI8()"));
			}
			catch (CFirteXException& e)
			{
				if(ui64array)
					delete[] ui64array;
				FIRTEX_RETHROW(e);
			}
		}
		CSortFieldCompArray* CSortFieldCacheImpl::getR4(CIndexReader* pReader, CFieldInfo* pFieldInfo)
		{
			float* r4array = NULL;
			int32_t len = pReader->maxDoc();
			if(len <= 0)
				return NULL;
			try
			{
				r4array = new float[len];
				memset(r4array,0,len*sizeof(float));
				CTermDocFreqs* pTermDocFreqs = NULL;

				firtex::index::CTermReader* pTermReader = pReader->termReader();
				std::auto_ptr<firtex::index::CTermReader> termreader_ptr;

				CTermIterator* pTermIterator = pTermReader->termIterator(pFieldInfo->getName());
				std::auto_ptr<firtex::index::CTermIterator> it_ptr(pTermIterator);

				CTerm* pTerm = NULL;
				while (pTermIterator->next())
				{
					pTerm = (CTerm*)pTermIterator->term();
					if(!pTermDocFreqs)
					{
						pTermReader->seek(pTerm);
						pTermDocFreqs = pTermReader->termDocFreqs();
						while (pTermDocFreqs->next())
						{
							r4array[pTermDocFreqs->doc()] = ((CTermX<float>*)pTerm)->getValue();
						}
					}
					else if(pTermDocFreqs->seek(pTerm))
					{
						while (pTermDocFreqs->next())
						{
							r4array[pTermDocFreqs->doc()] = ((CTermX<float>*)pTerm)->getValue();
						}
					}					
				}
				CSortFieldCompArray* pArray = new CSortFieldCompArray();
				pArray->arrayType = CSortFieldCompArray::R4_ARRAY;
				pArray->arrayLen = len;
				pArray->farray = r4array;
				return pArray;
			}
			catch (std::bad_alloc& ) 
			{
				if(r4array)
					delete[] r4array;
				FIRTEX_THROW3(OUTOFMEM_ERROR,_T("CSortFieldCacheImpl::getR4()"));
			}
			catch (CFirteXException& e)
			{
				if(r4array)
					delete[] r4array;
				FIRTEX_RETHROW(e);
			}
		}
		CSortFieldCompArray* CSortFieldCacheImpl::getR8(CIndexReader* pReader, CFieldInfo* pFieldInfo)
		{
			double* darray = NULL;
			int32_t len = pReader->maxDoc();
			if(len <= 0)
				return NULL;
			try
			{
				darray = new double[len];
				memset(darray,0,len*sizeof(double));
				CTermDocFreqs* pTermDocFreqs = NULL;

				firtex::index::CTermReader* pTermReader = pReader->termReader();
				std::auto_ptr<firtex::index::CTermReader> termreader_ptr;

				CTermIterator* pTermIterator = pTermReader->termIterator(pFieldInfo->getName());
				std::auto_ptr<firtex::index::CTermIterator> it_ptr(pTermIterator);

				CTerm* pTerm = NULL;
				while (pTermIterator->next())
				{
					pTerm = (CTerm*)pTermIterator->term();
					if(!pTermDocFreqs)
					{
						pTermReader->seek(pTerm);
						pTermDocFreqs = pTermReader->termDocFreqs();
						while (pTermDocFreqs->next())
						{
							darray[pTermDocFreqs->doc()] = ((CTermX<double>*)pTerm)->getValue();
						}
					}
					else if(pTermDocFreqs->seek(pTerm))
					{
						while (pTermDocFreqs->next())
						{
							darray[pTermDocFreqs->doc()] = ((CTermX<double>*)pTerm)->getValue();
						}
					}					
				}
				CSortFieldCompArray* pArray = new CSortFieldCompArray();
				pArray->arrayType = CSortFieldCompArray::R8_ARRAY;
				pArray->arrayLen = len;
				pArray->dbarray = darray;
				return pArray;
			}
			catch (std::bad_alloc& ) 
			{
				if(darray)
					delete[] darray;
				FIRTEX_THROW3(OUTOFMEM_ERROR,_T("CSortFieldCacheImpl::getR8()"));
			}
			catch (CFirteXException& e)
			{
				if(darray)
					delete[] darray;
				FIRTEX_RETHROW(e);
			}
		}
	}
}
